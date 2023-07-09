/*
   Note: The latest JSON library might not work with the code. 
   So you may need to downgrade the library to version v5.13.5
   
   Created by TAUSEEF AHMED
   
   YouTube: https://www.youtube.com/channel/UCOXYfOHgu-C-UfGyDcu5sYw/
   Github: https://github.com/ahmadlogs/
   
*/

//-----------------------------------------------------------------------------------
//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include <FirebaseESP8266.h>  //https://github.com/mobizt/Firebase-ESP8266
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h> //https://github.com/mikalhart/TinyGPSPlus
#include <Servo.h>
//Install ArduinoJson Library
//Note: The latest JSON library might not work with the code. 
//So you may need to downgrade the library to version v5.13.5
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
#define FIREBASE_HOST "https://anti-furto-de4a9-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "9DVtXOpVu9LMrwUR02IJewsL5i9Hm9PEmGuwrpNi"
#define WIFI_SSID "Mono-net"
#define WIFI_PASSWORD "Bike_CE289"
//-----------------------------------------------------------------------------------
//#define WIFI_SSID "Goiabeira-2G"
//#define WIFI_PASSWORD "monstrodosabao"
Servo myservo;
int DistanciaTotal;
//-----------------------------------------------------------------------------------
//Define FirebaseESP8266 data object
FirebaseData firebaseData;

FirebaseJson json;
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//GPS Module RX pin to NodeMCU D1
//GPS Module TX pin to NodeMCU D2
const int RXPin = 4, TXPin = 5;
SoftwareSerial neo6m(RXPin, TXPin);
TinyGPSPlus gps;
//-----------------------------------------------------------------------------------
int distancia;
int rotacao = 100;
bool furtada = false;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void setup()
{
  myservo.attach(D4);
  Serial.begin(9600);

  neo6m.begin(9600);
  
  wifiConnect();

  Serial.println("Connecting Firebase.....");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Serial.println("Firebase OK.");

}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void loop() {
  distancia = random(100);
  smartdelay_gps(1000);
  rotacao = -rotacao;
  if(!furtada){
    myservo.write(0);
  }
  
  
  if(gps.location.isValid()) 
  {
    float latitude = gps.location.lat();
    float longitude = gps.location.lng();
    float velocidade = gps.speed.kmph();
    //-------------------------------------------------------------
    //Send to Serial Monitor for Debugging
    Serial.print("LAT:  ");
    Serial.println(latitude);  // float to x decimal places
    Serial.print("LONG: ");
    Serial.println(longitude);
    //-------------------------------------------------------------
    
    //-------------------------------------------------------------
    if(Firebase.setFloat(firebaseData, "/GPS/f_latitude", latitude))
      {print_ok();}
    else
      {print_fail();}
    //-------------------------------------------------------------
    if(Firebase.setFloat(firebaseData, "/GPS/f_longitude", longitude))
      {print_ok();}
    else
      {print_fail();}
     if(Firebase.setFloat(firebaseData, "/GPS/velocidade", velocidade))
      {print_ok();}
    else
      {print_fail();}
   //-------------------------------------------------------------
  }
  else
  {
    Serial.println("No valid GPS data found.");
  }
  if(Firebase.getInt(firebaseData, "/GPS/distanciaTotal")){

  
      if(firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer){
        Serial.println(firebaseData.to<int>());

        DistanciaTotal = firebaseData.to<int>();
        if(Firebase.getFloat(firebaseData,"GPS/velocidade")){

        if(DistanciaTotal > 20 && firebaseData.to<float>() > 3){
          myservo.write(180);
          furtada = true;
        }
        else{
          furtada = false;
        }
        }
      }
      else{
        Serial.println(firebaseData.errorReason());
      }
      }
    else
      {print_fail();}

    

  delay(1000);
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
static void smartdelay_gps(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (neo6m.available())
      gps.encode(neo6m.read());
  } while (millis() - start < ms);
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void print_ok()
{
    Serial.println("------------------------------------");
    Serial.println("OK");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void print_fail()
{
    Serial.println("------------------------------------");
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

void firebaseReconnect()
{
  Serial.println("Trying to reconnect");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}