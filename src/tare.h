void tare() {
    if (tare_visible == false) { // do this once
        stepper.setSpeed(0); // stop stepper
        menu.resetMenu();
        menu.hide();
        menu_visible = false;
        tare_visible = true;
        lcd.clear();
    }
    lcd.setCursor(0, 0);
    lcd.print("Tare - nulovani");
    lcd.setCursor(0, 1);
    lcd.print("Ulozena: ");
    lcd.print(tareVal);
    lcd.setCursor(0, 2);
    lcd.print("Aktualni:     ");
    lcd.setCursor(9, 2);
    lcd.print(forceMes);
    lcd.setCursor(0, 3);
    lcd.print("[ENTER] pro ulozeni");

    static int counter = 2;
    Serial.println(counter);
    if (counter <= 0) {
        if (digitalRead(ENC_SW) == LOW) {
            Serial.println("Tare close!");
            tareVal = forceMes;
            settings_save();
            lcd.clear();
            lcd.setCursor(3, 1);
            lcd.print("Ulozeno!");
            delay(1000);
            tare_visible = false;
            menu_visible = true;
            menu.resetMenu();
            menu.show();
        }
    }
    else if (counter > 0) {
        counter--;
    }
}