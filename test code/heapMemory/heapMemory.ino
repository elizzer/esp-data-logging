void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Heap before variable size array: ");
  Serial.println(ESP.getFreeHeap()); 
}

void loop() {
  // put your main code here, to run repeatedly:

}
