/*
  Basic ESP8266 MQTT example
  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP8266 board/library.
  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.
  To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "HCor_Guest";
const char* password = "hcorwifi";
const char* mqtt_server = "mqtt.lt.gwachs.com";


WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(topic);
  Serial.print("|");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println("");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT Conectado");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void enviaMqtt(String s) {
  int divisao = s.indexOf("|");
  String topico = s.substring(0, divisao);
  String msg = s.substring(divisao + 1);
  Serial.println("Enviando MQTT");
  Serial.println("Topico: " + topico);
  Serial.println("Msg: " + msg);
  client.publish(topico.c_str(), msg.c_str());
}


void subscribeMqtt(String s) {
  int divisao = s.indexOf("sub:") + 4;
  String topico = s.substring(divisao);
  
  if(client.subscribe(topico.c_str())){
    Serial.println("Escutando MQTT topico: "+topico);
  }else{
    Serial.println("Falha ao escutar topico: "+topico);
  }
  
}

void unsubscribeMqtt(String s) {
  int divisao = s.indexOf("sub:") + 4;
  String topico = s.substring(divisao);
  
  if(client.unsubscribe(topico.c_str())){
    Serial.println("Deixando de escutar MQTT topico: " + topico);
  } else{
    Serial.println("Falha ao deixar de escutar topico: "+topico);
  }
}

void loop() {

  
  client.loop();

  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    s.trim();
    if (s.indexOf("|") >= 0) {
      enviaMqtt(s);
    }else if(s.indexOf("unsub:") >= 0) {
      unsubscribeMqtt(s);
    }else if (s.indexOf("sub:") >= 0) {
      subscribeMqtt(s);
    }else if (s.indexOf("connectMqtt") >= 0){
      if (!client.connected()) {
        reconnect();
      }else{
        Serial.println("MQTT Conectado");
      }
    }else if (s.indexOf("reset") >= 0){
       ESP.reset();
    }
  }

}
