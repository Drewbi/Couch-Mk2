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

int started;
int centreX = 512;
int centreY = 512;

int inputX; //0 - 255 in either direction
int directionX; //-1, 0 or 1 if the joystick is back, neutral or forward
int inputY;
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

int getInput(int inputPinX, int centre){
    int JoyIn = analogRead(inputPin);
    int JoyOut;
    if (JoyIn > centre + deadZone){ //Joystick pushed forward
        // Calculate the distance from the edge of the dead zone and map to range 0 - 255
        JoyOut = map((JoyIn - (centre + deadZone)), 0, centre, 0, 255);
    } 
    else if (JoyIn < centre - deadZone){ //Joystick pushed back
        // Calculate the distance from the edge of the dead zone and make positive
        JoyOut = -1 * map((JoyIn - (centre - deadZone)) * -1, 0, centre, 0, 255);
    }
    else{
        JoyOut = 0;
    }
    return JoyOut;
}

void calculateInput(inputX, inputY, currentX, currentY){
    double diffX = inputX - currentX;
    double diffY = inputY - currentY;
    double diffRatio;
    if(diffX < diffY){
        diffRatio = diffX/diffY;
        inputX += round((stepRate/2) * diffRatio);
        inputY += round((stepRate/2) * ((1 - diffRatio) + 1));
    } 
    else if(diffX > diffY){
        diffRatio = diffY/diffX;
        inputY += round((stepRate/2) * diffRatio);
        inputX += round((stepRate/2) * ((1 - diffRatio) + 1));    
    }
    else {
        diffRatio = 1.0; // diffX and diffY are equal
        inputX += round(stepRate/2)
        inputY += round(stepRate/2)
    }
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
