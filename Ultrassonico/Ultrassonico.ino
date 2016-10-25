#include <Ultrasonic.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#define PINO_TRG  2
#define PINO_ECHO 0
//Inicializa o sensor ultrasonico nos pinos especificados
Ultrasonic ultrasonic(PINO_TRG, PINO_ECHO);

// replace with your channel’s thingspeak API key,
String apiKey = "Chave do meu canal no ThingSpeak";
const char* ssid = "Nome da minha rede";
const char* password = "Senha da minha rede";

const char* server = "api.thingspeak.com";
#define DHTPIN 2 // what pin we’re connected to

WiFiClient client;

void setup() {
Serial.begin(115200);
delay(10);
//dht.begin();

WiFi.begin(ssid, password);

Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");

}

void loop() {
  float demo;
  long microsec = ultrasonic.timing();  //Le o valor do sensor ultrassonico
  //Atribui os valores em cm ou polegadas as variaveis
  demo = ultrasonic.convert(microsec, Ultrasonic::CM);

if (client.connect(server,80)) { 
String postStr = apiKey;
postStr +="&field1=";
postStr += String(demo);
//postStr +="&field2=";
//postStr += String(h);
postStr += "\r\n\r\n";

client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);

Serial.print("Demo: ");
Serial.print(demo);
Serial.println("cm send to Thingspeak");
}
client.stop();

Serial.println("Waiting…");
// thingspeak needs minimum 15 sec delay between updates
delay(20000);
}
