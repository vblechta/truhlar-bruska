#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HX711.h>
#include <AccelStepper.h>
#include <PID_v1.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
HX711 scale;

int seconds = 0;
float forceSet = 7;
float forceMes;
float forceRaw;
float load = 0;
float stepsPSecond = 0.0; // Motor speed +-1000

bool isAutomaticMode = true;

int buttonState1 = LOW;
int buttonState2 = LOW;

const int LOADCELL_DOUT_PIN = 7;
const int LOADCELL_SCK_PIN = 6;

#define dirPin 13
#define stepPin 12
#define motorInterfaceType 1

AccelStepper stepper(motorInterfaceType, stepPin, dirPin); // Moved stepper declaration to global scope

// PID variables
double Setpoint = 0; // forceSet
double Input = 0;    // forceMes
double Output = 127.5;   // 0-255

double Kp = 10, Ki = 8, Kd = 0.2;

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// Function declarations
void automatDisplay();
void set();
void measure();
void controlMotor();

void setup() {
  Serial.begin(57600);
  lcd.begin(20,4,LCD_5x8DOTS);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.print("read: ");
  Serial.println(scale.read());
  scale.set_scale(46919.4583);
  stepper.setMaxSpeed(1000); // Adjust the maximum speed as needed

  // PID setup
  Setpoint = forceSet;
  myPID.SetMode(AUTOMATIC);
  myPID.SetTunings(Kp, Ki, Kd);
}

void loop() {
  measure();
  set();
  controlMotor();
}


void measure() {
  // Get the instant measured value
  forceRaw = scale.get_units();
  forceMes = forceRaw - 0.5; // Corrected the typo

  // Display the instant measured value
  Serial.print("Instant measured value: ");
  Serial.println(forceMes, 1);

  // Display the average value every 5 seconds
  Serial.print("Average value: ");
  Serial.println(scale.get_units(10), 5);

  // Set the PID input to forceMes
  Input = forceMes;

  // PID calculation
  myPID.Compute();

  automatDisplay();
}

void set() {
  // Read button states with simple debouncing
  int reading1 = digitalRead(2);
  int reading2 = digitalRead(3);

  if (reading1 != buttonState1) {
    buttonState1 = reading1;

    if (buttonState1 == HIGH) {
      forceSet -= 0.2;
      Serial.print("forceSet decreased: ");
      Serial.println(forceSet);
    }
  }

  if (reading2 != buttonState2) {
    buttonState2 = reading2;

    if (buttonState2 == HIGH) {
      forceSet += 0.2;
      Serial.print("forceSet increased: ");
      Serial.println(forceSet);
    }
  }

  // Limit forceSet within the range of 2 to 50
  forceSet = constrain(forceSet, 2, 50);

  // Setpoint for PID control
  Setpoint = forceSet;
}

void controlMotor() {
  // Determine motor direction based on forceMes and forceSet
  if (abs(forceMes - forceSet) > 10) {
    // If the difference is greater than 10, run the motor at full speed
    stepsPSecond = (forceMes < forceSet) ? 1000 : -1000;
  } else {
    // Apply proportional control within the dead zone
    stepsPSecond = map(Output, 0, 255, -1000, 1000);
  }

  stepper.setSpeed(stepsPSecond);

  // Run the motor continuously using runSpeed inside the loop
  stepper.runSpeed();
  automatDisplay();
}



void automatDisplay() {
  if (isAutomaticMode) {
    seconds += 1;
    lcd.setCursor(0, 0);
    lcd.print("Fset=");
    lcd.setCursor(5, 0);
    lcd.print(forceSet);
    lcd.setCursor(10, 0);
    lcd.print("N");
    lcd.setCursor(0, 1);
    lcd.print("Fmes=");
    lcd.print(forceMes);
    lcd.setCursor(10, 1);
    lcd.print("N");
   
    lcd.setCursor(13, 0);
    lcd.print("kP=");
    lcd.setCursor(16, 0);
    lcd.print(Kp);

    lcd.setCursor(13, 1);
    lcd.print("kI=");
    lcd.setCursor(16, 1);
    lcd.print(Ki);

    lcd.setCursor(13, 2);
    lcd.print("kD=");
    lcd.setCursor(16, 2);
    lcd.print(Kd);

    lcd.setCursor(0, 2);
    lcd.print("SPS=   ");
    lcd.setCursor(5, 2);
    lcd.print(stepsPSecond);

    lcd.setCursor(0, 3);
    lcd.print("outp=");
    lcd.setCursor(5, 3);
    lcd.print(Output);
  }
}