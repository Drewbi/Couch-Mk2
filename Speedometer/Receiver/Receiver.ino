//Main Arduino 
char velocity = 0;
void setup() {
  
Serial.begin(9600);//begin serial port
while(!Serial){
  
  }
}

void loop() {
  // put your main code here, to run repeatedly:
while (Serial.available()){//if information arrives
velocity = Serial.read();//store variable into velocity.
 Serial.println(velocity);
  
  }
  delay(2000);
}
