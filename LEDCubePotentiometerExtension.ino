/* LED cube potentiometer code
 * 
 *
 * == Setting up the Serial Monitor ==
 * The Serial Monitor must be configured (bottom-right corner of the screen) as:
 *   - Newline (for the line ending)
 *   - Baud rate 115200
 *
 * ENGR 40M
 * July 2017
 */

// Arrays of pin numbers. Fill these in with the pins to which you connected
// your anode (+) wires and cathode (-) wires. Hint: if you order these
// intelligently, it will make your mapping getLEDState() function a lot
// simpler!
const byte ANODE_PINS[8] = {6, 7, 8, 9, 2, 3, 4, 5};
const byte CATHODE_PINS[8] = {A2, A3, A4, A5, 11, 12, 10, 13};
const byte knobPin = A0;
const long TIME_DELAY = 150;
const int MAX_RESISTOR_VALUE = 1023;

void setup()
{
  // Make all of the anode (+) wire and cathode (-) wire pins outputs
  for (byte i = 0; i < 8; i++) {
    pinMode(ANODE_PINS[i], OUTPUT);
    pinMode(CATHODE_PINS[i], OUTPUT);
  }
  // Set all LEDs to OFF state
  for (byte j = 0; j < 8; j++) {
    digitalWrite(ANODE_PINS[j], HIGH);
    digitalWrite(CATHODE_PINS[j], LOW);
  }
  pinMode(knobPin, INPUT_PULLUP);
  // Initialize serial communication
  // (to be read by Serial Monitor on your computer)
  Serial.begin(115200);
  Serial.setTimeout(100);
}

/* Function: getLEDState
 * ---------------------
 * Returns the state of the LED in a 4x4x4 pattern array, each dimension
 * representing an axis of the LED cube, that corresponds to the given anode (+)
 * wire and cathode (-) wire number.
 *
 * This function is called by display(), in order to find whether an LED for a
 * particular anode (+) wire and cathode (-) wire should be switched on.
 */
inline byte getLEDState(byte pattern[4][4][4], byte aNum, byte cNum)
{
  byte aMap = aNum % 4;
  byte cMap = cNum % 4;
  byte x;
  byte y;
  byte z;
  byte anodeZ1;
  byte anodeZ2;
  byte cathodeZ1;
  byte cathodeZ2;
  // Switch statement utilizes organization of circuitry to find x coordinate using aNum % 4
  switch (aMap) {
    case 0:
      x = 0;
    break;
    case 1:
      x = 1;
    break;
    case 2:
      x = 2;
    break;
    case 3:
      x = 3;
    break;
  }
  // Switch statement utilizes organization of circuitry to find y coordinate using cNum % 4
  switch (cMap) {
    case 0:
      y = 0;
    break;
    case 1:
      y = 1;
    break;
    case 2:
      y = 2;
    break;
    case 3:
      y = 3;
    break;
  }
  // Finds the two possible z coordinates associated with the given anode number based upon circuitry
  if (aNum == 0 || aNum == 1 || aNum == 2 || aNum == 3) {
    anodeZ1 = 1;
    anodeZ2 = 0; 
  } else {
    anodeZ1 = 2;
    anodeZ2 = 3;
  }
  // Finds the two possible z coordinates associated with the given cathode number based upon the circuitry
  if (cNum == 0 || cNum == 1 || cNum == 2 || cNum == 3) {
    cathodeZ1 = 0;
    cathodeZ2 = 3;
  } else {
    cathodeZ1 = 1;
    cathodeZ2 = 2;
  }
  // Finds value of z using conditional logic
  if (cathodeZ1 == anodeZ1 || cathodeZ1 == anodeZ2) {
    z = cathodeZ1;
  } else {
    z = cathodeZ2;
  }
  return pattern[x][y][z];
}

/* Function: display
 * -----------------
 * Runs through one multiplexing cycle of the LEDs, controlling which LEDs are
 * on.
 *
 * During this function, LEDs that should be on will be turned on momentarily,
 * one row at a time. When this function returns, all the LEDs will be off
 * again, so it needs to be called continuously for LEDs to be on.
 */
