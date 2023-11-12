
int count = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(count);
  Serial.println(" Apples");
  count++;
  delay(1000);
}
