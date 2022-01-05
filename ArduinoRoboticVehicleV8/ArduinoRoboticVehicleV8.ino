#define encoderPin 11 // encoder input
#define lineCrossPin 20 // IR sensor that detects reflective tape
#define motor1Pin1 3 // L293D Input 1
#define motor1Pin2 4 // L293D Input 2
#define motor2Pin1 7 // L293D Input 1
#define motor2Pin2 6 // L293D Input 2
#define speedPin 2 // L293D enable 1-2 pin 1. Speed for the left side motor
#define speedPin2 5 // L293D enable 3-4 pin 9. Speed for the right side motor
#define potPin 0 // Potentiometer on analog Pin 0
#define USSEchoPin 8 // Ultrasonic Sensor echo pin
#define USSTrigPin 9 // Ultrasonic Sensor trigger pin
#define lineCrossedPin 10 // IR sensor that detects reflective tape
#define COLUMN 0 // index 0 reffers to column in requred and currentLocation
#define ROW 1 // index 1 reffers to row in requred and currentLocation
#define LEFT 1 // indicates which direction to turn in for function turn90Degrees
#define RIGHT 0 // indicates which direction to turn in for function turn90Degrees
#define turnDelay 320  // delay required to make perfect 90 turn
#define motorOffset 14 // differance between two motor speeds

int Mspeed = 0; // a variable to hold the required speed value
int newSpeed = 0;
int bluetoothByte = 0;
int dataResetCount = 0;
int lastFunction = 0;
int currentLocation[] = {1,1};
int requiredLocation[] = {1,1};
volatile int encoderCounter = 0;
unsigned long USSPulseDuration;
unsigned long distanceToObject = 0;
String dataString;
String connectedString = "Arduino here";
char dir = 'E';
bool movingForwardFlag = false;
bool movingBackFlag = false;
bool isMovingForward = false;
bool isMovingBack = false;
bool remoteControlFlag = false;

bool firstRunFlag = true;
bool mcaFlag = false;
bool controllerFlag = false;
bool onLineFlag = true;
bool encoderFlag = true;
bool lineCrossedFlag = false;
bool objectFoundFlag = false;

void setLineCrossed()
{
  lineCrossedFlag = true;
}

