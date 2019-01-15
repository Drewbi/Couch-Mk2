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

#define outPinL 3
#define outPinR 4

#define deadZone 50
#define spinSpeedReduction 2 // Higher number = lower stationary rotation
#define responseSpeed 10 // Speed of change of delayed input
#define swapSpeed 1 // Speed at which couch can change direction.

int started;
int centreX;
int centreY;
int input[2]; // X = [0], Y = [1]
unsigned long currentTime;
unsigned long oldTime;

void setup(){
  Serial.begin(115200);
  pinMode(JoyYPin, INPUT);
  pinMode(JoyXPin, INPUT);
  pinMode(StartPin, INPUT);
  started = 0;
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

int getInput(int inputPin, int centre, double timeChange, int JoyOutSmooth){
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

    if (abs(JoyOut - JoyOutSmooth) < timeChange * responseSpeed){ // If the amount to be added is more than
        JoyOutSmooth = JoyOut; // the difference between the input and the current, make them equal, stops over shooting
    } else {
        if(JoyOut > 0){
            JoyOutSmooth += timeChange * responseSpeed; // Delta time will adjust the response speed 
        } else if(JoyOut < 0){
            JoyOutSmooth -= timeChange * responseSpeed; // so that exactly responseSpeed units will be added each second
        }
    }  
    return JoyOutSmooth;
}

double checkTime(){
    currentTime = millis();
    double timeChange = (double)(currentTime - oldTime) / 1000; // Change into seconds
    oldTime = currentTime;
    return timeChange;
}

void loop(){
    if(!started){
        startup(); // Loops inside itself
        started = 1;
        oldTime = millis();
    }
    double timeChange = checkTime();
    input[0] = getInput(JoyXPin, centreX, timeChange, input[0]);
    input[1] = getInput(JoyYPin, centreY, timeChange, input[1]);
}