void display(byte pattern[4][4][4])
{
  for (byte aNum = 0; aNum < 8; aNum++) { // iterate through anode (+) wires

    // Set up all the cathode (-) wires first
    for (byte cNum = 0; cNum < 8; cNum++) { // iterate through cathode (-) wires
      byte value = getLEDState(pattern, aNum, cNum); // look up the value
      if (value > 0) {
        digitalWrite(CATHODE_PINS[cNum], LOW);
      } else {
        digitalWrite(CATHODE_PINS[cNum], HIGH);
      }
    }

    // Activate the anode (+) wire (without condition)
    digitalWrite(ANODE_PINS[aNum], LOW);
    // Wait a short time
    delayMicroseconds(100);

    // We're now done with this row, so deactivate the anode (+) wire
    digitalWrite(ANODE_PINS[aNum], HIGH);
  }
}


/* Function: movePatternDown
 * -------------------------
 * Moves the pattern down by one. That is, ledOn[x][y][z] <- ledOn[x][y][z-1].
 * On return, the top plane (z == 3) will be a duplicate of the second plane
 * (z == 2).
 */
void movePatternDown(byte pattern[4][4][4]) {

  // Set bottom three layers equal to layer above it
  for (byte z = 3; z > 0; z--) {
    for (byte y = 3; y > 0; y--) {
      for (byte x = 3; x > 0; x--) {
        pattern[x][y][z] = pattern[x][y][z-1];
      }
    }
  }
  // Turn off top layer of LEDs
  
  for (byte y2 = 0; y2 < 4; y2++) {
    for (byte x2 = 0; x2 < 4; x2++) {
      pattern[x2][y2][0] = LOW; 
    }
  }
  
}

/* Function: chooseRandomInTopPlane
 * ------------------------------
 * Replaces the top plane pattern with a number of randomly-chosen LEDs.
 */
void chooseRandomInTopPlane(byte pattern[4][4][4]) {
    // Assumes the top plane has already been cleared (all lights set off)
    // Only turns one new light
    int y = random(0,4);
    int x = random(0,4);
    int z = 0;
    // Set the value for this random (x,y) in the top plane (z=0)
    // of your pattern so that the LED will be on.
    pattern[x][y][z] = HIGH;
}

/* Function: allOff
 * ------------------------------
 * Turns off all lights in the cube.
 * (You can delete this function if you don't think you need it.)
 */
void allOff(byte pattern[4][4][4]) {
  for (int z = 0; z < 4; z++) {
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        pattern[x][y][z] = LOW;
      }
    }
  }
}

// Turn on an entire layer of LEDs 
void turnOnLayer(byte pattern[4][4][4], int z) {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      pattern[x][y][z] = HIGH;
    }
  }
}

// Turn off an entire layer of LEDs
void turnOffLayer(byte pattern[4][4][4], int z) {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      pattern[x][y][z] = LOW;
    }
  }
}




void loop() {
    static byte pattern[4][4][4];
    static long nextUpdateTime = millis();
    static byte state;
    static int knobValue = analogRead(knobPin);
    static byte currPattern;

    // Get user input to change state of the cube
    byte user_input = 0;
    if (Serial.available() > 0) {
        user_input = Serial.read();
        state = user_input;
    }
    
    // Read value of potentiometer and determine what state the cube should be in
    knobValue = analogRead(knobPin);
    if (knobValue > (MAX_RESISTOR_VALUE / 2)) {
      currPattern = 'r';
    } else {
      currPattern = 'z';
    }
    
    switch(state) {
        case 'g':
            switch(currPattern) {
              // Create Raindrop pattern if in raindrop state
              case 'r':
                if (millis() >= nextUpdateTime) {
                  Serial.println("Update Raindrops");
                  movePatternDown(pattern);
                  chooseRandomInTopPlane(pattern);
                  nextUpdateTime = millis() + TIME_DELAY;
                }
                display(pattern);
                break;
              // Create alternating layer pattern if in layer state
              case 'z':
                Serial.println("Update Layers");
                // Move layers down
                for (int i = 0; i < 4; i++) {
                  turnOnLayer(pattern, i);
                  display(pattern);
                  delay(TIME_DELAY);
                  turnOffLayer(pattern, i);
                }
                // Move layers back up
                for (int i = 2; i >= 1; i--) {
                  turnOnLayer(pattern, i);
                  display(pattern);
                  delay(TIME_DELAY);
                  turnOffLayer(pattern, i);
                }
                break; 
            }
            break;
        // Pause the pattern in its current state
        case 'p':
            Serial.println("Pause");
            display(pattern);
            break;
        // Turn off all layers and end the pattern
        case 's':
            Serial.println("Stop");
            allOff(pattern);
            display(pattern);
            break;
    }
    
}




