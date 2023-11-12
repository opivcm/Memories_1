
int toggleSwitch = 3;
int switchval;

void setup() {
  // put your setup code here, to run once:
  pinMode(toggleSwitch, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  switchval = digitalRead(toggleSwitch);
  Serial.println(switchval);
  delay(1000);
}
