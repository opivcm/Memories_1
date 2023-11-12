// Code for Simon Says Arduino Wiring and Code Tutorial 
// Video found at: https://www.youtube.com/watch?v=TJiz7PT21B4

// Define all the LED and Button pins.
// {RED, GREEN, YELLOW, BLUE}
int buttons[] = {3, 5, 7, 9}; //The four button input pins
int leds[] = {2, 4, 6, 8};  // LED pins

int sequence[100];

int largestIndex = 0;

int buttonPin = 2;

const int START = 0;
const int PLAY = 1;
const int GAMEOVER = 2;

int gameState;

int speed = 500;
boolean running = true;  // Flag to know is knightrider is running

long currentMillis = 0;
long previousMillis = 0;

// the setup routine runs once when you press reset:
void setup() { 

  for(int pin=0; pin<4; pin++)
  {
    pinMode(leds[pin], OUTPUT);
  }
  
  for(int pin=0; pin<4; pin++)
  {
    pinMode(buttons[pin], INPUT_PULLUP);
  }
  
  Serial.begin(9600);

  gameState = START; // Start the game with the start.
  randomSeed(analogRead(40)); // initializes the pseudo-random number generator,
  
}

// the loop routine runs over and over again forever:
void loop() {
  
  if(gameState == START)
  {
    waitToStart();
  }
  else if(gameState == PLAY)
  {
    Serial.println("Start"); 
    showSequence();
    readSequence();
  }
  else if(gameState == GAMEOVER)
  {
    Serial.println("Gameover");
    blinkAll(5);
    gameState = START;
  }
  
}

void showSequence()
{

  //blinkRed(2);
  
  sequence[largestIndex] = random(0,4);
  Serial.println("Next out"); 
  Serial.println(sequence[largestIndex]); 
  largestIndex++;
  
  for(int index=0; index<largestIndex; index++)
  {
     delay(300);
     digitalWrite(leds[sequence[index]], HIGH);
     delay(700);
     digitalWrite(leds[sequence[index]], LOW);
  }
  //blinkGreen(2);
  
}

void readSequence()
{
  
  //blinkYellow(2);
  int positionPressed;
  boolean madeMistake = false;
  

  
  for(int index=0; index<largestIndex && madeMistake == false; index++)
  {
    Serial.println(""); 
    Serial.println("Should push"); 
    Serial.println(sequence[index]);
    
    positionPressed = waitForButton(1000); // 0, 1, 2, or 3

    Serial.println("Pressed"); 
    Serial.println(positionPressed); 
    if(positionPressed == -1 | positionPressed != sequence[index])
    {
        madeMistake = true; // Exit the loop.
        gameState = GAMEOVER;
    }
  }
  //blinkBlue(2);
}



// Returns the position of Button pressed (0, 1, 2, or 3) or -1 if no button is pressed in the time period.
int waitForButton(int delay)
{
  int buttonPressed = -1;
  int input;
  boolean buttonBackUp = false;
  
  currentMillis = millis();     // The number of ms since the program started running
  previousMillis = currentMillis;   // Records the point when we start spinning the loop.
  
  // Keep spinning the loop until "delay" seconds have passed.
  while (currentMillis - previousMillis < delay && buttonBackUp == false)
  {
    // Read the button and record when it has been pushed down.
    for(int pin = 0; pin < 4 && buttonBackUp == false; pin++)
    {
      if(digitalRead(buttons[pin]) == HIGH)
      {
        buttonPressed = pin;
        
        // Show the LED pushed.
        digitalWrite(leds[pin], HIGH);
              
        // It is possible the button is still being pushed.
        // This loop spins until the button is let up.
        while (currentMillis - previousMillis < delay && buttonBackUp == false)
        {
          input = digitalRead(buttons[pin]);
          if(input == LOW)
          {
            buttonBackUp = true;
          }
          currentMillis = millis();
        }
        
        // Turn the LED pushed off.
        digitalWrite(leds[pin], LOW);
        
        // See if they took to long.
        if(currentMillis - previousMillis > delay)
        {
          buttonPressed = -1; // They took to long to let the button up so they lose.
        }
      }
    }

    currentMillis = millis();
  }
  
  return buttonPressed;
}

void waitToStart()
{
  int buttonPressed = -1;
  allOff();
  
  for(int pin = 0; pin < 4; pin++)
  {
    if(buttonPressed == -1)
    {
      digitalWrite(leds[pin], HIGH);
      buttonPressed = waitForButton(800);
      digitalWrite(leds[pin], LOW);
    }
  }
  
  if(buttonPressed != -1)
  {
    // A button was pushed so wait then start playing.
    delay(2000);
    largestIndex = 0; // Restart
    gameState = PLAY; 
  }
}

// Turns all the LEDs off.
void allOff()
{
  for(int pin = 0; pin < 4; pin++)
  {
    digitalWrite(leds[pin], LOW);
  }
}

// Turns all the LEDs on.
void allOn()
{
  for(int pin = 0; pin < 4; pin++)
  {
    digitalWrite(leds[pin], HIGH);
  }
}

// Spins a loop until "delay" milliseconds passes.
// While the loop is spinning we keep looking at the value of the button to see if pushed.
boolean readAnyButton(int delay)
{
  boolean buttonDown = false;
  
  currentMillis = millis();     // The number of ms since the program started running
  previousMillis = currentMillis;   // Records the point when we start spinning the loop.
  
  // Keep spinning the loop until "delay" seconds have passed.
  while (currentMillis - previousMillis < delay && buttonDown == false)
  {
    // Read the button and record when it has been pushed down.
    for(int pin = 0; pin < 4; pin++)
    {
      if(digitalRead(buttons[pin]) == HIGH)
      {
        buttonDown = true;
      }
    }

    currentMillis = millis();
  }
  
  return buttonDown;
}

void blinkAll(int times)
{
  for(int count = 0; count < times; count++)
  {
    allOn();
    delay(300);
    allOff();
    delay(300);
  } 
}

void blinkRed(int times)
{
  for(int count = 0; count < times; count++)
  {
    digitalWrite(leds[0], HIGH);
    delay(300);
    digitalWrite(leds[0], LOW);
    delay(300);
  } 
}

void blinkGreen(int times)
{
  for(int count = 0; count < times; count++)
  {
    digitalWrite(leds[1], HIGH);
    delay(300);
    digitalWrite(leds[1], LOW);
    delay(300);
  } 
}

void blinkYellow(int times)
{
  for(int count = 0; count < times; count++)
  {
    digitalWrite(leds[2], HIGH);
    delay(300);
    digitalWrite(leds[2], LOW);
    delay(300);
  } 
}

void blinkBlue(int times)
{
  for(int count = 0; count < times; count++)
  {
    digitalWrite(leds[3], HIGH);
    delay(300);
    digitalWrite(leds[3], LOW);
    delay(300);
  } 
}
