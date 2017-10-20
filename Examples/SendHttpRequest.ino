#include <SoftwareSerial.h>
SoftwareSerial esp8266(2,3);

#define BAUD 9600

const char* HOST = "10.0.0.2";
const char* PORT = "8000";
const char* TYPE_CONNECTION = "TCP";

String data = "number=5";

String http_header[] = {
    "POST /arduinodata HTTP/1.1",
    "Host: 10.0.0.2",
    "User-Agent: Arduino/1.0",
    "Connection: close",
    "Content-Length: 1600",
    ""
};

void establishConnection(String type, String ip_address, String port) {
    String command = String("AT+CIPSTART=") + "\""+ type+"\"" +"," + "\"" +ip_address + "\"" + "," + port;
    esp8266.println(command);
}

void sendHttpPost(String http_header[], String body) {

}

void sendPackage(String package){
  esp8266.println("AT+CIPSEND=" + String(package.length()));
  delay(20);
  esp8266.println(package);
}

void closeConnection(){
  esp8266.println("AT+CIPCLOSE");
}

String esp8266Listener(){
    String response;
    char c;
    if(esp8266.available()>0)
    {
        while(esp8266.available())
        {
            c = esp8266.read();
            response.concat(c);
            Serial.write(c);
        }
    }
    return response;
}

void setup()
{
  Serial.begin(BAUD);
  esp8266.begin(BAUD);
}

void loop()
{
    establishConnection(TYPE_CONNECTION, HOST, PORT);
    Serial.println("Waiting...");
    esp8266Listener();
    //Serial.write(esp8266Listener());
    delay(3000);
    //esp8266.println("AT+CIPSTATUS");
    //Serial.write(esp8266Listener());
    delay(3000);
    //esp8266Listener();

}



/* TCP
AT+CIPSTART="TCP","10.0.0.2",8000
AT+CIPSEND=5
AT+CIPCLOSE

*/
/* HTTP
client.println("POST /Api/AddParking/3 HTTP/1.1");
client.println("Host: 10.0.0.138");
client.println("User-Agent: Arduino/1.0");
client.println("Connection: close");
client.print("Content-Length: ");
client.println(PostData.length());
client.println();
client.println(PostData);*/
