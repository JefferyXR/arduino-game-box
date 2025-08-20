// Pin definitions
const int yellowButtonPin = 5;
const int blueButtonPin = 6;
const int greenButtonPin = 7;
const int redButtonPin = 8;

const int yellowLEDpin = 9;
const int blueLEDpin = 10;
const int greenLEDpin = 11;
const int redLEDpin = 12;

const int buzzerPin = 4;
const int hapticMotorPin = A5;

// Arrays to store button and LED information
const int buttons[] = {yellowButtonPin, blueButtonPin, greenButtonPin, redButtonPin};
const int leds[] = {yellowLEDpin, blueLEDpin, greenLEDpin, redLEDpin};

// Array to store the random pattern
int pattern[100];
int patternLength = 0;

const int potentiometerPin = A1;
int potValue = 0;
int currentMode = 0;  // 0: Memory Game, 1: Whack-a-Mole
int requiredSequenceLength = 5;

void setup() {
  // Initialize buttons and LEDs
  for (int i = 0; i < 4; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
    pinMode(leds[i], OUTPUT);
  }

  pinMode(buzzerPin, OUTPUT);
  pinMode(hapticMotorPin, OUTPUT);

  // Seed the random number generator
  randomSeed(analogRead(0));

  // Generate the initial random pattern
  generatePattern();

  // Startup animation sequence
  startupAnimation();
}

void startupAnimation() {
  // LED and sound startup sequence
  for (int i = 0; i < 4; i++) {
    digitalWrite(leds[i], HIGH);
    playTone(500 + (i * 100), 200);
    delay(200);
    digitalWrite(leds[i], LOW);
    delay(100);
  }

  // Play a startup sound
  playTone(1000, 500);

  // Blink all LEDs to indicate startup completion
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(leds[j], HIGH);
    }
    delay(200);
    for (int j = 0; j < 4; j++) {
      digitalWrite(leds[j], LOW);
    }
    delay(200);
  }
}

void loop() {
  potValue = analogRead(potentiometerPin);
  currentMode = map(potValue, 0, 1023, 0, 2);  // Adjust the range for the number of modes

  switch (currentMode) {
    case 0:  // Memory Game
      playMemoryGame();
      break;
    case 1:  // Whack-a-Mole
      playWhackAMoleGame();
      break;
  }
}

void playWhackAMoleGame() {
  const int numMoles = 4;
  const int molePins[numMoles] = {yellowButtonPin, blueButtonPin, greenButtonPin, redButtonPin};

  for (int i = 0; i < numMoles; i++) {
    pinMode(molePins[i], OUTPUT);
  }

  int moleDuration = 1000;  // Duration in milliseconds
  unsigned long gameStartTime = millis();
  bool lost = false;

  while (millis() - gameStartTime < 10000 && !lost) {  // Game duration: 10 seconds
    int moleIndex = random(0, numMoles);

    // Activate the mole and light up the corresponding button
    digitalWrite(molePins[moleIndex], HIGH);
    digitalWrite(leds[moleIndex], HIGH);  // Light up the corresponding button

    // Vibrate the haptic motor
    vibrateMotor(100);  // Vibration duration: 100 milliseconds

    unsigned long moleStartTime = millis();
    bool moleWhacked = false;

    while (millis() - moleStartTime < moleDuration) {
      if (digitalRead(buttons[moleIndex]) == LOW) {
        moleWhacked = true;
        digitalWrite(leds[moleIndex], LOW);  // Turn off the button light
        vibrateMotor(100);  // Vibrate on button press
        break;
      }
    }

    digitalWrite(molePins[moleIndex], LOW);
    digitalWrite(leds[moleIndex], LOW);  // Turn off the button light

    // Vibrate the haptic motor even if the wrong button is pressed
    if (!moleWhacked) {
      lost = true;
      playLosingAnimation();  // Vibrate on losing the game
    }

    // Delay before the next mole appears
    delay(random(500, 2000));
  }

  // Turn off all moles and stop haptic motor at the end of the game
  for (int i = 0; i < numMoles; i++) {
    digitalWrite(molePins[i], LOW);
    digitalWrite(leds[i], LOW);
  }
  digitalWrite(hapticMotorPin, LOW);
}

void playWinningAnimation() {
  // LED and sound winning animation
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(leds[j], HIGH);
      playTone(500 + (j * 100), 150);
      delay(150);
      digitalWrite(leds[j], LOW);
    }
    delay(300);
  }

  // Play a victory sound
  playTone(2000, 800);
}

void playLosingAnimation() {
  // LED and sound losing animation
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(leds[j], HIGH);
      playTone(400, 100);
      delay(100);
      digitalWrite(leds[j], LOW);
    }
  }
}

void playMemoryGame() {
  // Display the pattern to the player
  displayPattern();
  
  // Let the player replicate the pattern
  for (int i = 0; i < patternLength; i++) {
    int buttonPressed = waitForButtonPress();

    // Light up the corresponding LED, play sound, and vibrate motor
    digitalWrite(leds[buttonPressed], HIGH);
    playTone(500 + (buttonPressed * 100), 300);
    vibrateMotor(100);
    delay(300);
    digitalWrite(leds[buttonPressed], LOW);
    delay(300);

    if (buttonPressed != pattern[i]) {
      // Wrong button pressed
      gameOver();
      return;
    }
  }

  // Player replicated the pattern successfully
  delay(500);

  // Check for a win when the pattern length reaches the current required length
  if (patternLength == requiredSequenceLength) {
    // Play winning animation
    playWinningAnimation();

    // Increase the required sequence length
    requiredSequenceLength++;

    // Reset the game
    patternLength = 0;
    generatePattern();
  } else {
    generatePattern();
  }
}

void generatePattern() {
  patternLength++;
  pattern[patternLength - 1] = random(0, 4);
}

void displayPattern() {
  for (int i = 0; i < patternLength; i++) {
    digitalWrite(leds[pattern[i]], HIGH);
    playTone(500 + (pattern[i] * 100), 300);
    vibrateMotor(100);
    delay(300);
    digitalWrite(leds[pattern[i]], LOW);
    delay(300);
  }
}

int waitForButtonPress() {
  while (true) {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(buttons[i]) == LOW) {
        while (digitalRead(buttons[i]) == LOW) {
          // Wait for button release
        }
        return i;
      }
    }
  }
}

void playTone(int frequency, int duration) {
  tone(buzzerPin, frequency, duration);
}

void vibrateMotor(int duration) {
  digitalWrite(hapticMotorPin, HIGH);
  delay(duration);
  digitalWrite(hapticMotorPin, LOW);
}

void gameOver() {
  // Play losing animation
  playLosingAnimation();

  // Reset the game
  requiredSequenceLength = 5;  // Reset the required sequence length to its initial value
  patternLength = 0;
  generatePattern();
}