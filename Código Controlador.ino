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
//definição do host e da key para acesso ao banco de dados
#define FIREBASE_HOST "https://anti-furto-de4a9-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "9DVtXOpVu9LMrwUR02IJewsL5i9Hm9PEmGuwrpNi"
// Definição das credenciais de acesso ao wifi (Deve ser alterada caso seja desejado conectar em outra rede)
#define WIFI_SSID "Mono-net"
#define WIFI_PASSWORD "Bike_CE289"
//-----------------------------------------------------------------------------------

Servo myservo; // Definição do servo
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
SoftwareSerial neo6m(RXPin, TXPin); //pinos do GPS
TinyGPSPlus gps;
//-----------------------------------------------------------------------------------
bool furtada = false;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void setup() //setup do projeto (inicialização das portas seriais, da conexão com o wifi e com o Firebase)
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
void loop() { // loop do código
 
  smartdelay_gps(1000);
 
  if(!furtada){ // se a bicicleta não estiver furtada, voltar o servo para a posição de relaxamento do barbante
    myservo.write(0);
  }
  
  
  if(gps.location.isValid()) // quando as informações do gps forem válidas (satélites forem identificados)
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
    if(Firebase.setFloat(firebaseData, "/GPS/f_latitude", latitude)) //envia os dados de latitude
      {print_ok();}
    else
      {print_fail();}
    //-------------------------------------------------------------
    if(Firebase.setFloat(firebaseData, "/GPS/f_longitude", longitude)) //envia os dados de longitude
      {print_ok();}
    else
      {print_fail();}
     if(Firebase.setFloat(firebaseData, "/GPS/velocidade", velocidade)) // envia os dados de velocidade
      {print_ok();}
    else
      {print_fail();}
   //-------------------------------------------------------------
  }
  else
  {
    Serial.println("No valid GPS data found.");
  }
  if(Firebase.getInt(firebaseData, "/GPS/distanciaTotal")){ //Recebe os dados da distancia do Banco de Dados
   
  
      if(firebaseData.dataTypeEnum() == fb_esp_rtdb_data_type_integer){
        Serial.println(firebaseData.to<int>()); 

        DistanciaTotal = firebaseData.to<int>();
        if(Firebase.getFloat(firebaseData,"GPS/velocidade")){ // Recebe os dados de velocidade

        if(DistanciaTotal > 30 && firebaseData.to<float>() > 4){ // caso a distância for maior que 30 metros e a velocidade maior que 4km/h
          myservo.write(180); //fechar o servo
          furtada = true; // bike furtada!!!
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
void wifiConnect() // Conexão com o Wifi
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