void setup() 
{
  Serial.begin(9600);
  Serial1.begin(9600);
  //set encoder pin as INPUT
  pinMode(encoderPin, INPUT);
  pinMode(lineCrossPin, INPUT);
  pinMode(USSEchoPin, INPUT);
  pinMode(lineCrossedPin, INPUT);
  // set remaining pins as outputs
  pinMode(USSTrigPin, OUTPUT);
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(speedPin, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(speedPin2, OUTPUT);
  newSpeed = analogRead(potPin)/4;
  Mspeed = analogRead(potPin)/4;
  //attachInterrupt(digitalPinToInterrupt(encoderPin), incrementCounter, HIGH);
  //attachInterrupt(digitalPinToInterrupt(lineCrossPin), setLineCrossed, HIGH);
}

void loop() 
{
  //Serial.println(encoderCounter);
  //ultrasonic();
  /*if(firstRunFlag)
  {
    turn90DegreesRight();
  }*/
  
  if (Serial1.available()) 
  {
    bluetoothByte = Serial1.read();
    dataString += char(bluetoothByte);
    
    int endOfString = dataString.indexOf('.');
    
    if(endOfString != -1)
    {
      if(dataString.equals("mca.") || mcaFlag)
      { Serial.println("mcaFlag true");
        if(mcaFlag)
        {
          //Serial1.println(dataString);
          //Serial.println("mcaFlag = true");
          Serial.println("Checking for action");
          checkForAction();
        }
        dataString = "";
        mcaFlag = true;
      }
      if(dataString.equals("controller.") || controllerFlag)
      {
        if(controllerFlag)
        {
          //Serial.println("Problem found");
          Serial.println(dataString);
          basicControls();
        }
        dataString = "";
        controllerFlag = true;
      }
     
    }
  }
  firstRunFlag = false;
  /*else if(isMovingBack)
  {
    if(HIGH == digitalRead(encoderPin))
    {
      encoderCounter--;
      Serial.println(encoderCounter);
    }
  }*/
}

void checkForAction()
{
  if(dataString == "search.")
  {
    remoteControlFlag = false;
    searchForObject();
  }
  else if(dataString == "remote.")
  {
    remoteControlFlag = true;
    Serial1.write("Remote control active \n Now select a location.");
  }
  else if(remoteControlFlag == false)
  {
    Serial1.write("Please request remote control before selecting a location.");
  }


  int containsColon = dataString.indexOf(':');
  if(remoteControlFlag && containsColon != -1)
  {
     moveToLocation();
     //turn90DegreesRight();
  }  
}

void moveToLocation()
{
  String firstChar = dataString.substring(0,1);
  String secondChar = dataString.substring(2,3);
  requiredLocation[COLUMN] = firstChar.toInt();
  requiredLocation[ROW] = secondChar.toInt();
  Serial1.write("Moving to position.");
  Serial.println(dir);
  Serial.print("requiredLocation = ");
  Serial.print(requiredLocation[COLUMN]);
  Serial.print(":");
  Serial.println(requiredLocation[ROW]);
  Serial.println("");
  
  Serial.print("currentLocation = ");
  Serial.print(currentLocation[COLUMN]);
  Serial.print(":");
  Serial.println(currentLocation[ROW]);
  Serial.println("");
  do
  {
    if(dir == 'E')
    {
      while(requiredLocation[COLUMN] > currentLocation[COLUMN])
      {
        distanceToObject = 20;
        //ultrasonic();
        // Wall about 10cm ahead. need to turn now
        if(distanceToObject < 11 && distanceToObject > 0)
        {
          Serial.println("breaking out of E while loop");
          Serial.println("");
          break; // break out of the while loop
        }
        
        if(movingForwardFlag == false)
        {
          moveForward();
        }
        
        if(digitalRead(lineCrossedPin) == LOW && onLineFlag == false)//lineCrossedFlag
        {
          delay(25); // Debouncing to negate false triggers
          if(digitalRead(lineCrossedPin) == LOW && onLineFlag == false)//lineCrossedFlag
          {
            //Serial.println(encoderCounter);
            currentLocation[COLUMN] += 1;
            //encoderCounter = 0;
            onLineFlag = true;
            //lineCrossedFlag = false;
            Serial.print("currentLocation[COLUMN] = ");
            Serial.println(currentLocation[COLUMN]);
            Serial.println("");
            Serial.println("Line crossed heading East");
            Serial.println("");
            //delay(50);
          }
        }
        else if(digitalRead(lineCrossedPin) == HIGH)
        {
          onLineFlag = false;
          //Serial.print("onLineFlag = ");
          //Serial.println(onLineFlag);
        }
        //delay(100);
      }
      delay(500);
      stopMoving();
      delay(500);
      turn90Degrees(RIGHT);
      dir = 'S';
    }
    if(dir == 'S')
    {
      while(requiredLocation[ROW] > currentLocation[ROW])
      {
        //ultrasonic();
        // Wall ahead. need to turn now
        if(distanceToObject < 11 && distanceToObject > 0)
        {
          Serial.println("breaking out of W while loop");
          Serial.println("");
          break; // break out of the while loop
        }
        
        if(movingForwardFlag == false)
        {
          moveForward();
        }
        
        if(digitalRead(lineCrossedPin) == LOW && onLineFlag == false)
        {
          delay(25); // Debouncing to negate false triggers
          if(digitalRead(lineCrossedPin) == LOW && onLineFlag == false)//lineCrossedFlag
          {
            //Serial.println(encoderCounter);
            currentLocation[ROW] += 1;
            //encoderCounter = 0;
            onLineFlag = true;
            Serial.print("currentLocation[ROW] = ");
            Serial.println(currentLocation[ROW]);
            Serial.println("");
            Serial.println("Line crossed heading South");
            Serial.println("");
          }
        }
        else if(digitalRead(lineCrossedPin) == HIGH)
        {
          onLineFlag = false;
        }
      }
      delay(500);
      stopMoving();
      delay(500);
      turn90Degrees(RIGHT);
      dir = 'W';
    }
    if(dir == 'W')
    {
      while(requiredLocation[COLUMN] < currentLocation[COLUMN])
      {
        //ultrasonic();
        // Wall ahead. need to turn now
        if(distanceToObject < 11 && distanceToObject > 0)
        {
          break; // break out of the while loop
        }
        
        if(movingForwardFlag == false)
        {
          moveForward();
        }
        
        if(digitalRead(lineCrossedPin) == LOW && onLineFlag == false)
        {
          delay(25); // Debouncing to negate false triggers
          if(digitalRead(lineCrossedPin) == HIGH && onLineFlag == false)
          {
            //Serial.println(encoderCounter);
            currentLocation[COLUMN] -= 1;
            //encoderCounter = 0;
            onLineFlag = true;
            Serial.println(currentLocation[COLUMN]);
            Serial.println("Line crossed heading West");
          }
          else if(digitalRead(lineCrossedPin) == LOW)
          {
            onLineFlag = false;
          }
        }
      }
      delay(500);
      stopMoving();
      delay(500);
      turn90Degrees(RIGHT);
      dir = 'N';
    }
    if(dir == 'N')
    {
      while(requiredLocation[ROW] < currentLocation[ROW])
      {
        //ultrasonic();
        // Wall ahead. need to turn now
        if(distanceToObject < 11 && distanceToObject > 0)
        {
          break; // break out of the while loop
        }
        
        if(movingForwardFlag == false)
        {
          moveForward();
        }
        
        if(digitalRead(lineCrossedPin) == LOW && onLineFlag == false)
        {
          delay(25); // Debouncing to negate false triggers
          if(digitalRead(lineCrossedPin) == HIGH && onLineFlag == false)
          {
            //Serial.println(encoderCounter);
            currentLocation[ROW] -= 1;
            //encoderCounter = 0;
            onLineFlag = true;
            Serial.println(currentLocation[ROW]);
            Serial.println("Line crossed heading North");
          }
          else if(digitalRead(lineCrossedPin) == LOW)
          {
            onLineFlag = false;
          }
        }
      }
      delay(500);
      stopMoving();
      delay(500);
      turn90Degrees(RIGHT);
      dir = 'E';
    }/**/
  }
  while(requiredLocation[ROW] != currentLocation[ROW] || requiredLocation[COLUMN] != currentLocation[COLUMN]);
  
  if(requiredLocation[ROW] == currentLocation[ROW] && requiredLocation[COLUMN] == currentLocation[COLUMN])
  {
      Serial1.write("Required location reached.");
  }
}

void turn90Degrees(int rotationDirection)
{
  bool reached90 = false;
  encoderCounter = 0;
  Serial.println(Mspeed);
  
  analogWrite (speedPin, Mspeed); // write speed to Enable 1 pin
  analogWrite (speedPin2, Mspeed); // write speed to Enable 9 pin

  if(rotationDirection == LEFT)
  {
    digitalWrite(motor1Pin1, HIGH); // set Input 1 of the L293D low
    digitalWrite(motor1Pin2, LOW); // set Input 2 of the L293D high
    digitalWrite(motor2Pin1, LOW); // set Input 3 of the L293D low
    digitalWrite(motor2Pin2, HIGH); // set Input 4 of the L293D high
  }
  else
  {
    digitalWrite(motor1Pin1, LOW); // set Input 1 of the L293D low
    digitalWrite(motor1Pin2, HIGH); // set Input 2 of the L293D high
    digitalWrite(motor2Pin1, HIGH); // set Input 3 of the L293D low
    digitalWrite(motor2Pin2, LOW); // set Input 4 of the L293D high
  }

  while(!reached90)
  {
    //Serial.println(encoderCounter);
    if(digitalRead(encoderPin)== HIGH && encoderFlag == false)
    {
      delay(15);  // Debouncing to negate false triggers
      if(digitalRead(encoderPin)== HIGH && encoderFlag == false)
      {
        encoderCounter++;
        encoderFlag = true;
        //Serial.print("encoderCounter = ");
        //Serial.println(encoderCounter);
      }
    }
    else if(digitalRead(encoderPin) == LOW)
    {
      encoderFlag = false;
    }
    if(encoderCounter == 4 || encoderCounter > 4)
    {
      delay(turnDelay);
      stopMoving();
      Serial.print("encoderCounter = ");
      Serial.println(encoderCounter);
      Serial.println("");
      encoderCounter = 0;
      reached90 = true;
    }
  }
}

void ultrasonic()
{    
  // 10us trigger
  digitalWrite(USSTrigPin, LOW);  
  delayMicroseconds(2); 
  digitalWrite(USSTrigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(USSTrigPin, LOW);
  
  delayMicroseconds(100);
  USSPulseDuration = pulseIn(USSEchoPin, HIGH);
  distanceToObject = USSPulseDuration * 0.034 / 2;
  Serial.print("distanceToObject = ");
  Serial.println(distanceToObject);
  delay(5);
  //return; 
}

void searchForObject()
{
  Serial1.print("Searching.");
  //ultrasonic();
  //Serial.print(distanceToObject);
  //Serial1.print(distanceToObject);
  //Serial1.write(".");
  int tileCounter = 0;
  do
  {
    if(dir == 'E' && objectFoundFlag == false)
    {
      ultrasonic();
      if(distanceToObject < 64 && distanceToObject > 35)
       {
          Serial.println("Object is 2 tiles away");
          Serial1.write("Object is 2 tiles away.");
          objectFoundFlag = true;
          break;
       }
       else if(distanceToObject < 34 && distanceToObject > 0)
       {
          Serial.println("Object is 1 tile away");
          Serial1.write("Object is 1 tile away.");
          objectFoundFlag = true;
          break;
       }
       else if(distanceToObject > 65)
       {
          Serial.println("No object detected");
       }
       delay(500);
       turn90Degrees(RIGHT);
       dir = 'S';
    }
    if(dir == 'S' && objectFoundFlag == false)
    {
      ultrasonic();
      if(distanceToObject < 64 && distanceToObject > 35)
       {
          Serial.println("Object is 2 tiles away");
          Serial1.write("Object is 2 tiles away.");
          objectFoundFlag = true;
          break;
       }
       else if(distanceToObject < 34 && distanceToObject > 0)
       {
          Serial.println("Object is 1 tile away");
          Serial1.write("Object is 1 tile away.");
          objectFoundFlag = true;
          break;
       }
       else if(distanceToObject > 65)
       {
          Serial.println("No object detected");
       }
       delay(500);
       turn90Degrees(LEFT);
       dir = 'E';
       delay(500);
       moveOneTile();
    }

    if(tileCounter == 2)
    {
      Serial.println("No object in area.");
      objectFoundFlag = true;
    }
    tileCounter++;
  }while(!objectFoundFlag);
}

void moveOneTile()
{
  bool inNextTileFlag = false;
  moveForward();
  do
  {
    Serial.println(onLineFlag);
    if(digitalRead(lineCrossedPin) == LOW && onLineFlag == false)
    {
      delay(25); // Debouncing to negate false triggers
      Serial.println("Made it this far");
      if(digitalRead(lineCrossedPin) == LOW && onLineFlag == false)
      {
        inNextTileFlag = true;
        onLineFlag = true;
        Serial.print("inNextTileFlag =");
        Serial.println(inNextTileFlag);
        //Serial.println(currentLocation[ROW]);
        //Serial.println("Line crossed heading North");
      }
    }
    else if(digitalRead(lineCrossedPin) == LOW)
    {
      onLineFlag = false;
    }
  }while(!inNextTileFlag);
  delay(500);
  stopMoving();
}
 
/************************************************************************************************************
************************************************************************************************************/
void basicControls()
{
 /* if (Serial1.available()) 
  {
    //Serial1.print(connectedString);
    bluetoothByte = Serial1.read();
    //Serial.write(bluetoothByte);
    dataString += char(bluetoothByte);
    Serial.println(dataString);*/
  if ( dataString=="forward.")
  {
    Serial.println("Moving Forward");
    Serial1.write("Moving Forward.");
    moveForward();
    dataString = "";
    dataResetCount = 0;
  }
     
  //Turn Left Command
  else if ( dataString=="left.")
  {
    if(movingForwardFlag)
    {
      Serial1.write("Moving Left.");
      moveLeft();
      dataString = "";
      dataResetCount = 0;
    }
    else if(movingBackFlag)
    {
      Serial1.write("Reversing Left.");
      reverseLeft();
      dataString = "";
      dataResetCount = 0;
    }
    else
    {
      dataString = "";
      dataResetCount = 0;
    }
  }

  //Turn Right Command
  else if ( dataString=="right.")
  {
    if(movingForwardFlag)
    {
      Serial1.write("Moving Right.");
      moveRight();
      dataString = "";
      dataResetCount = 0;
    }
    else if(movingBackFlag)
    {
      Serial1.write("Reversing Right.");
      reverseRight();
      dataString = "";
      dataResetCount = 0;
    }
    else
    {
      dataString = "";
      dataResetCount = 0;
    }
  }    
  
  //Reverse Command
  else if ( dataString=="back.")
  {
    Serial1.write("Moving Back.");
    moveBack();
    dataString = "";
    dataResetCount = 0;
  }
  
  //Increase Speed Command
  else if ( dataString=="faster.")
  {
    //Serial1.println("Increasing Speed.");
    increaseSpeed();
    dataString = "";
    dataResetCount = 0;
  }

  //Decrease Speed Command
  else if ( dataString=="slower.")
  {
    //Serial1.println("Decreasing Speed.");
    decreaseSpeed();
    dataString = "";
    dataResetCount = 0;
  }
  
  else if ( dataString=="stop.")
  {
    //Serial1.println("Stopping");
    Serial.println("Stopping.");
    stopMoving();
    dataString = "";
    dataResetCount = 0;
  }
  else
  {
    dataResetCount++;
    if (dataResetCount > 7)
    {
    //Serial.println("Success");
    dataString = "";
    dataResetCount = 0;
    }
  }
  //}
}

void moveForward()
{
  Serial.println("Moving Forward");
  Serial.println("");
  if(movingForwardFlag == false)
  {
    analogWrite (speedPin, Mspeed/2); // write speed to Enable 1 pin
    analogWrite (speedPin2, Mspeed/2); // write speed to Enable 9 pin
  }
  
  digitalWrite(motor1Pin1, LOW); // set Input 1 of the L293D low
  digitalWrite(motor1Pin2, HIGH); // set Input 2 of the L293D high
  digitalWrite(motor2Pin1, LOW); // set Input 3 of the L293D low
  digitalWrite(motor2Pin2, HIGH); // set Input 4 of the L293D high

  delay(250);
  analogWrite (speedPin, Mspeed-motorOffset); // write speed to Enable 1 pin
  analogWrite (speedPin2, Mspeed); // write speed to Enable 9 pin

  movingForwardFlag = true;
  movingBackFlag = false;
  isMovingForward = true;
  isMovingBack = false;
  lastFunction = 1;
}

void moveBack()
{
  if(movingBackFlag == false)
  {
    analogWrite (speedPin, Mspeed/2); // write speed to Enable 1 pin
    analogWrite (speedPin2, Mspeed/2); // write speed to Enable 9 pin
  }
  digitalWrite(motor1Pin1, HIGH); // set Input 1 of the L293D low
  digitalWrite(motor1Pin2, LOW); // set Input 2 of the L293D high
  digitalWrite(motor2Pin1, HIGH); // set Input 3 of the L293D low
  digitalWrite(motor2Pin2, LOW); // set Input 4 of the L293D high

  delay(250);
  analogWrite (speedPin, Mspeed); // write speed to Enable 1 pin
  analogWrite (speedPin2, Mspeed); // write speed to Enable 9 pin

  movingBackFlag = true;
  movingForwardFlag = false;
  isMovingForward = false;
  isMovingBack = true;
  lastFunction = 2;
}

void moveLeft()
{
  movingBackFlag = false;
  movingForwardFlag = false;
  lastFunction = 3;
  //analogWrite (speedPin, Mspeed/2); // write speed to Enable 1 pin
  //analogWrite (speedPin2, Mspeed*2); // write speed to Enable 9 pin
  
  digitalWrite(motor1Pin1, HIGH); // set Input 1 of the L293D low
  digitalWrite(motor1Pin2, LOW); // set Input 2 of the L293D high
  digitalWrite(motor2Pin1, LOW); // set Input 3 of the L293D low
  digitalWrite(motor2Pin2, HIGH); // set Input 4 of the L293D high
}

void reverseLeft()
{
  movingBackFlag = false;
  movingForwardFlag = false;
  lastFunction = 4;
  //analogWrite (speedPin, Mspeed/2); // write speed to Enable 1 pin
  //analogWrite (speedPin2, Mspeed*2); // write speed to Enable 9 pin
  
  digitalWrite(motor1Pin1, LOW); // set Input 1 of the L293D low
  digitalWrite(motor1Pin2, HIGH); // set Input 2 of the L293D high
  digitalWrite(motor2Pin1, HIGH); // set Input 3 of the L293D low
  digitalWrite(motor2Pin2, LOW); // set Input 4 of the L293D high
}

void moveRight()
{
  movingBackFlag = false;
  movingForwardFlag = false;
  lastFunction = 5;
  //analogWrite (speedPin, Mspeed*2); // write speed to Enable 1 pin
  //analogWrite (speedPin2, Mspeed/2); // write speed to Enable 9 pin
  
  digitalWrite(motor1Pin1, LOW); // set Input 1 of the L293D low
  digitalWrite(motor1Pin2, HIGH); // set Input 2 of the L293D high
  digitalWrite(motor2Pin1, HIGH); // set Input 3 of the L293D low
  digitalWrite(motor2Pin2, LOW); // set Input 4 of the L293D high
}

void reverseRight()
{
  movingBackFlag = false;
  movingForwardFlag = false;
  lastFunction = 6;
  //analogWrite (speedPin, Mspeed*2); // write speed to Enable 1 pin
  //analogWrite (speedPin2, Mspeed/2); // write speed to Enable 9 pin
  
  digitalWrite(motor1Pin1, HIGH); // set Input 1 of the L293D low
  digitalWrite(motor1Pin2, LOW); // set Input 2 of the L293D high
  digitalWrite(motor2Pin1, LOW); // set Input 3 of the L293D low
  digitalWrite(motor2Pin2, HIGH); // set Input 4 of the L293D high
}

void stopMoving()
{
  Serial.println("Stopping Vehicle");
  Serial.println("");
  movingBackFlag = false;
  movingForwardFlag = false;
  isMovingForward = false;
  isMovingBack = false;
  lastFunction = 0;
  //analogWrite (speedPin, Mspeed); // write speed to Enable 1 pin
  //analogWrite (speedPin2, Mspeed); // write speed to Enable 9 pin
  
  digitalWrite(motor1Pin1, LOW); // set Input 1 of the L293D low
  digitalWrite(motor1Pin2, LOW); // set Input 2 of the L293D high
  digitalWrite(motor2Pin1, LOW); // set Input 3 of the L293D low
  digitalWrite(motor2Pin2, LOW); // set Input 4 of the L293D high
}

void increaseSpeed()
{
  if(Mspeed<600)
  {
    Mspeed = Mspeed + 60; // read the speed value from the potentiometer
  }
  
  if (lastFunction==1)
  {
    moveForward();
  }
  else if (lastFunction==2)
  {
    moveBack();
  }
  else if (lastFunction==3)
  {
    moveLeft();
  }
  else if (lastFunction==4)
  {
    reverseLeft();
  }
  else if (lastFunction==5)
  {
    moveRight();
  }
  else if (lastFunction==6)
  {
    reverseRight();
  }
  else
  {
    stopMoving();
  }
}

void decreaseSpeed()
{
  if(Mspeed>60)
  {
    Mspeed = Mspeed - 60; // read the speed value from the potentiometer
  }
  
  if (lastFunction==1)
  {
    moveForward();
  }
  else if (lastFunction==2)
  {
    moveBack();
  }
  else if (lastFunction==3)
  {
    moveLeft();
  }
  else if (lastFunction==4)
  {
    reverseLeft();
  }
  else if (lastFunction==5)
  {
    moveRight();
  }
  else if (lastFunction==6)
  {
    reverseRight();
  }
  else
  {
    stopMoving();
  }
}
