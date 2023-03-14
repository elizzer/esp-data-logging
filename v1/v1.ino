
#include <Firebase_ESP_Client.h>
#include <ESP8266WiFi.h>
#include <FS.h>
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
#define COUNT 9
#define FILE "/access"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJsonArray AccessData;
String uid;
using namespace std;

File f;

class person{
public:
  String name;
  int id;
  bool access;
  String point;
  person(String n,String i,String a,String p){
    n.toLowerCase();
    name=n; 
    id=i.toInt();
    a.toLowerCase();
    if(a.equals("yes")){
      access=true;
    }
    else if(a.equals("no")){
      access=false;
    }
    p.toLowerCase();
    point=p;
  }
  void printIt(){
    Serial.println();
    Serial.print("[+]");
    Serial.print(name);
    Serial.print(" has ");
    Serial.print(access);
    Serial.print(" access at ");
    Serial.print(point);
    Serial.println();
  }

};

person *accessData;

bool haveAccess(int id,String access=AccessPoint){
  access.toLowerCase();
  for(int i=0;i<COUNT;i++){
    int TempId=accessData[i].id;
    if(TempId==id){
      if(accessData[i].point==access){
        accessData[i].printIt();
        return accessData[i].access;
      }
    }
  }
  return false;
}

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

void LoadData(){
  Serial.println("[+]Load data init...");

    FirebaseJsonData r;  
    AccessData.get(r,"documents/");  
    FirebaseJsonArray tempArray; 
    r.getArray(tempArray);  
    int l= tempArray.size();  
    l=COUNT;
    
  Serial.println("[+]Length of the data "+String(l));

  Serial.println(AccessData.raw());
  Serial.println(r.to<String>());
  Serial.println(tempArray.raw());
  
  Serial.println("[+]The available free heap is: "+String(ESP.getFreeHeap()));
  Serial.println("[+]Each object is of size: "+String(sizeof(person)));
  accessData=(person *) malloc(l*sizeof(person));
  Serial.println("[+]The available free heap is: "+String(ESP.getFreeHeap()));

  // Serial.println("[+]Address of the assigned array: "+String(*accessData));
  for(int i=0;i<l;i++){
    
    FirebaseJsonData name,id,point,access;

    String Namepath=String("documents/[")+i+String("]")+String("/fields/name/stringValue");
    String Idpath=String("documents/[")+i+String("]")+String("/fields/id/integerValue");
    String Pointpath=String("documents/[")+i+String("]")+String("/fields/point/stringValue");
    String Accesspath=String("documents/[")+i+String("]")+String("/fields/access/stringValue");

    AccessData.get(name,Namepath);
    AccessData.get(id,Idpath);
    AccessData.get(point,Pointpath);
    AccessData.get(access,Accesspath);

    person p1(name.to<String>(),id.to<String>(),access.to<String>(),point.to<String>());
 
    accessData[i]=p1;
    // p1.printIt();      
  }

  Serial.println("[+]load data finish...");

}

void FireBaseGetData(){
  Serial.println("[+]Pull data init...");
  String path="access/";
  if(Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", path.c_str())){
    Serial.println("[+]Pull data process...");
    // Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    f = SPIFFS.open(FILE,"w");
    if(!f){
      Serial.println("[*]Unable to open the file...");
    }else{
      Serial.println("[+]File open success...");
      f.print(fbdo.payload().c_str());
      f.close();
    }
    AccessData.setJsonArrayData(fbdo.payload().c_str()); 
  }
  else{
    Serial.println("[+]Pull data process into else block...");
    Serial.println(fbdo.errorReason());
     
    f = SPIFFS.open(FILE,"r");
    if(!f){
      Serial.println("[*]Unable to open the file...");
    }else{
      Serial.println("[+]File open success..."); 
      AccessData.setJsonArrayData(f.readString()); 
      f.close();
    }

  } 
  Serial.println("[+]Pull data end...");
}

void FileInit(){
    if(SPIFFS.begin()){
    Serial.println("[+] SPIFFFS init success..");
  }else{
    Serial.println("[*]SPIFFS init error");
  }

  if(SPIFFS.format()){
    Serial.println("[+]SPIFFS file format success...");
  }else{
    Serial.println("[*]SPIFFS file format fail...");
  }


}

void PrintData(){
  for(int i=0;i<COUNT;i++){
    accessData[i].printIt(); 
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  FileInit();
  networkInit();
  FireBaseSetup();
  // FireBasePushData(1,false);
  FireBaseGetData();

  LoadData();
  PrintData();
  Serial.println(haveAccess(23));
  Serial.println(haveAccess(100));
}

void loop() {
  // put your main code here, to run repeatedly:
   if (Firebase.isTokenExpired()){
    Firebase.refreshToken(&config);
    Serial.println("Refresh token");
  }
}
