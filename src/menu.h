// CALLBACK FUNCTIONS
void intCallback(uint16_t value) {
    Serial.print("Callback hodnota: ");
    Serial.println(value);
}
void callbackDisp(uint16_t value) {
    refresh_interval_display = value;
    settings_save();
}
void callbackMes(uint16_t value) {
    refresh_interval_measure = value;
    settings_save();
}
void callbackMot(uint16_t value) {
    refresh_interval_motor = value;
    settings_save();
}
void callbackFset(uint16_t value) {
    forceSet = value;
    settings_save();
}
void callbackP(uint16_t value) {
    Kp = value;
    settings_save();
}
void callbackI(uint16_t value) {
    Ki = value;
    settings_save();
}
void callbackD(uint16_t value) {
    Kd = value;
    settings_save();
}
void callbackTolerance(uint16_t value) {
    tolerance = value;
    settings_save();
}

// MAPPING FUNCTIONS

char* intMapping(uint16_t progress) {
    // Map the progress value to a new range (100 to 200)
    long mapped = mapProgress(progress, 0L, 1000L);

    // Buffer to store the converted string
    static char buffer[10];

    // // Convert the mapped value to a string
    itoa(mapped, buffer, 10);

    return buffer;
}

char* FsetMapping(uint16_t progress) {
    long mapped = mapProgress(progress, 0L, 1000L);
    static char buffer[10];
    itoa(mapped, buffer,10);
    concat(buffer,"N", buffer);
    return buffer;
}

char* mappingInterval(uint16_t progress) {
    long mapped = mapProgress(progress, 1L, 1000L);
    static char buffer[10];
    itoa(mapped, buffer,10);
    concat(buffer,"ms",buffer);
    return buffer;
}

extern MenuItem* menuSystem[];
extern MenuItem* menuPID[];
extern MenuItem* menuFactory[];

// Initialize the main menu items
MAIN_MENU(
    ITEM_COMMAND("< Opustit menu",menuExit),
    ITEM_PROGRESS("Fset",1,FsetMapping,callbackFset),
    ITEM_SUBMENU("PID regulace",menuPID),
    ITEM_COMMAND("Tare",tare),
    ITEM_PROGRESS("Toler.",1,intMapping,callbackTolerance),
    ITEM_SUBMENU("Dalsi nastaveni",menuSystem),
    ITEM_SUBMENU("Tovarni nastaveni",menuFactory)
);
SUB_MENU(menuSystem, mainMenu,
    ITEM_COMMAND("< Zpet", menuBack),
    ITEM_PROGRESS("Obn. LCD",10,mappingInterval,callbackDisp),
    ITEM_PROGRESS("Vzork.",10,mappingInterval,callbackMes),
    ITEM_PROGRESS("Rizeni mot.",10,mappingInterval,callbackMot)

);
SUB_MENU(menuPID, mainMenu,
    ITEM_COMMAND("< Zpet", menuBack),
    ITEM_PROGRESS("P",1,intMapping,callbackP),
    ITEM_PROGRESS("I",1,intMapping,callbackI),
    ITEM_PROGRESS("D",1,intMapping,callbackD)

);
SUB_MENU(menuFactory, mainMenu,
    ITEM_BASIC("Opravdu resetovat?"),
    ITEM_COMMAND("[Storno]", menuBack),
    ITEM_COMMAND("[Resetovat]",settings_default)
);