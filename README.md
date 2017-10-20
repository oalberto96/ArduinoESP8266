# ArduinoESP8266
Manual para programar arduino con el modulo wifi ESP8266

## Probar comandos AT

### Esquema
Para comprobar el funcionamiento del modulo, tendremos la siguiente configuracion:
[Configuracion 1](/docs/resources/config1.png)

Arduino | ESP8266
--------|---------
GND | GND
TX | TX
3.3v | CH_PD
3.3v | VCC
RX | RX
Nada | GPI00
Nada | GPI02
Nada | RST



### IDE
Para la configuracion de Arduino IDE
En *Tools* > *Board* seleccionamos Arduino/Genuino UNO, y en port seleccionamos /dev/ttyACMO, al tener lista la configuracion, copiamos
el codigo [Prueba Comandos AT!](/Examples/ATTest.ino)
Abrir *serial monitor* (CTRL+SHIFT+M) y seleccionar *Both NL & CR*, posteriormente se ingresara en la barra *AT*
Se debe recibir el siguiente mensaje
"OK"
Significa que el modulo esta funcionando correctamente
En caso de recibir basura, debera ir variando los baud

#### Comandos AT

Comando | Funcion | Respuesta | Ejemplo
--------|---------|-----------|---------
AT+CIOBAUD=NUmERO | Declarar los baud con los que trabajara el modulo | OK |AT+CIOBAUD=9600

## Programacion con modulo WIFI
Para realizar la Programacion de Comandos AT, se necesita otra configuracion, tanto fisica como logica.

### Esquema
Para poder programar los comandos AT en arduino, es necesario tener la sigueinte configuracion:
[Configuracion 2](/docs/resources/config2.png)

Arduino | ESP8266
--------|---------
GND | GND
2 | TX
3.3v | CH_PD
3.3v | VCC
3 | RX
Nada | GPI00
Nada | GPI02
Nada | RST

### IDE
La configuracion del IDE es la misma que la configuracion anterior.
El programa que se utilizara para la prueba es el siguiente [Programacion comando AT!](github.com)
Este programa envia un el comando "AT" al modulo, y este recibe y envia una respuesta "Ok", la cual se imprime en SerialMonitor
