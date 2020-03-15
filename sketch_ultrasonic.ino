
// Bibliotecas incluídas
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Ultrasonic.h>
#include <RTClib.h>

// Definições do relógio ds1307, para adquirirmos dados com estampas de tempo reais
#define DS1307_I2C_ADDRESS 0x68
#define TIME_MSG_LEN 11
#define TIME_HEADER 'T'

// Inicializa o arquivo que salvaremos a cada loop
File myFile;

// Definições da biblioteca Ultrasonic.h
#define pino_trigger 4
#define pino_echo 5

// Definição do tamanho de cada ciclo de medições, em segundos
#define TAM_CICLO 5

// Variáveis globais utilizadas pelo sensor, relógio do shield SD e para controlar a impressão na tela, respectivamente
Ultrasonic ultrasonic(pino_trigger, pino_echo);
RTC_Millis rtc;
int loops = 0;

// Variáveis globais para formatação dos números "float" para o arquivo.
char floatStr[9];
char dataStr[24];

// Funções de conversão decimal -> binário (codificado) e binário (codificado) -> decimal.
byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}

// Função de conversão float para uma string terminada em null
char *ftoa(char *a, double f, int precision)
{
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
  
  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long decimal = abs((long)((f - heiltal) * p[precision]));
  itoa(decimal, a, 10);
  return ret;
}



void setup()
{
  Wire.begin();
  Serial.begin(9600);
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  delay(3000);
  
  Serial.print("Iniciando cartão SD...");

  // Pino 10 precisa ser definido como output, senão as funções de SD.h não funcionam
  pinMode(10, OUTPUT);
  if (!SD.begin(10)) 
    {
      Serial.print("Cartão SD não inciou!");
      return;
    } 
  Serial.println("Inicialização completa.");
  floatStr[0] = '\0';   
  analogReference(INTERNAL);
}
 
void loop()
{
  loops++;
  float dist, dist_sum = 0.0;
  float cmMsec;
  long microsec = 0;
  
  DateTime agora = rtc.now();
  
  // Abre o arquivo csv. Note que só um arquivo pode ser aberto por vez, então precisamos fechá-lo toda vez que o loop for feito
  myFile = SD.open("leitura.csv", FILE_WRITE);
  
  // Se o arquivo abriu, escreva nele!:
  if (myFile && loops <= 1) 
  {
    Serial.println(F("Arquivo aberto para escrita."));
   } 
  if (!myFile)
  {
    // Se não abrir, imprima um erro:
    Serial.print("Erro abrindo o arquivo após ");
    Serial.print(loops);
    Serial.print(" loops");
    return;
  }
  
  for(int i=0; i< TAM_CICLO; i++)
  {
    microsec = ultrasonic.timing();
    cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);  

    dist_sum += cmMsec;

    delay(1000);
  }
  
  dist = dist_sum / TAM_CICLO;
  dist_sum = 0.0;

  // Imprime a data/hora no arquivo
  dataStr[0] = '\0';
  
  sprintf(dataStr, "%d/%d/%d, %d:%d:%d, ", agora.day(), agora.month(), agora.year(), agora.hour(), agora.minute(), agora.second());

  myFile.print(dataStr);
  Serial.print(dataStr);

  // Imprime a distância no arquivo.
  dataStr[0] = '\0';
  sprintf(dataStr, "%s", ftoa(floatStr, dist, 3));
  
  myFile.println(dataStr);
  Serial.println(dataStr);

  // Fecha o arquivo e finaliza o loop
  myFile.close();
}
