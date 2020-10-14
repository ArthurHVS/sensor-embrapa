#include <SoftwareSerial.h>// Include da biblioteca de criação de pares RX/TX
SoftwareSerial BTSerial(7, 8);// RX/TX virtual, que serão a entrada/saída do módulo HC-05
void setup()
{
  Serial.begin(9600); // Inicia o par "RX/TX" padrão
  Serial.println("Digite um comando AT:"); 
  BTSerial.begin(38400);       // 38400 = Velocidade padrão do módulo HC-05
}
void loop()
{
  if (BTSerial.available())    // Verifica a disponibilidade do HC-05
    Serial.write(BTSerial.read()); /* Conecta a saída serial do Arduino com a entrada serial do módulo, escrevendo no monitor serial do Arduino o que está sendo lido pelo módulo */
  if (Serial.available())     // Verifica a disponibilidade do próprio Arduino
    BTSerial.write(Serial.read()); /* Escreve o comando na entrada serial
}
