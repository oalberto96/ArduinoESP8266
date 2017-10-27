/*Http message
 * 
 * AT+CIPSTART="TCP", "10.0.0.2",8000
 * AT+CIPSEND=141
 * POST /arduinodata HTTP/1.1
 * Host: 10.0.0.2
 * User - Agent: Arduino/1.0
 * Connection: close
 * Content-Length: 26
 * humidity = 13&temperature=25
 * AT+CIPCLOSE
 */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| Modulo Wifi |~~~~~~~~~~~~~~~~~~~~~~*/

#include <SoftwareSerial.h>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| Sensor |~~~~~~~~~~~~~~~~~~~~~~*/

#include <DHT.h> //cargamos la libreria DHT
#define DHTPIN 5 //Seleccionamos el pin en el que se conectara el sensor
#define DHTTYPE DHT11 //Se selecciona el DHT11
DHT dht(DHTPIN, DHTTYPE); //Se inicia una variable que sera usada por Arduino para comunicarse con el sensor 

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| Display LCD |~~~~~~~~~~~~~~~~~~~~~~*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>//cargamos la libreria LiquidCrystal
LiquidCrystal_I2C lcd(0x3f,16,2);//Pines A4 y A5

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| Actuadores | ~~~~~~~~~~~~~~~~~~*/

#define VENTILADOR 8
#define FOCO 9
#define VAPORIZADOR 10

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|Wifi Headers|~~~~~~~~~~~~~~~~~~~~*/

SoftwareSerial esp8266(2,3);
#define BAUD 9600
const char* WIFI_SSID = "SpiderMan";
const char* PASSWORD = "jornadas2020";
const char* HOST = "192.168.43.37";
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

void establishConnection(String ssid, String password) {
    //AT+CWJAP="my-test-wifi","1234test"
    String command = String("AT+CWJAP=") + "\""+ ssid+"\"" +"," + "\"" + password + "\"";
    esp8266.println(command);
}



String getHttpBody(int h, int t){
    //body humidity=13.5&temperature=http_message25.4
    String http_body= "humidity=" + String() + h + "&" + "temperature=" + t + "\n";
    //String http_body = "humidity=13";

    return http_body;
}



void sendHttpRequest(String http_header[],int size_http_header, String body) {
    String http_message;
    establishTCPConnection(TYPE_CONNECTION, HOST, PORT);
    delay(20);
    for (int i = 0; i < size_http_header; i++) {
        http_message.concat(http_header[i]);
        http_message.concat("\r\n");
    }
    http_message.concat(http_header[5] + String(body.length()-1));
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

void establishTCPConnection(String type, String ip_address, String port) {
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| Arduino |~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD); //Se inicia la comunicacion serial
  //Wifi
  esp8266.begin(BAUD);
  establishConnection(WIFI_SSID, PASSWORD);
  delay(3000);
  dht.begin(); //Se inicia el sensor
  //LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  delay(1000);
  //Actuators
  pinMode(VENTILADOR, OUTPUT);// Ventilador
  pinMode(FOCO, OUTPUT);// Foco
  pinMode(VAPORIZADOR, OUTPUT);// Vaporizador
  //Test
  randomSeed(millis());
}

void loop() {
  // put your main code here, to run repeatedly:
  //Esperamos 2 segundos entre medidas
  delay(2000);
  Serial.println("Waiting...");
  int h = dht.readHumidity(); //Se lee la humedad
  int t = dht.readTemperature(); //Se lee temperatura en centigrados
  //Comprobamos si ha habido algun error en la lectura
  if(isnan(h)|| isnan(t)){
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }
  //imprimir en pantalla
  //Se imprimen las variables
  Serial.println("Humedad: ");
  Serial.println(h);
  Serial.println("Temperatura: ");
  Serial.println(t);
  Serial.println(" *C ");
  control(h,t);
  pantalla(h,t);
  String http_body = getHttpBody(27,15);
  sendHttpRequest(http_header,5,http_body);
  Serial.println(esp8266Listener());
  delay(3000);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| LCD functions |~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void pantalla(int h, int t){
  lcd.setCursor(0,0);
  lcd.print("Temperatura  ");
  lcd.print(t);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Humedad      ");
  lcd.print(h);
  lcd.print("%");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~| Actuators functions |~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void control(int h, int t){

  if(t<27){// Temperatura baja
    digitalWrite(VENTILADOR, LOW);// Apaga ventilador
    digitalWrite(FOCO, HIGH);// Prende foco
  }else{
    if(t>=27 && t<=28)
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
  //digitalWrite(VENTILADOR, HIGH);// Prende Ventilador
}

