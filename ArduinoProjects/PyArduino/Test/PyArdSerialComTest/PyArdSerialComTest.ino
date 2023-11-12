
#define LeftBtn 3
#define RightBtn 4
#define DownBtn 5
#define ChangeBtn 6
#define StartStop 7

int switchval;
int DL = 500;

void setup() {
  pinMode(LeftBtn, INPUT_PULLUP);
  pinMode(RightBtn, INPUT_PULLUP);
  pinMode(DownBtn, INPUT_PULLUP);
  pinMode(ChangeBtn, INPUT_PULLUP);
  pinMode(StartStop, INPUT_PULLUP);
  Serial.begin(600);
}

void loop() {
  Serial.println("Start");
  delay(DL);
  while(1){
    Serial.println("Right");
  }
}
