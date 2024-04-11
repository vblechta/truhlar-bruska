void settings_get() {
    refresh_interval_display = preferences.getUInt("intervDisp",0);
    refresh_interval_measure = preferences.getUInt("intervMes",0);
    refresh_interval_motor = preferences.getUInt("intervMot",0);
    forceSet = preferences.getUInt("forceSet", 0);
    tareVal = preferences.getUInt("tareVal", 0);
    tolerance = preferences.getUInt("tolerance", 0);
    Kp = preferences.getUInt("PID_p", 0);
    Ki = preferences.getUInt("PID_i", 0);
    Kd = preferences.getUInt("PID_d", 0);
    Serial.println("Settings - get");
}

void settings_save() {
    preferences.putUInt("intervDisp",refresh_interval_display);
    preferences.putUInt("intervMes",refresh_interval_measure);
    preferences.putUInt("intervMot",refresh_interval_motor);
    preferences.putUInt("forceSet",forceSet);
    preferences.putUInt("tareVal",tareVal);
    preferences.putUInt("tolerance",tolerance);
    preferences.putUInt("PID_p", Kp);
    preferences.putUInt("PID_i", Ki);
    preferences.putUInt("PID_d", Kd);
    Serial.println("Settings - save");
}

void settings_default(){
    refresh_interval_display = 500;
    refresh_interval_measure = 100;
    refresh_interval_motor = 100;
    Kp = 100;
    Ki = 5;
    Kd = 5;
    forceSet = 20;
    tareVal = 0;
    tolerance = 5;
    Serial.println("Settings - default");
    settings_save();
    menu.hide();
    lcd.clear();
    lcd.setCursor(3, 1);
    lcd.print("Resetovano!");
    delay(1000);
    lcd.setCursor(3, 1);
    lcd.print("Restartuji...");
    delay(1000);
    ESP.restart();
}