#include <FS.h>

#define FILE "/accessData"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
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

  File f = SPIFFS.open(FILE,"w");
  if(!f){
    Serial.println("[*]Unable to open the file...");
  }else{
    Serial.println("[+]File open success...");
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
 