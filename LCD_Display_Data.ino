/*includes the standard arduino header file allowing the Arduino
  to control the LCDs*/
#include <LiquidCrystal.h>

/*creates a variable called lcd of type LiquidCrystal with each
  respective pin number connecting to specific pins, such as
  pin 8's connection to the RS pin and pin 9's connection to
  the RW pin on the LCD, giving an interface between the arduino and the LCD*/
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

// Display settings
/*creates an integer variable called displaySetting and sets it to 
  1 so the LCD display will show the first data display by default*/
int displaySetting = 1;
const int MAX_DISPLAYS = 3; // sets a read-only integer named MAX_DISPLAYS to 3
const int buttonPin = 3; // sets a read-only integer named buttonPin to 3
// stores the number of milliseconds once the program started running
// into a variable of named lastDisplaySwitch with an unsigned long to give lots of memory
unsigned long lastDisplaySwitch = millis(); 
const int DISPLAY_DELAY = 250; // sets a read-only integer named DISPLAY_DELAY to 250

/*
 * VARIABLES FROM TACHOMETER DISPLAY
 */
 
// Pin labels
// sets a read-only integer named interruptPin to 2
const int interruptPin = 2;

// Global variables
/*creates an integer variabe called breakCount and sets it equal to 0. It will be
  incremented every time a break in the IR LED and phototransistor*/
int breakCount = 0; 
/* creates an unsigned long variabled named startTime and is given such a large
 *  amount of memory because it will be set using the millis() function*/
unsigned long startTime = 0;
/* creates an unsigned long variabled named endTime and is given such a large
 *  amount of memory because it will be set using the millis() function*/
unsigned long endTime = 0;
/*sets a global integer variable named rpm to 0 so that it can be changed and read throughout
  the entire program*/
int rpm = 0;
/*sets the read-only integer numberOfBlades to 4, corresponding with the four blades
  on the team's windmill*/
const int numberOfBlades = 4;

/*
 * VARIABLES ADDED FOR POWER OUTPUT
 */
 // sets the global double variable of torqueOut so that it can be changed and read everywhere in the program
double torqueOut = 0.0;
// sets the global double variable of powerOut so that it can be changed and read everywhere in the program
double powerOut = 0.0;

// sets up the desired components of the arduino, including the LCD setup, pin interrupts, and the Serial Monitor
// and is only run once
void setup() {
  // LCD setup
  // initializes interaction with the LCD and sets it to display 16 columns and 2 rows
  lcd.begin(16, 2);
  lcd.display(); //turns on the lcd display
  // end LCD setup

  // LCD pin interrupt
  pinMode(buttonPin, INPUT); //sets buttonPin to behave as input
  /*calls the function changeDisplaySetting whenever the (digital pin converted) form of buttonPin
    is detected to have gone from HIGH to LOW*/
  attachInterrupt(digitalPinToInterrupt(buttonPin), changeDisplaySetting, FALLING);

  // Set up the interrupt pin in input mode
  pinMode(interruptPin, INPUT);

  // Every time the interrupt pin has a falling signal, "broken" will be called
  attachInterrupt(digitalPinToInterrupt(interruptPin), broken, FALLING);

  // Serial interface for debugging
  Serial.begin(9600);
}

// continually run the functions within loop() until the program is exited
void loop() {
  // pauses the program for 1000 ms
  delay(1000);
  // calls on the displayLCD() function to so that it can print data to the LCD
  displayLCD(); // Only changes LCD display once per second
  
  // Change rpm every 30 breakcounts
  if (breakCount >= 30) {
    // calculate how much time has passed since the program started running
    // and can be reduced by startTime to calculate the time that passed for something to happen
    endTime = millis();
    // Calculate RPM by using the number of beams and time
    // and display the RPM result via Serial
    rpm = (float)(breakCount * 60000)/(float)((endTime - startTime) * numberOfBlades);
    // print the RPM to Serial for debugging purposes, ensuring the typecast rpm as a string
    Serial.println("RPM == " + String(rpm));

    // calculate and display to Serial powerOut
    double efficiency = 0.0;
    torqueOut = (rpm - 12800) / (-40);
    Serial.println("torqueOut == " + (String)torqueOut);
    if (torqueOut <= 48)
    {
      efficiency = (-0.001618 * (torqueOut * torqueOut)) + (0.363542 * torqueOut) + 49.376500;
    }
    else if (torqueOut > 48)
    {
      efficiency = (-0.000434 * (torqueOut * torqueOut)) - (0.072269 * torqueOut) + 67.566800;
    }

    double powerInMotor = (-0.000428 * (torqueOut * torqueOut)) + (0.136943 * torqueOut) + 0.000739;
    
    powerOut = efficiency * powerInMotor;
    Serial.println("powerOut == " + (String)powerOut);
    // end calculation of powerOut

    // TODO: Calculate total efficiency
    breakCount = 0;
  }
}

// Called whenever the LCD is called to be updated
void displayLCD()
{
  switch (displaySetting)
  {
    case 1:
      lcd.clear();
      lcd.print(rpm);
      break;
    case 2:
      lcd.clear();
      lcd.print(powerOut);
      break;
    case 3:
      lcd.clear();
      lcd.print("DATA 3");
      break;
    default:
      lcd.clear();
      lcd.print("Unknown Setting!");
  }
}

void changeDisplaySetting()
{
  // Only lets you switch display every DISPLAY_DELAY milliseconds
  if (lastDisplaySwitch + DISPLAY_DELAY < millis())
  {
    lastDisplaySwitch = millis();
    Serial.println("Switching display");
    displaySetting++;
    if (displaySetting > MAX_DISPLAYS)
    {
      displaySetting = 1;
    }
    displayLCD();
  }
}

void broken() {
  if (breakCount == 0) {
    startTime = millis();
  }
  breakCount++;
}

