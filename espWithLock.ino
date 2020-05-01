#include <ESP8266WiFi.h> // Include the Wi-Fi library
#include <ESP8266WebServer.h>

const char *ssid = "cdac"; // The name of the Wi-Fi network that will be created
const char *password = ""; // The password required to connect to it, leave blank for an


ESP8266WebServer server(80);

int solenoidPin=0;

void setup() {
Serial.begin(115200); // Start the Serial communication to send messages to the computer
delay(10);
Serial.println('\n');
WiFi.begin(ssid, password); // Connect to the network
Serial.print("Connecting to ");
Serial.print(ssid); Serial.println(" ...");
int i = 0;
while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
delay(1000);
Serial.print(++i); Serial.print(' ');
}
pinMode(solenoidPin,OUTPUT);
Serial.println('\n');
Serial.println("Connection established!");
Serial.print("IP address:\t");
Serial.println(WiFi.localIP());


  
server.on("/",matched); 
  
server.begin();
Serial.println("HTTP server started");
}
void loop() {
  server.handleClient();
  }
void matched(){
  if(server.arg("checked") == "done"){
    Serial.println("ooooooo");
    digitalWrite(solenoidPin,HIGH);
    delay(3000);
    digitalWrite(solenoidPin,LOW);
    delay(3000);
    
       } 
  
  }
  
