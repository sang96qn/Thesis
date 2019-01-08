/*
  Tran Sang DHBK 0964 500 940
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define ssid "TRANSANG"
#define password "0964500940"
#define server "192.168.43.99" // Raspberry Pi Zero - MQTT broker

#define Garden_Motor "Motor"
#define Garden_Copony "Copony"
#define Garden_Light "Light"
#define Garden_Temperature "Temperature"
#define Garden_Humidity "Humidity"

#include "DHT.h"
#define DHTPIN 2 
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

#include <SimpleTimer.h>
SimpleTimer timer;

const uint16_t port = 1883;// default
WiFiClient espClient;
PubSubClient client(espClient);

static bool autoMode = true;
static bool isFirst = true;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  payload[length] = '\0';
  char p = char(payload[0]);
  Serial.println(p);
    if(p=='1')
    {
       digitalWrite(0, HIGH);
       Serial.println(" Turn On Motor! " );
    }
    else if(p=='2')
    {
       digitalWrite(0, LOW);
       Serial.println(" Turn Off Motor! " );
    }

    if(p=='3')
    {
       digitalWrite(4, HIGH);
       Serial.println(" Turn On Copony! " );
    }
    else if(p=='4')
    {
       digitalWrite(4, LOW);
       Serial.println(" Turn Off Capony! " );
    }

    if(p=='5')
    {
       digitalWrite(5, HIGH);
       Serial.println(" Turn On Light! " );
    }
    else if(p=='6')
    {
       digitalWrite(5, LOW);
      Serial.println(" Turn Off Light! " );
    }
    if(p=='7')
    {
       autoMode = true;
       
       Serial.println(" Auto Mode " );
    }
    else if(p=='8')
    {
       autoMode = false;
       Serial.println(" Manual Mode" );
    }
    
//  else Serial.println(" Khong hop le");
    Serial.println();
  }
  
// a function to be executed periodically
void repeatMe() {
       // Reading temperature or humidity takes about 250 milliseconds!
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();

      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
      }
      
      Serial.print("Temperature in Celsius:");
      Serial.println(String(t).c_str());
      client.publish(Garden_Temperature, String(t).c_str(), true);

      Serial.print("Humidity:");
      Serial.println(String(h).c_str());
      client.publish(Garden_Humidity, String(h).c_str(), true);
}
void readAnhSang()
{
  int x = digitalRead(10);
  if(x == 0)
       {
         Serial.println("Troi Sang");
         if(isFirst == true)
         {
          client.publish(Garden_Light,"6",{retain: 2});//de tat
          isFirst = false;
          }
        } 
  else
    {
        Serial.println("Troi Toi");
        if(isFirst == false)
         {
          client.publish(Garden_Light,"5",{retain: 2});//de bat
          isFirst = true;
          }
        } 
  
        
}
 void readBtn()
 {
    if(digitalRead(13) == LOW)
  {
    while(digitalRead(13) == LOW);
    int statusLed = digitalRead(0);
    if(statusLed == HIGH)// dang sang
    {
      client.publish(Garden_Motor,"2",{retain: 2});//de tat
      }
      else {
         client.publish(Garden_Motor,"1",{retain: 2});// de bat
        }
    }
    if(digitalRead(12) == LOW)
    {
      while(digitalRead(12) == LOW);
      int statusLed = digitalRead(4);
      if(statusLed == HIGH)// dang sang
      {
        client.publish(Garden_Copony,"4",{retain: 2});//de tat
        }
        else {
           client.publish(Garden_Copony,"3",{retain: 2});// de bat
          }
      }
      if(digitalRead(14) == LOW)
      {
        while(digitalRead(14) == LOW);
        int statusLed = digitalRead(5);
        if(statusLed == HIGH)// dang sang
        {
          client.publish(Garden_Light,"6",{retain: 2});//de tat
          }
          else {
             client.publish(Garden_Light,"5",{retain: 2});// de bat
            }
        }
  }
void setup() {
  Serial.begin(115200);
  pinMode(13,INPUT_PULLUP);
  pinMode(12,INPUT_PULLUP);
  pinMode(14,INPUT_PULLUP);
  pinMode(10,INPUT);
  pinMode(5,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(0,OUTPUT);
  digitalWrite(5,LOW);
  digitalWrite(4,LOW);
  digitalWrite(0,LOW);
  setup_wifi();
  dht.begin();
  timer.setInterval(2000, repeatMe);
  client.setServer(server, port);
  client.setCallback(callback);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...

      client.publish("Auto","7");
    //  client.publish(topic_pub_motor,"",{retain: 1}); //gửi đè giá trị trống
      // ... and resubscribe
      client.subscribe(Garden_Motor);
      client.subscribe(Garden_Copony);
      client.subscribe(Garden_Light);
      client.subscribe("Auto");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  timer.run();
  if(autoMode == true)
  {
    readAnhSang();
    }
  if(autoMode==false) {readBtn();}
  client.loop();
}
