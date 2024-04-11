#include <Arduino.h>
#include <Wire.h>
#include <Preferences.h>
#include <LiquidCrystal_I2C.h>
#include <HX711.h>
#include <AccelStepper.h>
#include <PID_v1.h>
#include <RotaryEncoder.h>
#include <Bounce2.h>
#include <ItemProgress.h>
// #include <ItemInput.h>
#include <ItemCommand.h>
#include <ItemSubMenu.h>
#include <LcdMenu.h>
#include <utils/commandProccesors.h>


#define dirPin 26
#define stepPin 25
#define motorInterfaceType 1
#define LOADCELL_DOUT_PIN 4
#define LOADCELL_SCK_PIN 2
#define ENC_A 27
#define ENC_B 14
#define ENC_SW 23
#define ENDSTOP 18

#define LCD_ROWS 4
#define LCD_COLS 20

// Configure keyboard keys (ASCII) for serial input
#define UP 56        // NUMPAD 8
#define DOWN 50      // NUMPAD 2
#define LEFT 52      // NUMPAD 4
#define RIGHT 54     // NUMPAD 6
#define ENTER 53     // NUMPAD 5
#define BACK 55      // NUMPAD 7

//user defined values
int refresh_interval_display;
int refresh_interval_measure;
int refresh_interval_motor;
int Kp;
int Ki;
int Kd;
int forceSet;
int tolerance;
//end of user defined values
int forceMes;
float forceRaw;
float stepsPSecond = 0.0; // Motor speed +-1000
bool endstop = false;
bool error = false;
bool clear_lcd = false;
bool menu_visible = false;
bool tare_visible = false;
int enc_direction = 0; //rotacni enkoder
int enc_lastState = 0; //rotacni enkoder
int tareVal;
byte menu_type = 0;
String error_desc = "";
String error_desc2 = "";
bool disp_interrupt = false;
int home_step = 0;
int current_step = 0;

Preferences preferences;
AccelStepper stepper(motorInterfaceType, stepPin, dirPin);
LiquidCrystal_I2C lcd(0x27,LCD_COLS,LCD_ROWS);
HX711 scale;
RotaryEncoder encoder(ENC_A, ENC_B, RotaryEncoder::LatchMode::TWO03);
Bounce2::Button button = Bounce2::Button();

// PID variables
double Setpoint = 0; // Set to forceSet in measure() function
double Input = 0;    // Set to forceMes in measure() function
double Output = 127.5;   // 0-255


PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void stepper_drive(void * parameters) {
    while (1) {
        // Run the motor continuously using runSpeed inside the loop
        stepper.runSpeed();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
void settings_get();
void settings_save();
void settings_default();
void tare();
void menuExit();
void menuBack();

#include "menu.h"

// Construct the LcdMenu
LcdMenu menu(LCD_ROWS, LCD_COLS);

#include "menu_boot.h"

void setup() {
    //init serial
    Serial.begin(115200);
    
    //set pinmodes
    pinMode(ENC_A, INPUT);
    pinMode(ENC_B, INPUT);
    pinMode(ENC_SW, INPUT);
    pinMode(ENDSTOP, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    // init preferences
    preferences.begin("settings", false);

    //init scale
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(46919.4583);

    //init stepper
    stepper.setMaxSpeed(800); // Adjust the maximum speed as needed
    stepper.setSpeed(0);
    stepper.setPinsInverted(true, false, false);

    // init button
    button.attach(ENC_SW, INPUT_PULLUP);
    button.interval(10);
    button.setPressedState(LOW);
    
    //init lcd
    lcd.init();
    lcd.backlight();
    
    // Initialize LcdMenu with the menu items
    menu.setupLcdWithMenu(0x27, mainMenu);

    //hide menu
    menu.hide();

    // stepper calibration sequence
    lcd.setCursor(0, 1);
    lcd.print("Pockejte prosim");
    lcd.setCursor(0, 2);
    lcd.print("kalibrace motoru...");
    while (1) {
        if (!digitalRead(ENDSTOP)) {
        stepper.setSpeed(-100);
        stepper.runSpeed();
        }
        else {
            stepper.setSpeed(0);
            Serial.print("Found home: ");
            Serial.println(stepper.currentPosition());
            home_step = stepper.currentPosition();

            stepper.setSpeed(800);
            // stepper.move(-1);
            for (int x = 0; x < 500; x++) {
                stepper.runSpeed();
            }
            Serial.println("Homing done.");
            lcd.setCursor(0, 2);
            lcd.print("motor zkalibrovan  ");
            delay(1000);
            lcd.clear();
            settings_get();
            menu_boot();
            // menu.show();
            break;
        }
    }

    // PID setup
    myPID.SetMode(AUTOMATIC);
    myPID.SetTunings(Kp, Ki, Kd);
    xTaskCreatePinnedToCore (
        stepper_drive,     // Function to implement the task
        "Stepper drive func",   // Name of the task
        10000,      // Stack size in bytes
        NULL,      // Task input parameter
        20,         // Priority of the task
        NULL,      // Task handle.
        0          // Core where the task should run
    );

}

#include "eeprom.h"
#include "display.h"
#include "measure.h"
#include "motor.h"
#include "tare.h"


void menuExit() {
    Serial.println("Zaviram menu");
    menu.resetMenu();
    menu.hide();
    menu_visible = false;
}

void menuBack() {
    menu.back();
}

void loop() {
    current_step = stepper.currentPosition() - home_step;

    // PID calculation
    myPID.Compute();
    
    static unsigned long timer_display = millis();
    static unsigned long timer_measure = millis();
    static unsigned long timer_motor = millis();

    button.update();

    // Encoder reading
    encoder.tick();
    int enc_currPos = encoder.getPosition();
    int sw_state = digitalRead(ENC_SW);
    if (enc_lastState > enc_currPos) {
        enc_lastState = enc_currPos;
        if (menu.isInEditMode()) {
            enc_direction = 3; //LEFT
        }
        else {
            enc_direction = 1; //UP
        }
    }
    else if (enc_lastState < enc_currPos) {
        enc_lastState = enc_currPos;
        if (menu.isInEditMode()) {
            enc_direction = 4; //RIGHT
        }
        else {
            enc_direction = 2; //DOWN
        }
    }
    else if (button.pressed()){
        Serial.println("Button pressed");
        if (menu_visible == false && tare_visible == false) { // if menu is hidden, show menu
            Serial.println("Opening menu");
            menu.resetMenu();
            menu.show();
            menu_visible = true;
        }
        else if (tare_visible == false) { // if menu is visible
            if (menu.isInEditMode()) {
                enc_direction = 6; //BACK
            }
            else {
                enc_direction = 5; //ENTER
            }
        }
    }
    else {
        enc_direction = 0;
    }

    char command = Serial.read();
    processMenuCommand(menu, command, UP, DOWN, ENTER, BACK, LEFT, RIGHT);

    processMenuCommand(menu, enc_direction, 1, 2, 5, 6, 3, 4); // UP, DOWN, ENTER, BACK, LEFT, RIGHT


    if (millis() >= timer_display) {
        if (menu_visible == false && tare_visible == false) {
            display_data();
        }
        else if(tare_visible == true) {
            tare();
        }
        timer_display += refresh_interval_display;
    }

    if (millis() >= timer_measure) {
        measure();
        timer_measure += refresh_interval_measure;
    }

    if (millis() >= timer_motor) {
        if (tare_visible == false) {
            controlMotor();
        }
        timer_motor += refresh_interval_motor;
    }

}
