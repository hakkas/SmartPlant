#include <Wire.h>
#include <SeeedOLED.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h> // ethernet
#include <PubSubClient.h> // mqtt
#include "DHT.h"
#include <TimerOne.h>
#include "Arduino.h"
#include "SI114X.h"

byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0E, 0xA1, 0xC3 // 192.168.178.66
};
IPAddress ip(192, 168, 178, 66); // 192.168.178.46 Arduino met SmartPlant
IPAddress server(192, 168, 178, 131); // tweede raspbi

#define DHTPIN          A0     // what pin we're connected to
#define MoisturePin     A1
#define ButtonPin       2
#define DHTTYPE DHT11   // DHT 11 
#define EncoderPin1     3
#define EncoderPin2     4
#define WaterflowPin    5
#define RelayPin        6

#define RelayOn         HIGH
#define RelayOff        LOW

SI114X SI1145 = SI114X();
DHT dht(DHTPIN, DHTTYPE);
float DHTHumidity    = 0;
float DHTTemperature = 0;
float MoisHumidity   = 100;
float UVIndex        = 0;
char buffer[30];

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);


void callback(char* topic, byte* payload, unsigned int length) {
  turnWaterOn();
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  SeeedOled.clearDisplay();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient12")) {
      client.subscribe("MoistureUpdate", 1);

      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void sendValues (String message) {
  /*
  String msg = "";
  char cmsg[100];
  msg += message;
  msg += ("Moisture: ");
  msg += (MoisHumidity); 
  msg += (" Humidity: ");
  msg += (DHTHumidity);
  msg += (" Temp: ");
  msg += (DHTTemperature);
  msg += (" UVIndex: ");
  msg += (UVIndex); 
  msg += (". ");
  msg.toCharArray(cmsg, msg.length());
  client.publish("SmartPlantUpdate", cmsg);
  */

}


void waterPumpOn()
{ 
  digitalWrite(RelayPin,RelayOn);
  delay(3000);
 
}

void waterPumpOff()
{ 
  digitalWrite(RelayPin,RelayOff);
  
}



void turnWaterOn()
{
  SeeedOled.clearDisplay(); 
  waterPumpOn();

  //sendValues("Water is on.");

  SeeedOled.setTextXY(0,3);
  SeeedOled.putString("Watering");
  SeeedOled.setTextXY(2,0);
  SeeedOled.putString("FlowRate:");
  SeeedOled.setTextXY(4,0);
  SeeedOled.putString("Volume:");
  waterPumpOff();
}



void setup() 
{
  Serial.begin(9600); 

  /* Init OLED */
  Wire.begin();
  SeeedOled.init();  //initialze SEEED OLED display
  DDRB|=0x21;        
  PORTB |= 0x21;
  SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode

  /* Init DHT11 */
  dht.begin();
  Ethernet.begin(mac, ip);

  /* Init UV */
  while (!SI1145.Begin()) {
    delay(1000);
  }

  /* Init Water flow */
  pinMode(WaterflowPin,INPUT);

  /* Init Relay      */
  pinMode(RelayPin,OUTPUT);

  //PutWaterOn();
}

void readSensors() {
  DHTHumidity    = dht.readHumidity();
  DHTTemperature = dht.readTemperature();
  MoisHumidity   = analogRead(MoisturePin)/7;
  UVIndex        = (float)SI1145.ReadUV()/100 + 0.5;
}

void showValuesOnScreen() {
  sprintf(buffer,"UV Index");
  SeeedOled.setTextXY(0,4);          //Set the cursor to Xth Page, Yth Column  
  SeeedOled.putString(buffer);

  sprintf(buffer,"%d",(int)(UVIndex));
  SeeedOled.setTextXY(1,4);          //Set the cursor to Xth Page, Yth Column  
  SeeedOled.putString(buffer);

  sprintf(buffer,"Humidity");
  SeeedOled.setTextXY(2,4);          //Set the cursor to Xth Page, Yth Column  
  SeeedOled.putString(buffer);

  sprintf(buffer,"%d.%d%%",(int)(DHTHumidity),(int)((int)(DHTHumidity*100) % 100));
  SeeedOled.setTextXY(3,4);          //Set the cursor to Xth Page, Yth Column  
  SeeedOled.putString(buffer);

  sprintf(buffer,"Temperature");
  SeeedOled.setTextXY(4,4);          //Set the cursor to Xth Page, Yth Column  
  SeeedOled.putString(buffer);

  sprintf(buffer,"%d.%d*C",(int)(DHTTemperature),(int)((int)(DHTTemperature*100)%100));
  SeeedOled.setTextXY(5,4);          //Set the cursor to Xth Page, Yth Column  
  SeeedOled.putString(buffer);  
}

void loop() 
{
  if (!client.connected()) {
    Serial.println("Connecting....");
    reconnect();
  }
  
  readSensors();
  //sendValues("Update");
  showValuesOnScreen();
  //Serial.println("nu wordt loop aangeroepen");
  client.loop();
  delay(100);
}

