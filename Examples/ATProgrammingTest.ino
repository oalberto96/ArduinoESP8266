#include <SoftwareSerial.h>
SoftwareSerial esp8266(2,3);

#define BAUD 9600

void setup()
{
  Serial.begin(BAUD);
  esp8266.begin(BAUD);
}

void loop()
{
    esp8266.println("AT");
    Serial.println("Waiting...");
    if(esp8266.available()>0)
    {
        while(esp8266.available())
        {
            char c = esp8266.read();
            Serial.write(c);
        }
    }
    delay(3000);
}
