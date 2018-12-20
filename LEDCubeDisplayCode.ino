/* LED cube test code
 *
 * Reads (x,y,z) co-ordinates from the Serial Monitor and toggles the state of
 * the LED at that co-ordinate. The co-ordinates are specified as "x y z", e.g.
 * "1 2 2", followed by a newline. Invalid co-ordinates are rejected.
 *
 * You need to fill in all the places marked TODO.
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

void loop()
{
  static byte ledPattern[4][4][4]; // 1 for on, 0 for off

  byte x = 0;
  byte y = 0;
  byte z = 0;
  static char message[60];

  if (Serial.available()) {
    // Parse the values from the serial string
    x = Serial.parseInt();
    y = Serial.parseInt();
    z = Serial.parseInt();

    // Check for input validity
    if (Serial.read() != '\n') {
      Serial.println("invalid input - check that line ending is set to \"Newline\"; input must be three numbers");
      return;
    }
    if (x < 0 || x > 3 || y < 0 || y > 3 || z < 0 || z > 3) {
      sprintf(message, "x = %d, y = %d, z = %d -- indices out of bounds", x, y, z);
      Serial.println(message);
      return;
    }

    // Print to Serial Monitor to give feedback about input
    sprintf(message, "x = %d, y = %d, z = %d", x, y, z);
    Serial.println(message);

    // Toggle the LED state
    ledPattern[x][y][z] = !ledPattern[x][y][z];
  }

  // This function gets called every loop
  display(ledPattern);
}

