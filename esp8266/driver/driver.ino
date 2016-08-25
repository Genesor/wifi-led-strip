//Demo for LED Strip Driver controling single RGB LED strip by Catalex
//Hardware: A LED Strip Driver and 5 meters of RGB LED strip
//Board: Carduino or Arduino UNO R3
//IDE:   Arduino-1.0
//Function: Show 7 colors with a single RGB LED.
//Store: catalex.taobao.com
/***************************************************************/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "RGBdriver.h"
#define CLK 16//pins definitions for the driver
#define DIO 4
RGBdriver Driver(CLK,DIO);


const char* ssid = "XXXXXXXXX";
const char* password = "XXXXXX";

MDNSResponder mdns;

ESP8266WebServer server(80);


void handleRoot() {
 server.send(200, "text/html", "<!doctype html><html> <head> <meta charset=\"utf-8\"> <title>Main Bench LED strip</title> </head> <body>WIFI</body></html>");
}

void handleRGB(){
  unsigned long red   = 0;
  unsigned long blue  = 0;
  unsigned long green = 0;
  for (uint8_t i=0; i<server.args(); i++){
    if(server.argName(i) == "r")
    {
       red = server.arg(i).toInt();
    } else if(server.argName(i) == "b") {
      blue = server.arg(i).toInt();
    } else if(server.argName(i) == "g") {
      green = server.arg(i).toInt();
    }
  }

  Driver.begin();
  Driver.SetColor(red, green, blue); //Green
  Driver.end();

  handleRoot();
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/rgb", handleRGB);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
