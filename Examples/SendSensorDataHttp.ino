/* Http message
AT+CIPSTART="TCP","10.0.0.2",8000
AT+CIPSEND=141
POST /arduinodata HTTP/1.1
Host: 10.0.0.2
User-Agent: Arduino/1.0
Connection: close
Content-Length: 26
humidity=13&temperature=25
AT+CIPCLOSE
*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| Sensor |~~~~~~~~~~~~~~~~~~~~~~*/
#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| Actuators | ~~~~~~~~~~~~~~~~~~*/
#define VENTILADOR 8
#define FOCO 9
#define VAPORIZADOR 10

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|Wifi Headers|~~~~~~~~~~~~~~~~~~~~*/
#include <SoftwareSerial.h>
SoftwareSerial esp8266(2,3);
#define BAUD 9600
const char* HOST = "10.0.0.2";
const char* PORT = "8000";
const char* TYPE_CONNECTION = "TCP";

String http_header[] = {
    "POST /arduinodata HTTP/1.1",
    "Host: 10.0.0.2",
    "User-Agent: Arduino/1.0",
    "Connection: close",
    "Content-Type: application/x-www-form-urlencoded; charset=UTF-8",
    "Content-Length: "
};
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~| Wifi Functions|~~~~~~~~~~~~~~~~~~~~~~~*/

String getHttpBody(float h, float t){
    //body humidity=13.5&temperature=25.4
    String http_body="humidity=" + String(h) + "&" + "temperature=" + t + "\n";
    //String http_body = "humidity=13";

    return http_body;
}

void sendHttpRequest(String http_header[],int size_http_header, String body) {
    String http_message;
    establishConnection(TYPE_CONNECTION, HOST, PORT);
    delay(20);
    for (int i = 0; i < size_http_header; i++) {
        http_message.concat(http_header[i]);
        http_message.concat("\r\n");
    }
    http_message.concat(http_header[5] + String(body.length()-2));
    http_message.concat("\r\n\r\n");
    http_message.concat(body);
    Serial.println(http_message);
    sendPackage(http_message);
    closeConnection();
}

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

void sendPackage(String package){
  esp8266.println("AT+CIPSEND=" + String(package.length()) + "\r\n");
  delay(20);
  esp8266.println(package);
  esp8266.println("\r\n");
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
        }
    }
    return response;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~|Actuators functions|~~~~~~~~~~~~~~~~~~~~~~~~~*/

void control(float h, float t){
  if(t<25){// Temperatura baja
    digitalWrite(VENTILADOR, LOW);// Apaga ventilador
    digitalWrite(FOCO, HIGH);// Prende foco
  }else{
    if(t>=25 && t<=30)
    {
      digitalWrite(VENTILADOR, LOW);// Apaga Ventilador
      digitalWrite(FOCO, LOW);// Apaga foco
    }
    else
    {
      if(t>30){// Temperatura alta{
        digitalWrite(VENTILADOR, HIGH);// Prende Ventilador
        digitalWrite(FOCO, LOW);// Apaga foco
      }
    }
  }
  if(h<60){// Humedad baja
    digitalWrite(VAPORIZADOR, HIGH);// Prende vaporizador
  }else{
    if(h>=60){
      digitalWrite(VAPORIZADOR, LOW);// Apaga vaporizador
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| Arduino |~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void setup()
{
    Serial.begin(BAUD);
    //Wifi
    esp8266.begin(BAUD);
    //Sensor
    dht.begin();
    //Actuators
    pinMode(VENTILADOR, OUTPUT);
    pinMode(FOCO, OUTPUT);
    pinMode(VAPORIZADOR, OUTPUT);
    //Test
    randomSeed(millis());
}

void loop()
{
    Serial.println("Waiting...");
    float h = random(0,60)/100.0;
    float t = random(-20,20)/100.0;
    //float h = dht.readHumidity();
    //float t = dht.readTemperature();
    //float f = dht.readTemperature(true);
    /*if(isnan(h)|| isnan(t)||isnan(f)){
      Serial.println("Error obteniendo los datos del sensor DHT11");
      return;
    }*/
    //control(h,t);
    String http_body = getHttpBody(h,t);
    sendHttpRequest(http_header,5,http_body);

    delay(3000);
}
