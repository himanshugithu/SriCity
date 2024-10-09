#ifndef CTOP_SEND_H
#define CTOP_SEND_H
        
  
#include <ArduinoHttpClient.h>    
#include <ArduinoJson.h>  
// int status = WL_IDLE_STATUS;

const char serverAddress[] = "ctop.iiit.ac.in";
int port = 443;
#define ledPin  A3

WiFiSSLClient wifiClient;
HttpClient httpClient(wifiClient, serverAddress, port);

void postData(float voltage, float current, float frequency, float power, float energy, float powerFactor) {
        if (WiFi.status() == WL_CONNECTED) {
        DynamicJsonDocument doc(1024);
        doc["Voltage"] = voltage;
        doc["Current"] = current;
        doc["Frequency"] = frequency;
        doc["Total Power"] = power;        
        doc["Energy"] =energy;   
        doc["Power Factor"] = powerFactor ;

        String requestBody;
        serializeJson(doc, requestBody);

        httpClient.beginRequest();
        httpClient.post("/api/cin/create/38");  
        httpClient.sendHeader("Content-Type", "application/json");
        httpClient.sendHeader("Authorization", "Bearer 9efc949df609851c908c7f7311871aaf"); 
        httpClient.sendHeader("Content-Length", requestBody.length());
        httpClient.beginBody();
        httpClient.print(requestBody);
        httpClient.endRequest();

        int statusCode = httpClient.responseStatusCode();
        String response = httpClient.responseBody();

        if (statusCode > 0) {
            Serial.print("Status code: ");
            Serial.println(statusCode);
            Serial.print("Response: ");
            Serial.println(response);
            digitalWrite(ledPin, HIGH);
            delay(500);                    
            digitalWrite(ledPin, LOW); 
            delay(200);   
        } else {
            Serial.print("Error on sending POST: ");
            Serial.println(statusCode);
        }
    } else {
        Serial.println("WiFi not connected!");
    }
}

#endif // CTOP_SEND_H
