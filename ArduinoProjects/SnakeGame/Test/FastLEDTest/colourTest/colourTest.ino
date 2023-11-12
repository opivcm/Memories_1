/**
 * Simple snake game for a matrix of ws2812b leds strung up in serpentine pattern. The idea is
 * there is an apple and a snake on the board, and the snake tries to eat the apple. Each frame of the
 * game the snake moves in a single direction. If the snake is out of bounds or lands on its own tail,
 * then the game is over. If the head of the snake lands on the apple, the snake grows in length and the
 * speed of the snake increases.
 */
//LEDs
#include <FastLED.h>
#include <LinkedList.h>
#define NUM_LEDS    391
#define LED_PIN     12
int rows = 23;
int columns = 17;
CRGB leds[NUM_LEDS];

const int btn[] = {4, 5, 6, 7};

//Joystick
#define PAUSE_PIN 9
const int xPin        = A6;
const int yPin        = A7;

// Directions
#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_LEFT  2
#define DIR_RIGHT 3

int resetDelay = 250;

// Game State
#define MAX_SPEED 5
#define MIN_SPEED 300
#define SPEED_LOSS 15
int gameSpeed = MIN_SPEED;
int currDirection = DIR_UP;
boolean isGamePaused = false;
boolean isTogglingPause = false;

// class that represents a point on the matrix. Indexed starting at 0
class Point {
  private:
    byte x;
    byte y;
  public:
    Point(byte x, byte y) {
      this->x = x;
      this->y = y;
    }
    byte getX() {
      return x;
    }
    byte getY() {
      return y;
    }
    boolean isEqual(int x, int y) {
      return this->x == x && this->y == y;
    }
};

// body of the snake, last element representing the tail, first element representing the head
LinkedList<Point*> snakePositions = LinkedList<Point*>();

// where the apple is located
Point *applePosition;

// For Style ;)
CRGB appleColor = CRGB(255, 0, 0);
CRGB snakeColor = CRGB(0, 255, 0);
CRGB pausedAppleColor = CRGB(0, 255, 255);
CRGB pausedSnakeColor = CRGB(0, 0, 255);
CRGB emptyColor = CRGB::Black;
CRGB solidColor = CRGB(255, 255, 255);

void setup() {
  Serial.begin(9600);

  // setup analog stick
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(PAUSE_PIN, INPUT_PULLUP);

  for (int i = 0; i < 4; i++) {
      pinMode(btn[i], INPUT_PULLUP);   
  }
  
  // pick starting position
  snakePositions.add(getStartingPosition());
  
  // pick apple position
  applePosition = getApplePosition();

  // setup ws2812b leds
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  //FastLED.addLeds<TM1803, LED_PIN, RGB>(leds, NUM_LEDS); //WS2812B TM1803 GW6205_400

  // FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
    // FastLED.addLeds<SM16703, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1829, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1812, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1809, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1804, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1803, LED_PIN, RGB>(leds, NUM_LEDS); //This is the one!!!!!!!!!!
    // FastLED.addLeds<UCS1903, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1903B, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1904, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS2903, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<WS2852, LED_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<WS2812B, LED_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<GS1903, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SK6812, LED_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<SK6822, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<APA106, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<PL9823, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SK6822, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2811, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<APA104, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2811_400, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GE8822, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GW6205, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GW6205_400, LED_PIN, RGB>(leds, NUM_LEDS); // This is also the one??
    // FastLED.addLeds<LPD1886, LED_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<LPD1886_8BIT, LED_PIN, RGB>(leds, NUM_LEDS);
}

void loop() {

  if(btn[0] == LOW) {
    FastLED.setBrightness(30);
    fill_solid(leds, NUM_LEDS, CRGB(255, 0, 0));  // fill red
    FastLED.show();
    delay(2000);
  }
  else if(btn[1] == LOW) {
    FastLED.setBrightness(30);
    fill_solid(leds, NUM_LEDS, CRGB(0, 255, 0));  // fill green
    FastLED.show();
    delay(2000);
  }
  else if(btn[2] == LOW) {
    FastLED.setBrightness(30);
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 255));  // fill blue
    FastLED.show();
    delay(2000);
  }
  else if(btn[3] == LOW) {
    FastLED.setBrightness(30);
    fill_solid(leds, NUM_LEDS, CRGB(0, 255, 255));  // fill cyan
    FastLED.show();
    delay(2000);
  }
  else {
    FastLED.setBrightness(255);
    fill_solid(leds, NUM_LEDS, CRGB(255, 255, 255));  // fill black
    FastLED.show();
    delay(2000);
  }

  

}

// for a point in the matrix, map it to the index in the string
int getIndexForPoint(Point *p) {
  int x = p->getX();
  int y = p->getY();
  boolean oddRow = x % 2 == 1;

  // handle serpentine pattern
//  if(oddRow){
//    return (x + 1) * columns - y - 1;
//  }
  
//  return x * columns  + y;
  return (x + 1) * columns - y - 1;
}

void renderEmptyScreen(){
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = emptyColor;
  }
}

void renderSolidScreen() {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = solidColor;
  }
}

// always start the game in the same spot
Point * getStartingPosition(){
  return new Point(0,0);
}

Point * getApplePosition(){
  int x = random(rows);
  int y = random(columns);

  // make sure to generate an apple that is not in the same position as the snake
  while(snakeContainsPosition(x,y)){
    x = random(rows);
    y = random(columns);
  }
  return new Point(x,y);
}

// check if the x y coordinates are covered by a part of the snake
boolean snakeContainsPosition(int x, int y) {
  for(int i = 0; i < snakePositions.size(); i++){
    if(snakePositions.get(i)->isEqual(x,y)) {
      return true;
    }
  }
  
  return false;
}

// calculate the next position based on the current head position and the current direction
Point * getNextPosition() {
  Point *head = getHead();
  switch(currDirection) {
    case DIR_UP:
      return new Point(head->getX(), head->getY() + 1);
    case DIR_DOWN:
      return new Point(head->getX(), head->getY() - 1);
    case DIR_LEFT:
      return new Point(head->getX() - 1, head->getY());
    case DIR_RIGHT:
      return new Point(head->getX() + 1, head->getY());
    default:
      return new Point(-9, -9);
  }
}

int getYDir(int y) {
  return y > 0 ? DIR_UP : DIR_DOWN;
}

int getXDir(int x) {
  return x > 0 ? DIR_RIGHT : DIR_LEFT;
}

Point * getHead() {
  return snakePositions.get(0);
}

Point * getTail() {
  return snakePositions.get(snakePositions.size() - 1);
}
