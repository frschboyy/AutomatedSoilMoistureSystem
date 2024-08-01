//Tomato crop
//Loamy Soil -> 12.5% 27% soil moisture (ideal)
//https://support.spruceirrigation.com/knowledge-base/what-is-my-target-moisture-level/
  
#include <LiquidCrystal.h>
#define resistor 10000

LiquidCrystal lcd(1, 2, 4, 5, 6, 7); // Calling LCD object for related input pins

//Variables_/

//Pins
int smPin = A0;			//Analog pin connected to soil moisture sensor
const int dcPin = 13;	//Digital pin connected to DC motor
int prPin = A1;			//Analog pin connected to Photoresistor
int led0 = 12;			//Digital pin connected to red LED
int led1 = 11;			//Digital pin connected to blue LED
int status = 10;		//Digital pin connected to green LED

//Moisture Increase Parameters
long dcStart = 0;					// Motor start time;
const long moistIncreaseInt = 1000;	// Interval for moisture increase (in ms)
float percIncrease = 0.83;			// Percentage increase in moisture

//Moisture Thresholds
const float minMoisture = 12.5;   // Minimum acceptable moisture percentage
const float maxMoisture = 25;     // Maximum acceptable moisture percentage

//Threshold
const float minLux = 780.0;  //Minimum acceptable Lux

int prVal = 0; 	//Light intensity value
float perc = 0;	//Percentage value



void setup() {
  pinMode(led0, OUTPUT);	//Represents non optimal conditions
  pinMode(led1, OUTPUT);	//Represents agreeable conditions
  pinMode(status, OUTPUT);	//Represents the state of tomato plant
  lcd.begin(16, 2);			//Initialize the LCD with 16 columns and 2 rows
  pinMode(dcPin, OUTPUT);	//Set dcPin as output for controlling motor
  digitalWrite(dcPin, LOW);	//Set motor off initially
}

void loop() {
  digitalWrite(status, LOW);	//Set status LED to low
  lightIntensity();				//Call LightSensor
  soilMoisture();				//Call SoilMoisture Sensor

  //Check to see if both light and soil moisture are good...else

  if(prVal >= minLux && (perc <= maxMoisture && perc >= minMoisture)){
    lcd.print("STATUS: GOOD");	//Display message
  }
  else{
    lcd.print("STATUS: BAD");	//Display message
  }
  delay(4000);			//Hold message for 4 seconds
}


//Functions

//Monitoring Presence/Absence of Light
void lightIntensity(){
  digitalWrite(led0,LOW);					//Turn off LED 0
  digitalWrite(led1,LOW);					//Turn off LED 1
  delay(2000); 						//Delay for 2 seconds
  lcd.setCursor(0,0);
  lcd.print("LIGHT INTENSITY");				//Display message
  delay(3000); 							//Hold display for 3 seconds
  lcd.clear();							//Clear display
  prVal = analogRead(prPin);				//Read analog value from photoresistor
  lcd.setCursor(0,0);
  lcd.print("INTENSITY: "+String(prVal));	//Display intensity value on LCD
  delay(2000);					//Hold display for 2 seconds
  
  if(prVal >= minLux){	//If light intensity is above specified threshold
    digitalWrite(led1, HIGH);	//Turn on LED 1
    lcd.setCursor(0,1);
    lcd.print("Light? YES");	//Display Message
  }
  else{ //If light intensity is below specified threshhold
    digitalWrite(led0, HIGH);	//Turn on LED 0
    digitalWrite(status, HIGH);  //Turn on status LED
    lcd.setCursor(0,1);
    lcd.print("Light? NO");		//Display Message
    lcd.clear();			//Clear display
    lcd.setCursor(2,0);
    lcd.print("Light Source");	//Display message
    lcd.setCursor(4,1);
    lcd.print("REQUIRED");      
  }
  delay(3000);	//Delay
  lcd.clear();	//Clear display
}

//Monitoring Soil Moisture
void soilMoisture(){
  digitalWrite(led0,LOW);						//Set LED0 to low
  digitalWrite(led1,LOW);						//Set LED1 to low
  delay(2000); 							//Delay for 2 seconds
  lcd.setCursor(0,0);
  lcd.print("SOIL MOISTURE");
  delay(3000); 					//Delay for 3 seconds to set moisture reading
  lcd.clear();
  perc = convertToPerc(); 						//Read soil moisture
  lcd.setCursor(0, 0);					//Set LCD cursor to position (0, 0)
  lcd.print("Moisture: " + String(perc) + "%");   //Display moisture percentage
  delay(2000); 						 //Hold message for 2 seconds
  lcd.clear();						//Clear the LCD screen

  if (perc < minMoisture) {			//Check is moisture is below min threshhold
    lcd.print("NEEDS WATER :(");	//Display Warning
    digitalWrite(led0, HIGH); 		//Turn on LED 0    
    digitalWrite(status, HIGH); 	//Turn on status LED
    delay(2000);					//Hold message for 2 seconds
    increaseMoisture(perc); 		//Call function to increase moisture
    	
    if(prVal >= minLux)
      digitalWrite(status, LOW); //Turn off status LED
    
    digitalWrite(led0, LOW); 		//Turn off LED 0
    digitalWrite(led1, HIGH); 		//Turn on LED 1
  } 
  else if (perc > maxMoisture) { 	//Check if moisture is above max threshhold
    lcd.print("OVERSATURATED :(");	//Display Warning
    digitalWrite(dcPin, LOW);		//Turn off DC motor
    digitalWrite(led0, HIGH); 		//Turn on LED 0
    digitalWrite(status, HIGH); 		//Turn on status LED
    lcd.setCursor(0, 1);
    lcd.print("drain!!!");			//Display instructions
  } 
  else {
    lcd.print("HEALTHY :)");	//Display healthy message
    digitalWrite(dcPin, LOW);	//Turn off DC motor
    digitalWrite(led1, HIGH); 	//Turn on LED1
  }
  delay(2000);	//Delay for 2 seconds
  lcd.clear();
}




//Convert Analog SOil Moisture Reading to Percentage
int convertToPerc() {
  float smVal = analogRead(smPin);
  float percentage = map(smVal, 0, 876, 0, 100);
  return percentage;
}

//Increase Moisture Content in Soil - Watering Plant
void increaseMoisture(float perc) {
  int hold = 0;		//Initialize a variable to hold time
  lcd.clear();
  lcd.print("...WATERING...");
  delay(1000);
  
  //Continue loop while current moisture is below maximum
  while (perc < maxMoisture ) { 
    dcStart = millis() - hold; 	//Calculate motor start time at begining of each loop taking delays into account
    lcd.clear(); 				//Clear LCD screen
    lcd.setCursor(0, 0);
    lcd.print("Increasing");	//Display message
    digitalWrite(dcPin, HIGH); 	//Turn on DC motor
    delay(1000); 				//Motor runs for 1 second
    float timeElapsed = millis() - dcStart; 		//Calculate time elapsed since loop began (motor start time)
    float increaseAmount = percIncrease * (timeElapsed / moistIncreaseInt);  //Calculate moisture increase
    float newMoisture = perc + increaseAmount; 		//Calculate new moisture
    lcd.setCursor(0, 1); 
    lcd.print("moisture: " + String(newMoisture) + "%"); 	//Display new moisture
    int pause = millis(); 		//Record time of message
    delay(750); 				//Holds message for 0.750 seconds
    int hold = millis()-pause; 	//Calculate delay
    perc = newMoisture; 		//Updates moisture value
  }
  digitalWrite(dcPin, LOW); //Turn off DC motor
}