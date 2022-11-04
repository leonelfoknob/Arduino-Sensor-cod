#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif

#include "DHT.h"
#define DHTTYPE DHT11   

#define dht_dpin D3
DHT dht(dht_dpin, DHTTYPE);

// Replace with your network credentials
const char* ssid     = "FiberHGW_TP9130_2.4GHz";
const char* password = "xkzFjwXA";


const char* serverName = "http://www.datamakinafleo.000webhostapp.com/weater-send-data.php";

String apiKeyValue = "tPmAT5Ab3j7F6";


void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
 dht.begin();
  
}

void loop() {
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    
    http.begin(client, serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // verileri gönderiyor
    String httpRequestData = "api_key=" + apiKeyValue + "&value1=" + String(dht.readTemperature())
                           + "&value2=" + String(dht.readHumidity()) + "&value3=" + String(dht.readTemperature(true)) + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  delay(30000);  
}



//https://randomnerdtutorials.com/visualize-esp32-esp8266-sensor-readings-from-anywhere/
