#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>

const char* ssid = "XXXXXXX";
const char* password = "XXXXXXX";

MDNSResponder mdns;

ESP8266WebServer server(80);

IRsend irsend(0);

void handleRoot() {
 server.send(200, "text/html", "<!doctype html><html> <head> <meta charset=\"utf-8\"> <title>Main Bench LED strip</title> </head> <body> <div id=\"siteloader\"></div>​ <script src=\"http://code.jquery.com/jquery-1.7.2.min.js\"></script> <script>$(\"#siteloader\").load(\"http://iot.sadoma.so/\"); </script></body></html>");
}

void handleIr(){
  int sendDelay = 50;
  int maxDelay = 1000;
  for (uint8_t i=0; i<server.args(); i++){
    if(server.argName(i) == "delay")
    {
      sendDelay = server.arg(i).toInt();
      if (sendDelay > maxDelay) {
        sendDelay = maxDelay;
      }
    }
    if(server.argName(i) == "code")
    {
      unsigned long code = server.arg(i).toInt();
      irsend.sendNEC(code, 32);

    } else if(server.argName(i) == "code[]") {
      unsigned long code = server.arg(i).toInt();
      irsend.sendNEC(code, 32);
      delay(sendDelay);
    }
  }
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

void setup(void){
  irsend.begin();

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
  server.on("/ir", handleIr);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
