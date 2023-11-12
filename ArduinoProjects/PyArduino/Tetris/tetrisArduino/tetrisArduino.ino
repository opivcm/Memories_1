
#define LeftBtn 7
#define RightBtn 4
#define DownBtn 5
#define ChangeBtn 6
#define StartStop 8

int switchval;
int DL = 0;
String pyCmd;

void setup() {
  pinMode(LeftBtn, INPUT_PULLUP);
  pinMode(RightBtn, INPUT_PULLUP);
  pinMode(DownBtn, INPUT_PULLUP);
  pinMode(ChangeBtn, INPUT_PULLUP);
  pinMode(StartStop, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  if(digitalRead(StartStop) == HIGH) {
    while((pyCmd = Serial.readStringUntil('\n')) != "Recieved" ) {
      if (digitalRead(LeftBtn) == LOW || digitalRead(RightBtn) == LOW || digitalRead(ChangeBtn) == LOW || digitalRead(DownBtn) == LOW) {
        Serial.println("Start");
      }
    }

    // Game Loop:
    while(1) {
      if(digitalRead(LeftBtn) == LOW) {
        Serial.println("Left");
        delay(DL);
      }
      if(digitalRead(RightBtn) == LOW) {
        Serial.println("Right");
        delay(DL);
      }
      if(digitalRead(DownBtn) == LOW) {
        Serial.println("Down");
        delay(DL);
      }
      if(digitalRead(ChangeBtn) == LOW) {
        Serial.println("Change");
        delay(DL);
      }
      if(digitalRead(StartStop) == LOW) {
        Serial.println("Stop");
        break;
      }
      if((pyCmd = Serial.readStringUntil('\n')) == "Exit Game Loop") {
        break;
      }
    }
  }
}
