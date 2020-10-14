// Bibliotecas incluídas
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>

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
  analogReference(INTERNAL);
}
 
void loop()
{
  loops++;
  float cmMsec;
  long microsec = 0;
  int sensorValue = 0;
  DateTime agora = rtc.now();
  
  // Abre o arquivo csv. Note que só um arquivo pode ser aberto por vez, então precisamos fechá-lo toda vez que o loop for feito
  myFile = SD.open("leitura.csv", FILE_WRITE);
  
  // Se o arquivo abriu, escreva nele!:
  if (myFile && loops <= 1) 
  {
    //Serial.println(F("Arquivo aberto para escrita."));
   } 
  if (!myFile)
  {
    // Se não abrir, imprima um erro:
    Serial.print("Erro abrindo o arquivo após ");
    Serial.print(loops);
    .print(" loops");
    return;
  }
   
  // Lê o valor do sensor em A1
  sensorValue = analogRead(A1);
  // Imprime o valor que foi lido:
  Serial.println(sensorValue);
  delay(10);        // Delay entre leituras, para estabilidade (10 ms)


  // Imprime a data/hora no arquivo
  dataStr[0] = '\0';
  
  sprintf(dataStr, "%d/%d/%d, %d:%d:%d, ", agora.day(), agora.month(), agora.year(), agora.hour(), agora.minute(), agora.second());

  myFile.print(dataStr);
  //Serial.print(dataStr);

  // Imprime a pressao na string.
  sprintf(dataStr, "%d" , sensorValue);

  //Imprime a string no arquivo.
  myFile.println(dataStr);
  //Serial.println(dataStr);

  // Fecha o arquivo e finaliza o loop
  myFile.close();
}
