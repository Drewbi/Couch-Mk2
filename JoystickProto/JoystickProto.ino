/* Motorised couch project
 * - 
 * By Drew Alexander
 * 
 * This program maps joystick input to a middle centred Forward, Back, Left and Right values
 * and then to smoothed left and right wheel values.
 */

#define JoyYPin 0
#define JoyXPin 1
#define StartPin 2
#define deadZone 50
#define responseSpeed 10
#define outPin 3

//Raw voltage inputs
int JoyX = 0;
int JoyY = 0;

//Normalized Joystick values
int InF = 0;
int InB = 0;
int InL = 0;
int InR = 0;

int centreX;
int centreY;
int start = 0;

//Final output values
int outputL = 0;
int outputR = 0;


void setup(){
  Serial.begin(115200);
  pinMode(JoyYPin, INPUT);
  pinMode(JoyXPin, INPUT);
  pinMode(StartPin, INPUT);
  //digitalWrite(StartPin, HIGH); For arduino switch pin
  Serial.print("Initialized\n");
}

void loop(){
  // Before the couch starts the joystick must be inside the dead zone
  while (!start) {
    JoyX = analogRead(JoyXPin);
    JoyY = analogRead(JoyYPin);

    if (digitalRead(StartPin)){
      if (JoyX > 512 - deadZone && JoyX < 512 + deadZone) {
        start = 1;
        Serial.print("Running\n");

        centreX = JoyX;
        centreY = JoyY;
      } else { //Joystick is not in deadzone
        Serial.print(JoyX);
        Serial.print(",");
        Serial.print(JoyY);
        Serial.print("\n");
      }
    }
  }
  //Read Forward and Back input and split into separate values.
  JoyY = analogRead(JoyYPin);
  if (JoyY > centreY + deadZone) { //Joystick pushed forward
    InB = 0;
    // Calculate the distance from the edge of the dead zone and map to range 0 - 255
    InF = map((JoyY - (centreY + deadZone)), 0, centreY, 0, 255);
    Serial.print(InF);
    Serial.print(" Forward\n");
  } else if (JoyY < centreY - deadZone) { //Joystick pushed back
    InF = 0;
    // Calculate the distance from the edge of the dead zone and make positive
    InB = map((JoyY - (centreY - deadZone)) * -1, 0, centreY, 0, 255);
    Serial.print(InB);
    Serial.print(" Back\n");
  } else {
    InF = 0;
    InB = 0;
  }

  //Read Forward and Back input and split into separate values.
  JoyX = analogRead(JoyXPin);
  if (JoyX > centreX + deadZone) { //Joystick pushed forward
    InL = 0;
    // Calculate the distance from the edge of the dead zone and map to range 0 - 255
    InR = map((JoyX - (centreX + deadZone)), 0, centreX, 0, 255);
    Serial.print(InR);
    Serial.print(" Right\n");
  } else if (JoyX < centreX - deadZone) { //Joystick pushed back
    InR = 0;
    // Calculate the distance from the edge of the dead zone and make positive
    InL = map((JoyX - (centreX - deadZone)) * -1, 0, centreX, 0, 255);
    Serial.print(InL);
    Serial.print(" Left\n");
  } else {
    InR = 0;
    InL = 0;
  }

}
