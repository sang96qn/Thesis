#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define ssid "TRANSANG"
#define password "0964500940"
#define server "192.168.43.99"

#define Garden_Motor "Motor"
#define Garden_Copony "Copony"
#define Garden_Light "Light"
#define Garden_Temperature "Temperature"
#define Garden_Humidity "Humidity"

#define MOTOR 0
#define FAN   4
#define LIGHT 5
#define MOTOR_BUTTON 13
#define FAN_BUTTON   12
#define LIGHT_BUTTON 14
#define QUANGTRO 10

#include "DHT.h"
#define DHTPIN 2 
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

#include <SimpleTimer.h>
SimpleTimer timer;

const uint16_t port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);

static bool autoMode = true;
static bool isFirst = true;
static bool isFirstDHT = true;

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
       digitalWrite(MOTOR, LOW);
       Serial.println(" Turn On Motor! " );
    }
    else if(p=='2')
    {
       digitalWrite(MOTOR, HIGH);
       Serial.println(" Turn Off Motor! " );
    }

    if(p=='3')
    {
       digitalWrite(FAN, LOW);
       Serial.println(" Turn On Fan! " );
    }
    else if(p=='4')
    {
       digitalWrite(FAN, HIGH);
       Serial.println(" Turn Off Fan! " );
    }

    if(p=='5')
    {
       digitalWrite(LIGHT, LOW);
       Serial.println(" Turn On Light! " );
    }
    else if(p=='6')
    {
       digitalWrite(LIGHT, HIGH);
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
    Serial.println();
  }
void docNhietDo() {
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

       if((t >= 27)||(h <=70))
       {
         Serial.println("Bom nuoc");
         if(isFirstDHT == true)
         {
          client.publish(Garden_Motor,"1",{retain: 2});
          client.publish(Garden_Copony,"3",{retain: 2});
          isFirstDHT = false;
          }
        } 
  else if ((t<27)||(h>70))
    {
        Serial.println("Tat motor");
        if(isFirstDHT == false)
         {
          client.publish(Garden_Motor,"2",{retain: 2});
          client.publish(Garden_Copony,"4",{retain: 2});
          isFirstDHT = true;
          }
        }
}
void docQuangTro()
{
  int statusQuangTro = digitalRead(QUANGTRO);
  if(statusQuangTro == 0)
       {
         Serial.println("Troi Sang");
         if(isFirst == true)
         {
          client.publish(Garden_Light,"6",{retain: 2});
          isFirst = false;
          }
        } 
  else
    {
        Serial.println("Troi Toi");
        if(isFirst == false)
         {
          client.publish(Garden_Light,"5",{retain: 2});
          isFirst = true;
          }
        }       
}
 void docNutNhan()
 {
    if(digitalRead(MOTOR_BUTTON) == LOW)
  {
    while(digitalRead(MOTOR_BUTTON) == LOW);
    int statusMOTOR = digitalRead(MOTOR);
    if(statusMOTOR == HIGH)
    {
      client.publish(Garden_Motor,"1",{retain: 2});
      }
      else {
         client.publish(Garden_Motor,"2",{retain: 2});
        }
    }
    if(digitalRead(FAN_BUTTON) == LOW)
    {
      while(digitalRead(FAN_BUTTON) == LOW);
      int statusFAN = digitalRead(FAN);
      if(statusFAN == HIGH)
      {
        client.publish(Garden_Copony,"3",{retain: 2});
        }
        else {
           client.publish(Garden_Copony,"4",{retain: 2});
          }
      }
      if(digitalRead(LIGHT_BUTTON) == LOW)
      {
        while(digitalRead(LIGHT_BUTTON) == LOW);
        int statusLIGHT = digitalRead(LIGHT);
        if(statusLIGHT == HIGH)
        {
          client.publish(Garden_Light,"5",{retain: 2});
          }
          else {
             client.publish(Garden_Light,"6",{retain: 2});
            }
        }
  }
void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_BUTTON,INPUT);
  pinMode(FAN_BUTTON,INPUT);
  pinMode(LIGHT_BUTTON,INPUT);
  pinMode(QUANGTRO,INPUT);
  pinMode(LIGHT,OUTPUT);
  pinMode(FAN,OUTPUT);
  pinMode(MOTOR,OUTPUT);
  digitalWrite(LIGHT,LOW);
  digitalWrite(FAN,LOW);
  digitalWrite(MOTOR,LOW);
  setup_wifi();
  dht.begin();
  timer.setInterval(2000, docNhietDo);
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
    docQuangTro();
    }
  if(autoMode==false) {docNutNhan();}
  client.loop();
}
