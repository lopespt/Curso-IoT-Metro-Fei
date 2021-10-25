
#include "SoftwareSerial.h"

SoftwareSerial ESP_Serial(10, 11); // RX, TX
int valorSensor;
int valorSensorAntigo = 0;
char buffer[1024];
unsigned long ultimoEnvio = 0;
void setup(){
  pinMode(10, INPUT);
  pinMode(A0, INPUT);
  pinMode(11, OUTPUT);
  Serial.begin(9600);
  ESP_Serial.begin(115200);
}

void envia(char * dados){
  Serial.print("Enviando ");
  Serial.print(dados);
  ESP_Serial.write(dados);
  
}

void loop(){
  if (ESP_Serial.available()){
    Serial.write(ESP_Serial.read());
  }
  
  if (Serial.available()){
    ESP_Serial.write(Serial.read());
  }

  valorSensor = analogRead(A0);
  valorSensor = map(valorSensor, 0,1023, 0, 10);

  if ( abs(valorSensorAntigo - valorSensor) >= 1 && abs(millis() - ultimoEnvio) > 2000){
     ultimoEnvio = millis();
     valorSensorAntigo = valorSensor;
     sprintf(buffer,"%d",valorSensor);
     envia(buffer);
  }  
}
