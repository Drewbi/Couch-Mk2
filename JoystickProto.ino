/* Motorised couch project
 * - 
 * By Drew Alexander
 */

#define JoyYPin 1
#define JoyXPin 0
#define StartPin 2
#define deadZone 50

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

void setup() {
  Serial.begin(115200);
  pinMode(JoyYPin, INPUT);
  pinMode(JoyXPin, INPUT);
  pinMode(StartPin, INPUT);
  digitalWrite(StartPin, HIGH);

  Serial.print("Initialized\n");
}

void loop() {
  JoyX = analogRead(JoyXPin);
  JoyY = analogRead(JoyYPin);
  
  if (!digitalRead(StartPin) && !start){
    if (JoyX > 512 - deadZone && JoyX < 512 + deadZone){
      start = 1;
      delay(100);
      Serial.print("Running\n");
      centreX = JoyX;
      centreY = JoyY;
    } else {
      Serial.print(JoyX);
      Serial.print(",");
      Serial.print(JoyY);
      Serial.print("\n");
    }
  }
  
  if(start){
    if(JoyY > centreY + deadZone){
      outputB = 0;
      outputF = map((JoyY - (centreY + deadZone)), 0, centreY, 0, 255);
      Serial.print(outputF);
      Serial.print(" Forward\n");
    } else if(JoyY < centreY - deadZone){
      outputF = 0;
      outputB = map((JoyY - (centreY - deadZone))*-1, 0, centreY, 0, 255);
      Serial.print(outputB);
      Serial.print(" Back\n");
    }
  }

}
