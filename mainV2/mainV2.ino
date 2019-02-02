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
#define stepRate 10 // Speed of change of delayed input
#define swapSpeed 1 // Speed at which couch can change direction.

// Global Variables
int started;
int centreX = 512;
int centreY = 512;
unsigned long oldTime;

int inputX; //0 - 255 in either direction
int directionX; //-1, 0 or 1 if the joystick is back, neutral or forward
int inputY;
double smoothedY;
int directionY;

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
    while(true){
        if (digitalRead(StartPin)){
            int JoyX = analogRead(JoyXPin);
            int JoyY = analogRead(JoyYPin);
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
    oldTime = micros();
}

int getInput(int inputPin, int centre){
    int JoyIn = analogRead(inputPin);
    int JoyOut = 0;
    if (JoyIn > centre + deadZone){ //Joystick pushed forward
        // Calculate the distance from the edge of the dead zone and map to range 0 - 255
        JoyOut = map((JoyIn - (centre + deadZone)), 0, centre, 0, 255);
    } 
    else if (JoyIn < centre - deadZone){ //Joystick pushed back
        // Calculate the distance from the edge of the dead zone and make positive
        JoyOut = -1 * map((JoyIn - (centre - deadZone)) * -1, 0, centre, 0, 255);
    }
    return JoyOut;
}

void accelerationThrottle(int inputY, double currentY, double timeDiff){
    yStep = timeDiff * stepRate;
    if (inputY + yStep < (timeChange * responseSpeed)){ // If the amount to be added is more than
        JoyOutSmooth = JoyOut; // the difference between the input and the current, make them equal, stops over shooting
    } else {
        if(JoyOut > 0){
            JoyOutSmooth += (timeChange * responseSpeed); // Delta time will adjust the response speed 
        } else if(JoyOut < 0){
            JoyOutSmooth -= (timeChange * responseSpeed); // so that exactly responseSpeed units will be added each second
        }
    } 
}

double getTimeDiff(){
    unsigned long currentTime = micros();
    double timeDiff = (double)(currentTime - oldTime)/(double)1000000; // Change into seconds
    oldTime = currentTime;
    return timeDiff;
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
