#include <FastLED.h>

#define WIDTH 23
#define HEIGHT 17
#define NUM_LEDS WIDTH * HEIGHT

#define DATA_PIN 2
#define TOP 3
#define RIGHT 4
#define BOTTOM 5
#define LEFT 6

CRGB leds[NUM_LEDS];

bool mapLayout[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,\
                    1,0,1,0,1,0,1,0,0,1,0,0,1,0,0,0,1,0,1,0,0,0,1,\
                    1,0,1,0,1,0,1,0,0,1,0,0,1,0,1,1,1,0,1,0,1,1,1,\
                    1,0,0,0,1,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1,\
                    1,1,1,0,0,0,0,0,0,1,0,0,0,0,1,1,0,1,1,1,1,1,1,\
                    1,0,0,0,1,0,1,1,1,1,1,1,1,0,0,1,0,1,0,0,0,0,1,\
                    1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,\
                    1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,0,0,0,0,0,0,1,\
                    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,\
                    1,0,0,0,0,0,0,0,1,0,1,1,1,0,1,1,1,0,1,0,1,0,1,\
                    1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,\
                    1,0,0,0,0,1,0,1,0,0,1,1,1,1,1,1,1,0,1,0,0,0,1,\
                    1,1,1,1,1,1,0,1,1,0,0,0,0,1,0,0,0,0,0,0,1,1,1,\
                    1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,1,0,0,0,1,\
                    1,1,1,0,1,0,1,1,1,0,1,0,0,1,0,0,1,0,1,0,1,0,1,\
                    1,0,0,0,1,0,1,0,0,0,1,0,0,1,0,0,1,0,1,0,1,0,1,\
                    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

int coinPos[] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};  
int charPos = 195;
int ghostPos[] = {24,44,346,366};   
uint8_t score = 0;                 
void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600);
  pinMode(TOP, OUTPUT);
  pinMode(RIGHT, OUTPUT);
  pinMode(BOTTOM, OUTPUT);
  pinMode(LEFT, OUTPUT);
}

void loop() {
  startFrame();
//  while(digitalRead(TOP)== false and digitalRead(BOTTOM)== false and digitalRead(LEFT)== false and digitalRead(RIGHT)== false){
//    delay(100);
//  }
//  clearScreen();
//  delay(1000);
//  gameLogic();
//  gameOver();
////  while(digitalRead(TOP)== false and digitalRead(BOTTOM)== false and digitalRead(LEFT)== false and digitalRead(RIGHT)== false){
////    delay(100);
////  }
//  clearScreen();
//  delay(1000);
delay(2000);
}

void startFrame(){
  bool startText[] = {0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,0,1,1,1,0,0,\
                      0,1,0,0,0,0,1,0,0,1,0,1,0,1,0,0,1,0,0,1,0,0,0,\
                      0,1,1,1,0,0,1,0,0,1,1,1,0,1,1,1,1,0,0,1,0,0,0,\
                      0,0,0,1,0,0,1,0,0,1,0,1,0,1,0,1,0,0,0,1,0,0,0,\
                      0,1,1,1,0,0,1,0,0,1,0,1,0,1,0,0,1,0,0,1,0,0,0};
  for(int i=0;i<69;i++){
    leds[i]= CRGB::Black;
  }
//  for(short int i=69;i<138;i++){
//    leds[i]= CRGB::Yellow;
//  }
//  for(short int i=0;i<115;i++){
//    if(startText[i]){
//      leds[138+i] = CRGB::Red; 
//    }
//    else{
//      leds[138+i] = CRGB::Yellow; 
//    }
//  }
//  for(int i=253;i<322;i++){
//    leds[i]= CRGB::Yellow;
//  }
//  for(int i=322;i<391;i++){
//    leds[i]= CRGB::Black;
//  }
  
  FastLED.show();
}

