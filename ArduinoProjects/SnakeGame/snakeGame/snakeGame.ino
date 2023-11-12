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
CRGB emptyColor = CRGB(0, 0, 0);
CRGB solidColor = CRGB(255, 0, 0);

void setup() {
  Serial.begin(9600);

  // setup analog stick
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(PAUSE_PIN, INPUT_PULLUP);
  
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
  // if we're paused set the scene
  checkForPause();
  
  if(isGamePaused) {
    pauseGame();
  }else{
    // set current direction
    currDirection = getCurrentDirection();
  
    // get next position
    Point *nextPoint = getNextPosition();
  
    // check if we are still valid and continue the game, otherwise reset it
    if(isNextPointValid(nextPoint)){
      playGame(nextPoint);
    }else{
      resetGame();
    }
  }
}

// always start the game in the same spot
Point * getStartingPosition(){
  return new Point(0,0);
}

// generate the position of the apple so it is not out of bounds or within the snake
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

// read direction from analog stick
int getCurrentDirection(){
  int dir = currDirection;
  int xPosition = analogRead(xPin);
  int yPosition = analogRead(yPin);
  int mapX = map(xPosition, 0, 1023, -512, 512);
  int mapY = map(yPosition, 0, 1023, -512, 512);

  // The max is 512, so let's assume that if the analog stick is over 3/4ths in one direction then we are good
  // Then choose the direction based on the which value is greater
  //left
  int absX = abs(mapX);
  int absY = abs(mapY);
  int threshold = 384;
  if(absX > threshold || absY > threshold) {
    dir = absX > absY ? getXDir(mapX) : getYDir(mapY);
  }

  //ensure you can't go the direction you just came
  switch(dir) {
    case DIR_UP:
      dir = (currDirection == DIR_DOWN) ? DIR_DOWN : dir;
      break;
    case DIR_DOWN:
      dir = (currDirection == DIR_UP) ? DIR_UP : dir;
      break;
    case DIR_LEFT:
      dir = (currDirection == DIR_RIGHT) ? DIR_RIGHT : dir;
      break;
    case DIR_RIGHT:
      dir = (currDirection == DIR_DOWN) ? DIR_LEFT : dir;
      break;
    default:
      break;
  }

  return dir;
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

void addToBeginning(Point *p){
  snakePositions.add(0, p);
}

void removeTail() {
  delete(snakePositions.pop());
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

// make sure the next point for the head of the snake is in a valid position
boolean isNextPointValid(Point *p) {
  int x = p->getX();
  int y = p->getY();

  // check if within boundary or if we are in the snake
  if(x < 0 || x >= rows || y < 0 || y >= columns || snakeContainsPosition(x,y)) {
    return false;
  }

  return true;
}

// draw the apple
void renderApple() {
  leds[getIndexForPoint(applePosition)] = isGamePaused ? pausedAppleColor : appleColor;
}

// draw the snake
void renderSnake(){
  Point *p;
  for(int i = 0; i < snakePositions.size(); i++) {
    p = snakePositions.get(i);
    int index = getIndexForPoint(p);
    int x = p->getX();
    int y = p->getY();
    leds[index] = isGamePaused ? pausedSnakeColor : snakeColor;
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

void playGame(Point *nextPoint){
  // clear screen
  renderEmptyScreen();
  
  // if we land on an apple grow the snake
  if(applePosition->isEqual(nextPoint->getX(), nextPoint->getY())){
    growSnake(nextPoint);
  }else{
    moveSnake(nextPoint);
  }

  renderSnake();
  renderApple();
  
  FastLED.show();

  delay(gameSpeed);
}

void moveSnake(Point *p){
  addToBeginning(p);
  removeTail();
}

void growSnake(Point *p){
  addToBeginning(p);
  resetApple();
  increaseSpeed();
}

void increaseSpeed() {
  gameSpeed -= SPEED_LOSS;
}

void checkForPause() {
  boolean isPressedDown = digitalRead(PAUSE_PIN) == 0;
  
  boolean shouldPause = false;
  if(isPressedDown) {
    // we are trying to pause the game and the button is held down
    isTogglingPause = true;
  }else{
    // the pause button is released, so let's trigger a pause
    shouldPause = isTogglingPause;
    isTogglingPause = false;
  }

  if(shouldPause) {
    isGamePaused = !isGamePaused;
  }
}

// initiate a different view
void pauseGame(){
  renderSnake();
  renderApple();
  FastLED.show();
}

// delete the snake and create a new one
void resetSnake() {
  while(snakePositions.size() > 0){
    delete(snakePositions.pop());
  }
  snakePositions.add(getStartingPosition());
}

// delete the current position and draw a new apple
void resetApple() {
  delete(applePosition);
  applePosition = getApplePosition();
}

// show an end screen and reset the game state
void resetGame(){
  resetSnake();
  resetApple();
  gameSpeed = MIN_SPEED;
  currDirection = DIR_UP;
  renderSolidScreen();
  FastLED.show();
  delay(resetDelay);
  renderEmptyScreen();
  FastLED.show();
  delay(resetDelay);
  renderSolidScreen();
  FastLED.show();
  delay(resetDelay);
  renderEmptyScreen();
  FastLED.show();
  delay(resetDelay);
  renderSolidScreen();
  FastLED.show();
  delay(resetDelay);
  renderEmptyScreen();
  FastLED.show();
}
