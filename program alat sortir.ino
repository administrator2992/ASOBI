#define S0 19
#define S1 18
#define S2 5
#define S3 4
#define sensorOut 15

#include <ESP32Servo.h>

#include <WiFi.h>
#include <PubSubClient.h>
const char* ssid = "mikdar";
const char* password = "Sakinah100%";

// Local MQTT broker
char *mqttServer = "iot.tf.itb.ac.id";
int mqttPort = 1884;
const char* mqtt_username = "TF4061";
const char* mqtt_password = "TF4061_iiot!";
const char* mqtt_client_name = "ESP32ffr";
const char* pub_m = "ESP32/kondisi/m";
const char* pub_bm = "ESP32/kondisi/bm";
const char* pub_tm = "ESP32/kondisi/tm";
const char* mqtt_sub_topic = "coba_out";

WiFiClient client;
PubSubClient mqttClient(client);
 
Servo myservo;  // create servo object to control a servo
Servo myservo2;  // create servo object to control a servo
 
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 23;
int servoPin2 = 21;

byte frequency = 0;
byte a=0;
byte b=0;
byte c=0;
/*q   
 * Nilai Pembacaan Warna
 */
//Matang
byte Ra=229; //R
byte Rb=304; //G
byte Rc=374; //B
//belum matang
byte Ga=331; //R
byte Gb=394; //G
byte Gc=346; //B
//busuk
byte Ba=450; //R
byte Bb=522; //G
byte Bc=459; //B
byte i=50; //Toleransi Nilai Pembacaan

char* msg = "";
int stp;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received from: ");
  Serial.println(topic);
  msg = (char*)payload;
  for (int i = 0; i < length; i++) {
    stp = msg[i];
    Serial.print(stp);
    if(stp == 48){
    Serial.println("servo mati");
    myservo.detach();
    myservo2.write(0);
    delay(500);
    myservo2.detach();
    } else{
      Serial.println("servo nyala");
      myservo.attach(servoPin);
      myservo2.attach(servoPin2);
      }
  }
  Serial.println();
  Serial.println();
}

void reconnect() {
    while( !mqttClient.connected() ){
      if( mqttClient.connect(mqtt_client_name, mqtt_username, mqtt_password) ){
        Serial.println("MQTT Connected!");
        mqttClient.subscribe(mqtt_sub_topic);
        mqttClient.setCallback(callback);
      }
      else{
        Serial.print(".");
        delay(5000);
      }
    }
  }

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
   }
  Serial.println("");
  Serial.print("WiFi connected to: ");
  Serial.println(ssid);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(2000);
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  // Setting frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  myservo.setPeriodHertz(50); 
  myservo.attach(servoPin);
  myservo2.setPeriodHertz(50); 
  myservo2.attach(servoPin2);
}
void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  // Setting red filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  // Printing the value on the serial monitor
  Serial.print("R= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.print("  ");
  a=frequency;
  delay(100);
  // Setting Green filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  // Printing the value on the serial monitor
  Serial.print("G= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.print("  ");
  b=frequency;
  delay(100);
  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  // Printing the value on the serial monitor
  Serial.print("B= ");//printing name
  Serial.print(frequency);//printing RED color frequency
  Serial.print("  ");
  c=frequency;
  delay(100);
 if(stp == 48){
  Serial.println(".");
  }
  else if((a>(Ra-i) && a<(Ra+i)) & (b>(Rb-i) && b<(Rb+i)) & (c>(Rc-i) && c<(Rc+i)))
  {
    Serial.println("Matang");
    mqttClient.publish(pub_m,"matang");
    myservo.write(45);
    mqttClient.loop();
    delay(5000);
    myservo2.write(90);
    delay(3000);
    myservo2.write(0);
    }
  else if ((a>(Ga-i) && a<(Ga+i)) & (b>(Gb-i) && b<(Gb+i)) & (c>(Gc-i) && c<(Gc+i)))
  {
    Serial.println("belum matang");
    mqttClient.publish(pub_bm,"belum matang");
    myservo.write(90);
    mqttClient.loop();
    delay(5000);
    myservo2.write(90);
    delay(3000);
    myservo2.write(0);
    }
  else if ((a>(Ba-i) && a<(Ba+i)) & (b>(Bb-i) && b<(Bb+i)) & (c>(Bc-i) && c<(Bc+i)))
  {
    Serial.println("Busuk");
    mqttClient.publish(pub_tm,"terlalu matang");
    myservo.write(130);
    mqttClient.loop();
    delay(5000);
    myservo2.write(90);
    delay(3000);
    myservo2.write(0);
    }
  else
  {
    Serial.println(" ");
    }
  delay(50);
}
