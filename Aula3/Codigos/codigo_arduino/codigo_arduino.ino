/*
  Software serial multple serial test

  Receives from the hardware serial, sends to software serial.
  Receives from software serial, sends to hardware serial.

  The circuit:
   RX is digital pin 10 (connect to TX of other device)
   TX is digital pin 11 (connect to RX of other device)

  Note:
  Not all pins on the Mega and Mega 2560 support change interrupts,
  so only the following can be used for RX:
  10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

  Not all pins on the Leonardo and Micro support change interrupts,
  so only the following can be used for RX:
  8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

  created back in the mists of time
  modified 25 May 2012
  by Tom Igoe
  based on Mikal Hart's example

  This example code is in the public domain.

*/
#include <SoftwareSerial.h>
#include <Servo.h>



unsigned long lastMsg = 0;
int lastFlex = 0;
SoftwareSerial esp(10, 11); // RX, TX
Servo s;

void mqttSubscribe(String topic) {
  delay(500);
  esp.print("sub:" + topic);
  bool sucesso = false;
  while (!sucesso) {
    if (esp.available()) {
      String m = esp.readStringUntil('\n');
      m.trim();
      Serial.println(m);
      sucesso = (m.indexOf("Escutando") >= 0);
      sucesso = sucesso && (m.indexOf(topic) >= 0);
    }
  }
}

void mqttUnsubscribe(String topic) {
  delay(500);
  esp.print("unsub:" + topic);
  bool sucesso = false;
  while (!sucesso) {
    if (esp.available()) {
      String m = esp.readStringUntil('\n');
      m.trim();
      Serial.println(m);
      sucesso = (m.indexOf("Deixando") >= 0);
      sucesso = sucesso && (m.indexOf(topic) >= 0);
    }
  }
}

void mqttMessage(String topic, String msg) {
  if (topic == "led" && msg == "1") {
    digitalWrite(2, true);
    Serial.println("ligando led");
  } else if (topic == "led" && msg == "0") {
    digitalWrite(2, false);
  }else if(topic == "servo"){
    int x = msg.toInt();
    s.write(x);
  }

}

void mqttConnected() {
  mqttSubscribe("led");
  mqttSubscribe("servo");
}

void setupSerial() {
  // Open serial communications and wait for port to open:
  digitalWrite(4, LOW);
  delay(100);
  digitalWrite(4, HIGH);
  delay(100);
  esp.begin(9600);
  Serial.begin(9600);

  Serial.println("Inicio Serial");


  // set the data rate for the SoftwareSerial port
  delay(100);
  esp.print("connectMqtt");

  bool mqtt = false;
  while (!mqtt) {

    if (esp.available()) {
      String msg = esp.readString();
      msg.trim();
      if (msg.indexOf("MQTT Conectado") >= 0) {
        mqtt = true;
        Serial.println("mqtt conectado");
        mqttConnected();
      }
    }
  }
}

void processEsp() {
  if (esp.available()) {
    String package = esp.readStringUntil('\n');
    package.trim();
    Serial.println("ESP: " + package);
    int split = package.indexOf("|");
    String topic = package.substring(0, split);
    String msg = package.substring(split + 1);
    mqttMessage(topic, msg);
  }
  if (Serial.available()) {
    esp.write(Serial.read());
  }
}

void setup() {
  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);
  s.attach(6);
  setupSerial();
}

void leiaLuz(){
  int x = analogRead(A0);
  if(millis() - lastMsg > 10000){
    esp.println("luz|" + String(x));
    Serial.println(String(x));
    lastMsg = millis();
  }
}


void loop() { // run over and over
  processEsp();
  leiaLuz();
}
