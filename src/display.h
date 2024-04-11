
void display_data() {
    if (!error) {
        if (clear_lcd) {
            lcd.clear();
            clear_lcd = false;
        }
        lcd.setCursor(0, 0);
        lcd.print("Fset=    ");
        lcd.setCursor(5, 0);
        lcd.print(forceSet);
        lcd.print("N");

        lcd.setCursor(0, 1);
        lcd.print("Fmes=       ");
        lcd.setCursor(5, 1);
        lcd.print(forceMes);
        lcd.print("N");

        lcd.setCursor(0, 2);
        lcd.print("SPS=       ");
        lcd.setCursor(4, 2);
        lcd.print(stepsPSecond);

        lcd.setCursor(0, 3);
        lcd.print("outp=      ");
        lcd.setCursor(5, 3);
        lcd.print(Output);

        lcd.setCursor(13, 0);
        lcd.print("kP=   ");
        lcd.setCursor(16, 0);
        lcd.print(Kp);

        lcd.setCursor(13, 1);
        lcd.print("kI=   ");
        lcd.setCursor(16, 1);
        lcd.print(Ki);

        lcd.setCursor(13, 2);
        lcd.print("kD=   ");
        lcd.setCursor(16, 2);
        lcd.print(Kd);

        lcd.setCursor(12, 3);
        lcd.print("Tare=   ");
        lcd.setCursor(17, 3);
        lcd.print(tareVal);
        
    }
    else { // error == true
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("=======CHYBA:=======");
        lcd.setCursor(0, 1);
        lcd.print(error_desc);
        lcd.setCursor(0, 2);
        lcd.print(error_desc2);
        clear_lcd = true;
    }
}
