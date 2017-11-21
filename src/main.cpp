#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include "credentials.h"

const char* hostName = "GaragemX";
const char* update_path = "/firmware";
const char* update_username = "update";
const char* update_password = UPDATE_PASSWORD;
const char* ssid = SSID;
const char* password = PASSWORD;

IPAddress ip(10,1,1,52);  

int status1 = 0;
int status2 = 0;



ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void handleRoot() {
  httpServer.send(200, "text/plain", "hello from esp8266!");
}

void handleNotFound(){

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += httpServer.uri();
  message += "\nMethod: ";
  message += (httpServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += httpServer.args();
  message += "\n";
  for (uint8_t i=0; i<httpServer.args(); i++){
    message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\n";
  }
  httpServer.send(404, "text/plain", message);
}


void setup(void){
  pinMode(D1, OUTPUT); 
  pinMode(D2, OUTPUT); 
  Serial.begin(115200);
//  delay(200);
//  Serial.println();
//  Serial.println("Booting Sketch...");
  IPAddress gateway(10, 1,1, 1);  
  IPAddress subnet(255, 255, 255, 0);  
  WiFi.config(ip, gateway, subnet);
  WiFi.hostname(hostName);
	WiFi.begin ( ssid, password );
	Serial.println ( "" );
  MDNS.begin(hostName);
 
	// Wait for connection
	while ( WiFi.status() != WL_CONNECTED ) {
		delay ( 500 );
		Serial.print ( "." );
	}
 
  
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  httpUpdater.setup(&httpServer, update_path, update_username, update_password);
    httpServer.on("/", handleRoot);

  httpServer.on("/inline1", [](){
    status1 = !status1;
    digitalWrite(D1, status1); 
    Serial.print("Status 1: ");
    Serial.println(status1);
    httpServer.send(200, "text/plain", "this works as well");
  });

  httpServer.on("/inline2", [](){
    status2 = !status2;
    digitalWrite(D2, status2); 
    Serial.print("Status 2: ");
    Serial.println(status2);
    httpServer.send(200, "text/plain", "this works as well");
  });
  

  httpServer.onNotFound(handleNotFound);
  
  httpServer.begin();
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local%s in your browser and login with username '%s' and password '%s'\n", hostName, update_path, update_username, update_password);
}

void loop(void){
  httpServer.handleClient();
}