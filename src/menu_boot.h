void menu_boot() {
    menu[2]->setProgress(forceSet);

    menu[3]->getSubMenu()[2]->setProgress(Kp);
    menu[3]->getSubMenu()[3]->setProgress(Ki);
    menu[3]->getSubMenu()[4]->setProgress(Kd);
    
    menu[5]->setProgress(tolerance);

    menu[6]->getSubMenu()[2]->setProgress(refresh_interval_display);
    menu[6]->getSubMenu()[3]->setProgress(refresh_interval_measure);
    menu[6]->getSubMenu()[4]->setProgress(refresh_interval_motor);

    menu.update();
}