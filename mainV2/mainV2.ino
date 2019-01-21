/* Motorised couch project
 * Version 2.0
 * By Drew Alexander
 * 
 * This program maps joystick input to a middle centred Forward, Back, Left and Right values
 * and then to smoothed left and right wheel values.
 */

#define JoyYPin 0
#define JoyXPin 1
#define StartPin 2

#define outPWMPinL 3
#define outDirPinL 4
#define outPWMPinR 5
#define outDirPinR 6

#define deadZone 50
#define spinSpeedReduction 2 // Higher number = lower stationary rotation
#define responseSpeed 10 // Speed of change of delayed input *change to step rate
#define swapSpeed 1 // Speed at which couch can change direction.

int directional = 0;
int turningDirection = 0; //Left = -1, Right = 1
int started;
int centreX = 512;
int centreY = 512;
int input[2]; // X = [0], Y = [1]
//unsigned long currentTime;
//unsigned long oldTime;

void setup(){
    Serial.begin(115200);
    pinMode(JoyYPin, INPUT);
    pinMode(JoyXPin, INPUT);
    pinMode(StartPin, INPUT);
    pinMode(StartPin, INPUT);
    pinMode(outPWMPinL, OUTPUT);
    pinMode(outPWMPinR, OUTPUT);
    pinMode(outDirPinL, OUTPUT);
    pinMode(outDirPinR, OUTPUT);

    started = 1;
    Serial.println("Initialized");    
}

void startup(){
    int JoyX;
    int JoyY;
    while(true){
        if (digitalRead(StartPin)){
            JoyX = analogRead(JoyXPin);
            JoyY = analogRead(JoyYPin);
            if (JoyX > 512 - deadZone && JoyX < 512 + deadZone) {
                Serial.print("Running\n");
                break;
            }
            Serial.println("Joystick not inside deadzone.");
            Serial.print("X: ");
            Serial.print(JoyX);
            Serial.print(", Y: ");
            Serial.print(JoyY);
            Serial.println();
        }
    }
    centreX = JoyX;
    centreY = JoyY;
}

int getInput(int inputPin, int centre){
    int JoyIn = analogRead(inputPin);
    int JoyOut;
    if (JoyIn > centre + deadZone){ //Joystick pushed forward
        // Calculate the distance from the edge of the dead zone and map to range 0 - 255
        JoyOut = map((JoyIn - (centre + deadZone)), 0, centre, 0, 255);
        // Serial.println(JoyOut);
    } 
    else if (JoyIn < centre - deadZone){ //Joystick pushed back
        // Calculate the distance from the edge of the dead zone and make positive
        JoyOut = -1 * map((JoyIn - (centre - deadZone)) * -1, 0, centre, 0, 255);
        // Serial.println(JoyOut);
    }
    else{
        JoyOut = 0;
        // Serial.println(JoyOut);
    }
    return JoyOut;
}

int getOutputDirection(int input){
   if(input < 0){
       return -1;//negative position
   }
  else if(input > 0 ){
      return 1;// positive position
  }
  else{
      return 0;// netural postition
  }
}

int calculateTurningWheel(int X, int Y){
    int wheelVal = Y - X;
    if(wheelVal < 0){
        wheelVal = 0;
    }
    return wheelVal;
}


void loop(){
    if(!started){
        startup(); // Loops inside itself
        started = 1;
    }
    input[0] = getInput(JoyXPin, centreX);
    input[1] = getInput(JoyYPin, centreY);
    delay(10);
}
