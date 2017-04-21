// Pin labels
const int interruptPin = 2;

// Global variables
int breakCount = 0;
unsigned long startTime = 0;
unsigned long endTime = 0;
int rpm = 0;
int numberOfBlades = 4;

void setup() {
  Serial.begin(9600);
  // Set up the interrupt pin in input mode
  pinMode(interruptPin, INPUT);

  // Every time the interrupt pin has a falling signal, "broken" will be called
  attachInterrupt(digitalPinToInterrupt(interruptPin), broken, FALLING);
}

void loop() {
  if (breakCount >= 30) {
    endTime = millis();
    // Calculate RPM by using the number of beams and time
    // and display the RPM result via Serial
    Serial.println("SUCCESS!");
    double rpm = (float)(breakCount * 60000)/(float)((endTime - startTime) * numberOfBlades);
    Serial.println("RPM == " + String(rpm));
    breakCount = 0;
  }
}

void broken() {
  Serial.println("IN BROKEN");
  if (breakCount == 0) {
    startTime = millis();
  }
  breakCount++;
}

