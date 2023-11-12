#define DL 500
#define BtnDL 10000
#define afterBtnInputDL 300

// YRGB
int buttons[] = {6, 7, 8, 9}; //The four button input pins
int leds[] = {2, 3, 4, 5};  // LED pins

int sequence[100];
int largestIndex = -1;
int index = -1;


bool GAMEOVER;
int currentMillis, previousMillis;

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

    GAMEOVER = true;
    randomSeed(analogRead(A0)); // initializes the pseudo-random number generator
}

void loop() {
  
    // Press any button to Start
    if(GAMEOVER) {
      allOn();
      GAMEOVER = !(started());
    }
    
    // Game Started
    else {
      delay(DL);
      allOff();
      
      addToSequence();
      showSequence();
      allOff();

      int pins[largestIndex];
      for(int i = 0; i <= largestIndex; i++)
      {
          delay(afterBtnInputDL);
          Serial.println(largestIndex + 1);
          pins[i] = waitForButton(BtnDL);
          Serial.println("Stop");
      }
      Serial.println();
      
      GAMEOVER = checkSequence(pins);

      if(GAMEOVER)
        ResetGameState();
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

// Checks for buttonPress before Starting
bool started()
{
     for(int pin = 0; pin < 4; pin++)
     {
        if(digitalRead(buttons[pin]) == LOW)
        {
          return true;
        }
     }

     return false;
}

// Adds a new leds[] index to sequence[]
void addToSequence() 
{
    sequence[++largestIndex] = random(0, 4);
}

// Blinks all leds[] with index sequence[i]
void showSequence() 
{
    for (int i = 0; i <= largestIndex; i++) 
    {
        digitalWrite(leds[sequence[i]], HIGH);
        delay(DL);
        digitalWrite(leds[sequence[i]], LOW);
        delay(DL);
    }
}

// Returns the position of Button pressed (0, 1, 2, or 3) or -1 if no button is pressed in the time period.
int waitForButton(int del)
{
  int buttonPressed = -1;
  int input;
  boolean buttonBackUp = false;
  
  currentMillis = millis();     // The number of ms since the program started running
  previousMillis = currentMillis;   // Records the point when we start spinning the loop.
  
  // Keep spinning the loop until "delay" seconds have passed.
  while (currentMillis - previousMillis < del && buttonBackUp == false)
  {
    // Read the button and record when it has been pushed down.
    for(int pin = 0; pin < 4 && buttonBackUp == false; pin++)
    {
      if(digitalRead(buttons[pin]) == LOW)
      {
        buttonPressed = pin;
        
        // Show the LED pushed.
        digitalWrite(leds[pin], HIGH);
              
        // It is possible the button is still being pushed.
        // This loop spins until the button is let up.
        while (currentMillis - previousMillis < del && buttonBackUp == false)
        {
          input = digitalRead(buttons[pin]);
          if(input == HIGH)
          {
            buttonBackUp = true;
          }
          currentMillis = millis();
        }
        
        // Turn the LED pushed off.
        digitalWrite(leds[pin], LOW);
        
        // See if they took to long.
        if(currentMillis - previousMillis > del)
        {
          buttonPressed = -1; // They took to long to let the button up so they lose.
        }
      }
    }

    currentMillis = millis();
  }
  return buttonPressed;
}

// Compares pins[] with sequence[]
bool checkSequence(int pins[])
{
    for(int i = 0; i <=largestIndex; i++)
    {
        if(pins[i] != sequence[i])
            return true; // Gameover
    }
    return false;
}

// Resets Global Variables
void ResetGameState()
{
    largestIndex = -1;
    index = -1;
}
