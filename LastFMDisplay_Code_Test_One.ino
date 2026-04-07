//While my project has an ESP32C3, I did this practice to get familiar with the API of LastFM, pulling my most recent three tracks.

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_WIFI_GOES_HERE";
const char* password = "YOUR_PASSWORD_GOES_HERE";
const char* lastfm_username = "USERNAME_GOES_HERE";
const char* lastfm_api_key = "YOUR_API_KEY_GOES_HERE";

String lastfm_url = "GET_YOUR_URL";

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\nConnecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost");
    delay(10000);
    return;
  }

  HTTPClient http;
  // Last.fm supports HTTP; if using HTTPS, you'd need a root CA certificate or insecure mode
  http.begin(lastfm_url);
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    String payload = http.getString();
    
    // Using StaticJsonDocument or DynamicJsonDocument depending on your library version
    // For ArduinoJson 6+, 4096 bytes is usually enough for 3 tracks
    DynamicJsonDocument doc(4096); 
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("JSON parse failed: ");
      Serial.println(error.f_str());
    } else {
      // Last.fm returns an array of tracks inside ["recenttracks"]["track"]
      JsonArray tracks = doc["recenttracks"]["track"];
      
      Serial.println("\n--- Last 3 Tracks ---");
      for (int i = 0; i < tracks.size(); i++) {
        const char* song = tracks[i]["name"];
        const char* artist = tracks[i]["artist"]["#text"];
        const char* album = tracks[i]["album"]["#text"];

        Serial.print(i + 1);
        Serial.print(". ");
        Serial.print(song);
        Serial.print(" by ");
        Serial.print(artist);
        Serial.print(" [Album: ");
        Serial.print(album);
        Serial.println("]");
      }
    }
  } else {
    Serial.print("HTTP Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end(); // Free resources
  
  // Refresh every 30 seconds
  Serial.println("Waiting 30 seconds...");
  delay(30000);
}