void gameOver(){
  bool gameText[]={0,0,1,1,1,1,0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,0,0,\
                   0,0,1,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,\
                   0,0,1,0,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,0,0,0,0,\
                   0,0,1,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,\
                   0,0,1,1,1,1,0,1,0,1,0,1,0,0,0,1,0,1,1,1,0,0,0};
  bool overText[]={0,0,1,1,1,0,1,0,0,0,1,0,1,1,1,0,1,1,1,1,0,0,0,\
                   0,0,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,1,0,0,0,\
                   0,0,1,0,1,0,0,1,0,1,0,0,1,1,0,0,1,1,1,1,0,0,0,\
                   0,0,1,0,1,0,0,1,0,1,0,0,1,0,0,0,1,0,1,0,0,0,0,\
                   0,0,1,1,1,0,0,0,1,0,0,0,1,1,1,0,1,0,0,1,0,0,0};
  for(short int i=0;i<46;i++){
    leds[i]= CRGB::Blue;
  }
  for(short int i=46;i<161;i++){
    if(gameText[i-46]){
      leds[i]=CRGB::Red;
    }
    else{
      leds[i]=CRGB::Blue;
    }
  }
  for(short int i=161;i<230;i++){
    leds[i]= CRGB::Blue;
  }
  for(int i=230;i<345;i++){
    if(overText[i-230]){
      leds[i]=CRGB::Red;
    }
    else{
      leds[i]=CRGB::Blue;
    }
  }
  for(int i = 345;i<391;i++){
    leds[i]= CRGB::Blue;
  }
  FastLED.show();
}

void clearScreen(){
  for(int i=0;i<NUM_LEDS;i++){
    leds[i]=CRGB::Black;
  }
  FastLED.show();
}

void gameLogic(){
  loadMap();
  spawnChars();
  dropCoins();
  playGame();
}

void loadMap(){
  for (int i=0;i<NUM_LEDS;i++){
    if(mapLayout[i]){
      leds[i]=CRGB::Brown;
    }
    else{
      leds[i]=CRGB::Black;
    }
  }
  FastLED.show();
}

void spawnChars(){::
  leds[charPos]=CRGB::White;
  for(uint8_t i=0;i<4;i++){
    leds[ghostPos[i]] = CRGB::Pink;
  }
  FastLED.show();
}

void dropCoins(){
  for(uint8_t i=0;i<10;i++){
    long pos = random(0,NUM_LEDS);
    while(mapLayout[pos]){
      pos = random(0,NUM_LEDS);
    }
    coinPos[i] = (int)pos;
    leds[pos] = CRGB::Yellow;
  }
  FastLED.show();
}

void playGame(){
  bool playState = true;
  unsigned long then = millis();
  unsigned long now;
  bool t;
  while(playState and score <10){
    if(digitalRead(TOP)){
      t = moveChar(1);
    }
    if(digitalRead(BOTTOM)){
      t = moveChar(2);
    }
    if(digitalRead(LEFT)){
      t = moveChar(3);
    }
    if(digitalRead(RIGHT)){
      t = moveChar(4);
    }
    now = millis();
    if((now - then)>700){
      delay(100);
      then = millis();
    }
    playState = t;
  }
}

bool moveChar(uint8_t dir){
  if(dir==1 and charPos>=23 and mapLayout[charPos-23]==false){
    leds[charPos] = CRGB::Black;
    charPos-=23;
    leds[charPos] = CRGB::White;
  }
  else if(dir==2 and charPos<=366 and mapLayout[charPos+23]==false){
    leds[charPos] = CRGB::Black;
    charPos+=23;
    leds[charPos] = CRGB::White;
  }
  else if(dir==3 and mapLayout[charPos-1]==false){
    leds[charPos] = CRGB::Black;
    charPos--;
    leds[charPos] = CRGB::White;
  }
  else if(dir==4 and mapLayout[charPos+1]==false){
    leds[charPos] = CRGB::Black;
    charPos++;
    leds[charPos] = CRGB::White;
  }
  for(uint8_t i=0;i<10;i++){
    if(charPos == coinPos){
      score++;
      coinPos[i]=-1;
    }
  }
  FastLED.show();
  if(score>9){
    return false;
  }
  return true;
}
