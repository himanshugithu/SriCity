#ifndef CTOP_SEND_H
#define CTOP_SEND_H

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>


void postData(float voltage, float current, float frequency, float power, float energy, float powerFactor) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Update the URL to point to your API endpoint
        http.begin("https://ctop.iiit.ac.in/api/cin/create/38");  
        
        // Add the required headers
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "Bearer 9efc949df609851c908c7f7311871aaf");

        // Create a JSON document to store the data
        DynamicJsonDocument jsonDoc(1024);
        jsonDoc["Voltage"] = voltage;
        jsonDoc["Current"] = current;
        jsonDoc["Frequency"] = frequency;
        jsonDoc["Total Power"] = power;
        jsonDoc["Energy"] = energy;
        jsonDoc["Power Factor"] = powerFactor;

        // Serialize JSON to a string
        String requestBody;
        serializeJson(jsonDoc, requestBody);

        // Send the POST request
        int httpResponseCode = http.POST(requestBody);

        // Check the response from the server
        if (httpResponseCode == 200) {  // Success
            String response = http.getString();
            Serial.println(httpResponseCode);
            Serial.println(response);

            // Blink the LED to indicate success
            digitalWrite(LED_BUILTIN, HIGH); 
            delay(500);  // LED on for 500ms
            digitalWrite(LED_BUILTIN, LOW);  
        } else {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }

        http.end();  // End the HTTP connection
    } else {
        Serial.println("WiFi not connected. Cannot send data.");
    }
}

#endif // CTOP_SEND_H
