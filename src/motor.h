void controlMotor() {
  // // Determine motor direction based on forceMes and forceSet
  // if (abs(forceMes - forceSet) > 10) {
  //   // If the difference is greater than 10, run the motor at full speed
  //   stepsPSecond = (forceMes < forceSet) ? 1000 : -1000;
  // } else {
  //   // Apply proportional control within the dead zone
  //   stepsPSecond = map(Output, 0, 255, -1000, 1000);
  // }
  stepsPSecond = map(Output, 0, 255, -500, 500);
  

  if (current_step <= 0 && stepsPSecond < 1) {
    //hit min range limit
    stepper.setSpeed(0);
    error = true;
    error_desc = "Mimo rozsah - min";
    error_desc2 = "Robot nahoru v Z";
  }
  else if (current_step >= 1100 && stepsPSecond > 1) {
    //hit max range limit
    stepper.setSpeed(0);
    error = true;
    error_desc = "Mimo rozsah - max";
    error_desc2 = "Robot dolu v Z";
  }
  else if (forceMes >= (forceSet-tolerance) && forceMes < forceSet) {
    stepper.setSpeed(0);
    error = false;
  }
  else if (forceMes <= (forceSet+tolerance) && forceMes > forceSet) {
    stepper.setSpeed(0);
    error = false;
  }
  else {
    error = false;
    stepper.setSpeed(stepsPSecond);
  }
}