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
#define outPinL 3
#define outPinR 4

#define deadZone 50
#define spinSpeedReduction 2 // Higher number = lower stationary rotation

#define responseSpeed 10 // Speed of change of delayed input
#define swapSpeed 1 // Speed at which couch can change direction.


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

//Delayed output 4 axis
int delayF = 0;
int delayB = 0;
int delayR = 0;
int delayL = 0;

unsigned long newTime = 0;
unsigned long oldTime = 0;
float deltaTime = 0.0;
//Final output values
int outputL = 0;
int outputR = 0;

int speedL = 0;
int speedR = 0;
int speed = 0; // Speed of the fastest wheel

int direction = 0; // Forwards = 1, neutral = 0, back = -1

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

  // Moves the output towards the desired input at responseSpeed
  newTime = millis();
  if (oldTime != 0){
    deltaTime = newTime - oldTime / 1000; // Change into seconds
  }
  oldTime = newTime;


  // Forward desired pos calculation
  if (delayF >= InF){ // If the input is less than the current,
    delayF = InF;     // the current is set to the input with no delay
  } else {
    if (InF - delayF < deltaTime * responseSpeed){ // If the amount to be added is more than
      delayF = InF; // the difference between the input and the current, make them equal, stops over shooting
    } else {
      delayF += deltaTime * responseSpeed; // Delta time will adjust the response speed 
    }  // so that exactly responseSpeed units will be added each second
  }

  // Backward desired pos calculation
  if (delayB >= InB){
    delayB = InB;
  } else {
    if (InB - delayB < deltaTime * responseSpeed){
      delayB = InB;
    } else {
      delayB += deltaTime * responseSpeed;
    } 
  }

  // Right desired pos calculation
  if (InR == 0){
    delayR = 0;
  } else {
    if (InR - delayR < deltaTime * responseSpeed){
      delayR = InR;
    } else {
      delayR += deltaTime * responseSpeed;
    } 
  }

  // Left desired pos calculation
  if (InL == 0){
    delayL = 0;
  } else {
    if (InL - delayL < deltaTime * responseSpeed){
      delayL = InL;
    } else {
      delayL += deltaTime * responseSpeed;
    } 
  }

  if (speedL > speedR){
    speed = speedL;
  } else if (speedR > speedL){
    speed = speedR;
  } else {
    speed = speedL;
  }

  if (InF > 0 && speed < swapSpeed){
    direction = 1;
  } else if (InB > 0 && speed < swapSpeed){
    direction = -1;
  } else if (InF == 0 && InB == 0 && speed < swapSpeed){
    direction = 0;
  }
  // Going places
  if (delayF > 0 && direction == 1){
    outputL = delayF;
    outputR = delayF;
  } else if(delayB > 0 && direction == -1){
    outputL = -1 * delayB; // TODO Figure out how to do reverse
    outputR = -1 * delayB;
  } else if(direction == 0 || delayF == 0 && delayB == 0){
    outputL = 0;
    outputR = 0;
  }
  // Turning
  if(direction == 1 && delayR > 0){
    outputR -= delayR; // TODO Test if this is ok
  } else if(direction == 1 && delayL > 0){
    outputL -= delayL;
  } else if(direction == 0){
    if(delayR > 0){
      outputL = delayR/spinSpeedReduction;
      outputR = -1 * delayR/spinSpeedReduction;
    } else if(delayL > 0){
      outputR = delayL/spinSpeedReduction;
      outputL = -1 * delayL/spinSpeedReduction;
    }
  }
  Serial.print("Left: ");
  Serial.print(outputL);
  Serial.print("| Right: ");
  Serial.print(outputR);
  Serial.println();
  analogWrite(outPinL, outputL);
  analogWrite(outPinR, outputR);

}
