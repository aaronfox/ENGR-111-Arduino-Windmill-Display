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
  1 so the LCD will show the first data display by default*/
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

/*
 * VARIABLES ADDED FOR SYSTEM EFFICIENCY 
 */
 // sets the global double sys_eff so that it can be changed and read everywhere in the program
 // and eventually displayed on the LCD
 double sys_eff = 0.0;

// sets up the desired components of the arduino, including the LCD setup, pin interrupts, and the Serial Monitor
// and is only run once
void setup() {
  // LCD setup
  // initializes interaction with the LCD and sets it to display 16 columns and 2 rows
  lcd.begin(16, 2);
  lcd.display(); //turns on the LCD
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
  displayLCD(); // Only changes LCD once per second
  
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

    // create a double named efficiency so so that it can be printed to the LCD 
    double efficiency = 0.0;
    // calculates the torque being output with the equation extrapolated from 
    // the ratio of torque_in * rpm_in = torque_out * rpm_out
    torqueOut = (rpm - 12800) / (-40);

    // only execute this portion if the torque_out is less than or equal 48 g-cm
    if (torqueOut <= 48)
    {
      // sets efficiency equal to the y = −0.001618x^2 + 0.363542x + 49.376500
      // which relates motor efficiency and torque
      efficiency = (-0.001618 * (torqueOut * torqueOut)) + (0.363542 * torqueOut) + 49.376500;
    }
    // else execute the formula used for when torque_out is greater than 48 g-cm
    else if (torqueOut > 48)
    {
      // sets efficiency equal to the y = −0.000434x^2 − 0.072269x + 67.566800
      // which relates motor efficiency and torque
      // and implicitly converts g-cm to N-m
      efficiency = (-0.000434 * (torqueOut * torqueOut)) - (0.072269 * torqueOut) + 67.566800;
    }

    // calculates the power input to the motor using the equation relating relating torque
    // and power: y = −0.000428x^2 + 0.136943x+ 0.000739
    double powerInMotor = (-0.000428 * (torqueOut * torqueOut)) + (0.136943 * torqueOut) + 0.000739;

    // calculates power_out by multiplying the efficiency by power_in
    powerOut = efficiency * powerInMotor;
    // prints power_out to Serial for debugging purposes
    Serial.println("powerOut == " + (String)powerOut);
    // end calculation of powerOut

    // Calculate torqueIn so sys_eff can be calculated
    const int GEAR_RATIO = 8; // read-only integer that divides the 64 tooth gear by 8 tooth gear, yielding 8
    double rpm_out = GEAR_RATIO * rpm; // calculates rpm output using the equation
    // the 0.00009... converts torque in to N-m from g-cm
    double powerIn = (torqueIn * 0.0000980665 * rpm / 9549) *1000; // multiply by 1000 to convert kW to W
    // calculate system efficiency by dividing power output by power input
    sys_eff = (powerOut / powerIn);
    // print system efficiency to Serial for debugging purposes
    Serial.println("sys_eff == " + (String)sys_eff);

    // resets the breakCount so the loop can actually recount to 30 before entering this loop again
    // otherwise, it would never escape this loop!
    breakCount = 0;
  }
}

// Called whenever the LCD is called to be updated
void displayLCD()
{
  // switch case statement comparing the integer displaySetting
  // to the 3 cases specified
  switch (displaySetting)
  {
    // if displaySetting == 1
    case 1:
      // clears LCD to prevent stacking of output
      lcd.clear();
      // prints rpm to LCD 
      lcd.print((String)rpm + " RPM");
      // break so the other cases are avoided
      break;
    // if displaySetting == 2
    case 2:
      // clears LCD to prevent stacking of output
      lcd.clear();
      // prints power_out to LCD in 
      lcd.print((String)(powerOut) + " W");
      // break so the other cases are avoided
      break;
    // if displaySetting == 3
    case 3:
      // clears LCD to prevent stacking of output
      lcd.clear();
      // prints the system efficiency to LCD as a percentage
      lcd.print((String)(sys_eff * 100) + "%");
      // break so the other cases are avoided
      break;
    // else, displaySetting somehow got to be a number it shouldn't be
    default:
      // clears LCD to prevent stacking of output
      lcd.clear();
      // prints to LCD that that somehow displaySetting became something other than the numbers 1,2, or 3
      lcd.print("Unknown Setting!");
  }
}

// changes display setting to be between 1 and 3
void changeDisplaySetting()
{
  // Only lets you switch display every DISPLAY_DELAY milliseconds
  if (lastDisplaySwitch + DISPLAY_DELAY < millis())
  {
    // figure out the time which the last time displaySwitch has been changed
    // so we can make sure to only switch display eveery DISPLAY_DELAY milliseconds
    lastDisplaySwitch = millis();
    // print to Serial that the displaySetting variable is changing
    Serial.println("Switching display");
    // increment displaySetting by one
    displaySetting++;
    // reset display setting back to MAX_DISPLAYS to avoid having a displaySetting that is higher than 3
    if (displaySetting > MAX_DISPLAYS)
    {
      // reset displaySetting to 1
      displaySetting = 1;
    }
    // calls on displayLCD() so that it can check the new displaySetting and change it's
    // output to the LCD accordingly
    displayLCD();
  }
}

// increments breakCouunt every time the IR led and phototransistors's signal is broken
// i.e. the windmill blades came between the two
void broken() {
  // if the breakCount was reset to 0, obtain this startTime so that it can be substracted from the time 
  // that is recorded at the endTime at 30 breaks and used in calculating the RPM
  if (breakCount == 0) {
    // record the current time from the program startto now into startTime so that it can be subtracted from endTime
    startTime = millis();
  }
  // increments breakCount by one each time broken is called
  breakCount++;
}

