#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <Servo.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
SoftwareSerial bluetooth(14, 2);
Servo servo;

const char* ssid = "ari hasugian";                   // wifi ssid
const char* password =  "penerangan";         // wifi password
const char* mqttServer = "soldier.cloudmqtt.com";    // IP adress Raspberry Pi
const int mqttPort = 15567;
const char* mqttUser = "mvqmbgfk";      // if you don't have MQTT Username, no need input
const char* mqttPassword = "9dfVvMvG4S_j";  // if you don't have MQTT Password, no need input        
const char* myTopik = "pintuCon"; 

//mosquitto_sub -h soldier.cloudmqtt.com -t pintuCon -u  mvqmbgfk -P qNKOm2UT3QiB -p 15567
//mosquitto_pub -h soldier.cloudmqtt.com -t pintuCon -u  mvqmbgfk -P qNKOm2UT3QiB -p 15567 -m on

WiFiClient espClient;
PubSubClient client(espClient);

long int data;
int magnetic = 4; 
int flagMagnetic = 1;
 
int state;
int buzzer = 0;

long int password1 = 0;
long int password2 = 1; 

int flagPintu=0;
int flagBuzzer=0;
String data1 = "tertutup",data2 = "terkunci",linkdata1 ,linkdata2, Link;

void setup() {
  Serial.begin(9600);
  Serial.println("================================"); 
  pinMode(buzzer,OUTPUT); 
   digitalWrite(buzzer,LOW); 
  pinMode(magnetic, INPUT_PULLUP);
  servo.attach(5); //D1 orange, red 3v, brown gnd
  servo.write(0); 

  bluetooth.begin(9600);
  
      tone(buzzer, 1000);  
  delay(2000);
       noTone(buzzer); 
  servo.write(90); 
  
  delay(2000);
  servo.write(0); 
  
  delay(2000);
  servo.write(90); 

  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }

//  client.publish("esp8266", "Hello Raspberry Pi");
//  client.subscribe("esp8266");

}


void uploadDatatoDB(){
  
    //Serial.println(data1);
    //Serial.println(data2);
    ///////////////////////////////////////////////////////////////////////
    HTTPClient http;    //Declare object of class HTTPClient
  
    //http://rumahari123.000webhostapp.com/esptodb.php?statusPintu=tertutup&statusKunci=tertutup
      
    linkdata1 = "statusPintu=" + data1;   
    linkdata2 ="&statusKunci=" + data2;
    Link = "http://rumahari.mamminasa.com/rumahAri.php?" +linkdata1 + linkdata2;
//    Link = "http://rumahari123.000webhostapp.com/esptodb.php?" + linkdata1 + linkdata2; 
//      Link = "http://rumahari123.000webhostapp.com/esptodb.php?statusPintu=terbuka&statusKunci=terkunci";
        Serial.println(Link);
    http.begin(Link);     //Specify request destination
    
    int httpCode = http.GET();            //Send the request
    String payload = http.getString();    //Get the response payload
  
    Serial.println(httpCode);   //Print 200, and -1 if error
    Serial.println(payload);    // //print OK or error
  
    //check if it works just fine
      if(payload.equals("OK")){ 
        Serial.println("berhasil");
      }else{ 
        Serial.println("gagal"); 
      }
      
    http.end();  //Close connection
  }


void callback(char* topic, byte* payload, unsigned int length) {
uint8_t textInput[5];  
    

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");  
  for (int i = 0; i < 5; i++) { //jadi kan ini max 5 huruf, biar gampang kalo ada input 4 huru
      Serial.print((char)payload[i]); //dibikin jadi 5 dimana huruf ke 5 itu "z"
      textInput[i]=(char)payload[i];
      if(i>=length){
        textInput[i]='Z';
        }
  }  
  Serial.println();
  String str((char*)textInput);  
     
  Serial.println(str);
   if(str=="openZ"){//note, harus 5 huruf karena arraynya di set 5
      // client.publish(sendTopik, "open the door");  
      flagMagnetic=0;
      servo.write(0); 
      data2 = "terbuka";
      uploadDatatoDB();
      Serial.print("terbuka");    
    }
  if(str=="close"){
      // client.publish(sendTopik, "close the door");
      servo.write(90); 
      flagMagnetic=1;
       data2 = "terkunci";
      uploadDatatoDB();
      Serial.print("terkunci");   
    }
  
  if(str=="qwert"){
      // client.publish(sendTopik, "publish data arduino");
        
      Serial.print("publish data arduino");  
    }
  Serial.println();
  Serial.println("-----------------------");
}

 

  
void loop() {  
//data from android
  if(bluetooth.available()>0){
    data = bluetooth.parseInt();
  
//Serial.print(data);
 
  if (data == password1){
    servo.write(0);
    Serial.println("bluetooth open");
     flagMagnetic=0;
     data2 = "terbuka";
     uploadDatatoDB();
   }else{
  servo.write(90);
  Serial.println("bluetooth  close");
      flagMagnetic=1;
      data2 = "tertutup";
      uploadDatatoDB();
   }
 
 }
  
  
//  read magnetic door
  state = digitalRead(magnetic); 

//  Serial.print("State =");
//  Serial.println(state);
//  Serial.print("-----------------------");

//   if(state == HIGH && flagMagnetic ==1 && flagBuzzer==0){
//      Serial.println("buzzer on");
//      tone(buzzer, 1000); 
//      delay(2000);
//      flagBuzzer==1;
//    }
     
       
      if(state == HIGH && flagPintu==0){ 
        flagPintu=1;  
        data1 = "terbuka";
        uploadDatatoDB();
//        Serial.println(data1);
        tone(buzzer, 1000); 
        delay(2000);

      }else if(state == LOW && flagPintu==1){ 
        data1 = "tertutup";
        uploadDatatoDB();
//        Serial.println(data1);
        flagPintu=0; 
      } 
    noTone(buzzer); 
    client.subscribe(myTopik); 
    client.loop();

    
}
