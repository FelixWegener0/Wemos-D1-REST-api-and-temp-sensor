#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SimpleDHT.h>

int pinDHT11 = 5;
SimpleDHT11 dht11(pinDHT11);

const char *ssid = "";
const char *password = "";

ESP8266WebServer server(80);

float getTemp()
{
    float temperature = 0;
    float humidity = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
    {
        Serial.print("Read DHT11 failed, err=");
        Serial.println(err);
        delay(2000);
    }
    Serial.print("check temp from sensor: ");
    Serial.println(temperature);
    return temperature;
}

float getHumidity()
{
    float temperature = 0;
    float humidity = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
    {
        Serial.print("Read DHT11 failed, err=");
        Serial.println(err);
        delay(2000);
    }
    Serial.print("check humid from sernsor: ");
    Serial.println(humidity);
    return humidity;
}

void getTempForServer()
{
    String message = "";
    message = message + getTemp();
    server.send(200, "text/plain", message);
}

void getHumidForServer()
{
    String message = "";
    message = message + getHumidity();
    server.send(200, "text/plain", message);
}

// Serving Hello world
void getHelloWord()
{
    server.send(200, "text/json", "{\"name\": \"Hello world\"}");
}

// Define routing
void restServerRouting()
{
    server.on(F("/temp"), HTTP_GET, getTempForServer);
    server.on(F("/humid"), HTTP_GET, getHumidForServer);

    server.on("/", HTTP_GET, []()
              { server.send(200, F("text/html"),
                            F("Welcome to the REST Web Server")); });

    server.on(F("/helloWorld"), HTTP_GET, getHelloWord);
}

// Manage not found URL
void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void setup(void)
{
    // Serial connection for debugging
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Activate mDNS this is used to be able to connect to the server
    // with local DNS hostmane esp8266.local
    if (MDNS.begin("esp8266"))
    {
        Serial.println("MDNS responder started");
    }

    // Set server routing
    restServerRouting();
    // Set not found response
    server.onNotFound(handleNotFound);
    // Start server
    server.enableCORS(true);
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void)
{
    server.handleClient();
}