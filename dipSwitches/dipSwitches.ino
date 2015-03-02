

//int dipSwitchArray[1, 2, 3, 4, 5 ,6];
int dipSwitch = 13;
int dipSwitchVal;

void setup() {
  /*
  for (int i = 0; i < 6; i++) {
    pinMode(dipSwitchArray[i], INPUT)
    digitalWrite(dipSwitchArray[i], HIGH)
  }*/
  pinMode(dipSwitch, INPUT);
  digitalWrite(dipSwitch, HIGH);
  Serial.begin(9600);

  
}
void loop() {
  dipSwitchVal = digitalRead(dipSwitch);
  Serial.print(dipSwitchVal);
}
