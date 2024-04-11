void measure() {
  // Get the value from scale tensometer
  forceRaw = scale.get_units();
  forceMes = forceRaw - 0.5; // Corrected the typo
  forceMes = roundf(forceMes * 10) / 10;

  //Get value from endstop sensor
  endstop = !digitalRead(ENDSTOP);

  // Display the instant measured value
  // Serial.print("Instant measured value: ");
  // Serial.println(forceMes, 1);

  // Display the average value
  //Serial.println("Average value: " + String(scale.get_units(10)));

  // Set the PID setpoint to forceSet
  Setpoint = forceSet - tareVal;

  // Set the PID input to forceMes
  Input = forceMes;

  // // PID calculation
  // myPID.Compute();

}