#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

const char* ssid = "MI A1";        // Your WiFi SSID
const char* password = "23456789"; // Your WiFi Password

unsigned long myChannelNumber = 2297390;  // Your ThingSpeak Channel Number
const char* myWriteAPIKey = "R3SCZAGU7B7FNPO4"; // Your ThingSpeak Write API Key

WiFiClient client;

void setup() {
  Serial.begin(9600);
  delay(10);
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  ThingSpeak.begin(client);
}

void loop() {
  float sensorData[3]; // Array to store 5 sensor values as floats

  // Read 5 sensor values from the serial port
  for (int i = 0; i < 3; i++) {
    while (!Serial.available()) {
      delay(1000); // Wait for data to be available on the serial port
    }
    String data = Serial.readStringUntil('\n'); // Read a line of data
    // Extract the value part and convert it to a float
    sensorData[i] = data.toFloat();
  }

  // Check if all 5 values are received
  if (!isnan(sensorData[0]) && !isnan(sensorData[1]) && !isnan(sensorData[2])) {
    // Send the sensor values to ThingSpeak fields 1 to 5
    ThingSpeak.setField(1, sensorData[0]);
    ThingSpeak.setField(2, sensorData[1]);
    ThingSpeak.setField(3, sensorData[2]);


    // Write the data to ThingSpeak
    int statusCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if (statusCode == 200) {
      Serial.println("Data sent to ThingSpeak successfully.");
      // Print the values sent to ThingSpeak
      Serial.print("Field 1: ");
      Serial.println(sensorData[0], 2); // Display 2 decimal places
      Serial.print("Field 2: ");
      Serial.println(sensorData[1], 2); // Display 2 decimal places
      Serial.print("Field 3: ");
      Serial.println(sensorData[2], 2); // Display 2 decimal places
     
    } else {
      Serial.println("Error sending data to ThingSpeak. Status code: " + String(statusCode));
    }
  } else {
    Serial.println("Error reading sensor data.");
  }

  // Delay for 1 minute before the next group of values (adjust as needed)
  delay(60000);
}
