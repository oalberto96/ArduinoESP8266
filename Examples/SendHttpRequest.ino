/* Http message
AT+CIPSTART="TCP","10.0.0.2",8000
AT+CIPSEND=120
POST /arduinodata HTTP/1.1
Host: 10.0.0.2
User-Agent: Arduino/1.0"
Connection: close"
Content-Length: 1600"
number=5
AT+CIPCLOSE
*/

#include <SoftwareSerial.h>
SoftwareSerial esp8266(2,3);

#define BAUD 9600

const char* HOST = "10.0.0.2";
const char* PORT = "8000";
const char* TYPE_CONNECTION = "TCP";

String http_body = "number=5";

String http_header[] = {
    "POST /arduinodata HTTP/1.1\n",
    "Host: 10.0.0.2\n",
    "User-Agent: Arduino/1.0\n",
    "Connection: close\n",
    "Content-Length: 1600\n",
    ""
};

bool findText(String text,String text_to_find) {
    bool exist = false;
    int index = text.indexOf(text_to_find);
    if(index>=0){
        exist = true;
    }
    return exist;
}

bool statusConnection(){
    //3: Connected
    //4: Disconnected
    String response;
    String status = "AT";
    esp8266.println("AT+CIPSTATUS");
    response = esp8266Listener();
    return findText(response,status);

}

void establishConnection(String type, String ip_address, String port) {
    String command = String("AT+CIPSTART=") + "\""+ type+"\"" +"," + "\"" +ip_address + "\"" + "," + port;
    esp8266.println(command);
}

void sendHttpRequest(String http_header[],int size_http_header, String body) {
    establishConnection(TYPE_CONNECTION, HOST, PORT);
    delay(20);
    for (int i = 0; i < size_http_header; i++) {
        sendPackage(http_header[i]);
    }
    sendPackage(http_body);
    closeConnection();
}

void sendPackage(String package){
  esp8266.println("AT+CIPSEND=" + String(package.length()));
  delay(20);
  esp8266.println(package);
  esp8266.println("\n");
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
            //Serial.write(c);
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
    
    Serial.println("Waiting...");
    sendHttpRequest(http_header,5,http_body);
    /*if(statusConnection()){
        Serial.println("Conectado");
    }
    else {
        Serial.println("No Conectado");
        establishConnection(TYPE_CONNECTION, HOST, PORT);
    }*/
    //Serial.write(esp8266Listener());
    delay(3000);
}
