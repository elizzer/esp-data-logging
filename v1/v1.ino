
#include <Firebase_ESP_Client.h>
#include <ESP8266WiFi.h>

#include <string.h>

#include "addons/RTDBHelper.h"
#include "addons/TokenHelper.h"


#define Wifi_SSID "Protosem"
#define Wifi_PWD "proto123"
#define AccessPoint "DOOR"
#define API_KEY "AIzaSyBerVodscP609gn0LyEDrIwwnnqhIzKQ7I"
#define FIREBASE_PROJECT_ID "esp-log-cb372"
#define USER_EMAIL "logger@forge.com"
#define USER_PASSWORD "logger@forge"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
String uid;
using namespace std;

void networkInit(){
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(Wifi_SSID,Wifi_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void FireBaseSetup(){
  Serial.println("[+]Firebase setup init...");
  config.api_key=API_KEY;

 

  auth.user.email=USER_EMAIL;
  auth.user.password=USER_PASSWORD;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback;
  config.max_token_generation_retry = 5;
  Firebase.begin(&config,&auth);  
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.print(uid);  
  Serial.println("[+]Firebase setup finish...");
}


void FireBasePushData(int id,bool status){
  Serial.println("[+]Firebase push data init...");
  if(WiFi.status()==WL_CONNECTED && Firebase.ready()){
    Serial.print("[+]Test print");
    String documentPath;
    sprintf(&documentPath[0],"logs/%d/%s",id,AccessPoint);
    Serial.println(documentPath);
  }
  Serial.println("[+]Firebase push data finish...");
}

void FireBaseGetData(){
  Serial.println("[+]Pull data init...");
  String path="access/";
  String ids;
  if(Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", path.c_str())){
    Serial.println("[+]Pull data process...");
    Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    // FirebaseJson json;
    // json.setJsonData(fbdo.payload());  
   
       
  }
  else{
    Serial.println(fbdo.errorReason());
  } 
  Serial.println("[+]Pull data end...");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  networkInit();
  FireBaseSetup();
  FireBasePushData(1,false);
  FireBaseGetData();
}

void loop() {
  // put your main code here, to run repeatedly:
   if (Firebase.isTokenExpired()){
    Firebase.refreshToken(&config);
    Serial.println("Refresh token");
  }
}
