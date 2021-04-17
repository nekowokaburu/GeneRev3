// ________________________________________________________________________
// Actual code for roasting process
//_________________________________________________________________________

// Roast using different roast modes which affact display and I/O regarding SD card
//    roastMode == 0 --> Free roast, save time and temperature values to SD if requested
//    roastMode == 1 --> Replicate free roast, read user selected free roast data from sd and try to set these time and temp. values
//    roastMode == 2 --> Use Marker mode from SD loaded and user selected data
bool roast(int roastMode) {
  double roastTime = (millis() - startTime) / 1000;

  if (LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();
    return true;
  }

  // Switch according to menue selected roasting mode

  if (roastMode == 0) {
    Setpoint = roastParam[2];
    changeParamDuringRoast();
    //Display current roasting process for individual roasting mode
    dispRoast(roastTime, roastParam[1], Input, roastParam[2], roastMode);

    // Write roasting data do SD if specified
    //Serial.println(floor(roastTime));
    if (saveRoastToSD && floor(roastTime) > roastParam[0]) {
      writeSDfreeRoast(int(roastTime), Setpoint);
      roastParam[0] = roastTime;
    }
    if (roastTime > roastParam[1]) {
      return true; // Times up
    }
  } else if (roastMode == 1) {
    // Recreating a logged free roast.
    //    roastParam[0] = end time of log data
    //    roastParam[1] = user changable time which shifts the original logged time data
    //    roastParam[2] = user changable temp. which shifts the original logged temp. data

    changeParamDuringRoastRep();
    //Serial.print("roastTime: ");Serial.print(roastTime);//Serial.print(" Main_Array[0]+roastParam[1]: ");Serial.println(Main_Array[0]+roastParam[1]);
    //Serial.print("Main_Array[0] = ");Serial.print(Main_Array[0]);  Serial.print(" Main_Array[1] = ");Serial.print(Main_Array[1]);
    //Serial.print(" roastParam[1] = ");Serial.print(roastParam[1]);Serial.print(" roastParam[2] = ");Serial.println(roastParam[2]);
    if (roastTime >= Main_Array[0] + roastParam[1]) {
      lr++;
      dispFileSelect(1, 1, lr);
      Setpoint = Main_Array[1] + roastParam[2];
    }

    //Display current roasting process for individual roasting mode. Ent time saved to roastParam[0] for changability of other values
    dispRoast(roastTime, roastParam[0] + roastParam[1], Input, Main_Array[1] + roastParam[2], roastMode);

    //Serial.print("roastTime: ");Serial.print(roastTime);Serial.print("  roastParam[0]+dings: ");Serial.print(roastParam[0]+roastParam[1]);
    if (roastTime >= roastParam[0] + roastParam[1]) {
      return true; // Times up (roastTime >= logged end time + user changed additional time)
    }
  }


  else if (roastMode == 2) { // Marker roast
    //Serial.print("Marker[lr][0]: ");Serial.print(Marker[lr][0]);Serial.print("Marker[lr][1]: ");Serial.println(Marker[lr][1]);
    //Serial.print("roastTime: ");Serial.print(roastTime);Serial.print("lr: ");Serial.println(lr);

    changeMarkerDuringRoast();

    if (roastTime >= Marker[lr][0]) {
      lr++; //Serial.println("LR SHOULD CHANGE! MUST BE OVERWRITTEN!");
    }
    // Compute curve between markers for different heating along fixpoints using: (dcurrTemp/dt)^slopePot * dT + previous_temp.
    // Where slopePot changes the shape of the heating slope. 1 = linear 0.1 to 2 gives a user defined slope.
    if (lr == 0) {
      Setpoint = pow((roastTime / Marker[lr][0]), slopePot) * Marker[lr][1];
    } else { // Compute slope between two points:
      Setpoint = pow((roastTime - Marker[lr - 1][0]) / (Marker[lr][0] - Marker[lr - 1][0]), slopePot) * (Marker[lr][1] - Marker[lr - 1][1]) + Marker[lr - 1][1];
    }
    //Serial.print("Setpoint: ");Serial.print(Setpoint);
    //Serial.print("lr after Setpoint computation: ");Serial.println(lr);

    //Display current roasting process for individual roasting mode. Ent time saved to roastParam[0] for changability of other values
    if (Marker[lr + 1][0] == 0 || lr == 9 || Marker[lr+1][0]<Marker[lr][0]) {
      dispRoast(roastTime,  Marker[lr][0], Input, Marker[lr][1], roastMode);
    } else {
    dispRoast(roastTime, Main_Array[1], Input, Marker[lr][1], roastMode);
    }

    if (Marker[lr][0] == 0 || lr == 10) {
      return true;
    }
  }

  run_PID();
  return false;
}









// ________________________________________________________________________
// General purpose functions
//_________________________________________________________________________
// Wait for user specified time in seconds
void wait(unsigned int delayTime) {
  unsigned long startTime = millis();
  while ((millis() - startTime) / 1000 < delayTime) {
  }
}

// Compute temperature with PID
void run_PID(void) {
  // Activate drum and fan
  analogWrite(Fan_PIN, Fan_Speed);
  analogWrite(Drum_PIN, Drum_Speed);

  //Determine PID response based on current temp
  Input = thermocouple.readCelsius();
  if (isnan(Input) == 1) {
    Input = Setpoint + shiftSP; // Removes reading errors of sensor
  }
  Setpoint = Setpoint + shiftSP;  // Change setpoint to reach desired temperature without changing PID parameters
  myPID.Compute();
  Setpoint = Setpoint - shiftSP;

  unsigned long now = millis();
  if (now - windowStartTime > Wi) {
    windowStartTime += Wi;
  }
  if (Output > now - windowStartTime) {
    digitalWrite(SSR_PIN, HIGH);
  } else {
    digitalWrite(SSR_PIN, LOW);
  }
}

// Enable user to change time and temp. parameters in real time
void changeParamDuringRoast(void) {
  // Change end time on the fly. Up Down is time, blue button
  if (LCDML_BUTTON_checkUp()) {
    if (roastParam[1] > 0) {
      roastParam[1]--;
    } LCDML_BUTTON_resetUp();
  }
  if (LCDML_BUTTON_checkDown()) {
    if (roastParam[1] < maxTime) {
      roastParam[1]++;
    } LCDML_BUTTON_resetDown();
  }

  // Change temperature on the fly
  if (LCDML_BUTTON_checkRight()) {
    if (roastParam[2] < maxTemp) {
      roastParam[2]++;
    } LCDML_BUTTON_resetRight();
  }
  if (LCDML_BUTTON_checkLeft()) {
    if (roastParam[2] > 0) {
      roastParam[2]--;
    } LCDML_BUTTON_resetLeft();
  }
}

// Enable user to change time and temp. parameters in real time
void changeParamDuringRoastRep(void) {
  // Change end time on the fly. Up Down is time, blue button
  if (LCDML_BUTTON_checkUp()) {
    roastParam[1]--;
    LCDML_BUTTON_resetUp();
  }
  if (LCDML_BUTTON_checkDown()) {
    roastParam[1]++;
    LCDML_BUTTON_resetDown();
  }

  // Change temperature on the fly
  if (LCDML_BUTTON_checkRight()) {
    roastParam[2]++;
    LCDML_BUTTON_resetRight();
  }
  if (LCDML_BUTTON_checkLeft()) {
    roastParam[2]--;
    LCDML_BUTTON_resetLeft();
  }
}

// Enable user to change time and temp. parameters in real time Marker Mode
void changeMarkerDuringRoast(void) {
  // Change end time on the fly. Up Down is time, blue button
  if (LCDML_BUTTON_checkUp()) {
    if (Marker[lr][0] > 0) {
      Marker[lr][0]--;
    } LCDML_BUTTON_resetUp();
  }
  if (LCDML_BUTTON_checkDown()) {
    if (Marker[lr][0] < maxTime) {
      Marker[lr][0]++;
    } LCDML_BUTTON_resetDown();
  }

  // Change temperature on the fly
  if (LCDML_BUTTON_checkRight()) {
    if (Marker[lr][1] < maxTemp) {
      Marker[lr][1]++;
    } LCDML_BUTTON_resetRight();
  }
  if (LCDML_BUTTON_checkLeft()) {
    if (Marker[lr][1] > 0) {
      Marker[lr][1]--;
    } LCDML_BUTTON_resetLeft();
  }
}



// Preheat roster to user defined temperature
bool preheat() {
  // Start preheating till user presses enter
  if (LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();
    parkRoaster();
    return true;
  }
  // Actually heat drum here
  dispPreheat();  // Display and temperature change
  Setpoint = roastParam[0];
  run_PID();
  return false;
}



// Cool roast until user specified end temperature is reached if selective == 1 check rmBeansBeforeCooldown
bool coolDown(int coolOnly) {
  digitalWrite(SSR_PIN, LOW);  // Turn OFF relay

  double prevTemp = 101;
  double CT = thermocouple.readCelsius();

  // Serial.print("CT: ");Serial.print(CT);Serial.print("  Temp_Cool: ");Serial.print(Temp_Cool);
  // Serial.print("  coolOnly: ");Serial.print(coolOnly);Serial.print("  RMbeans: ");Serial.println(rmBeansBeforeCooldown);

  // Let user remove beans if specified
  if (rmBeansBeforeCooldown && coolOnly == 0) {
    // Serial.print("coolStage: ");Serial.println(coolStage);
    switch (coolStage) {
      case 0: if (parkRoaster()) {
          coolStage++;
        } break;
      case 1: if (waitForUser()) {
          coolStage++;
        } break;
    }
  } else {
    coolStage = 2;
  }

  if (coolStage == 2) {
    analogWrite(Fan_PIN, Fan_Speed);
    analogWrite(Drum_PIN, Drum_Speed);
    if (LCDML_BUTTON_checkLeft()) {
      if (Temp_Cool > 30) {
        Temp_Cool--;
      } LCDML_BUTTON_resetLeft();
    }
    if (LCDML_BUTTON_checkRight()) {
      if (Temp_Cool < minCool) {
        Temp_Cool++;
      } LCDML_BUTTON_resetRight();
    }
    if (LCDML_BUTTON_checkEnter()) {
      LCDML_BUTTON_resetEnter();
      parkRoaster();
      return true;
    }

    CT = thermocouple.readCelsius();
    if (isnan(CT) == 1) {
      CT = prevTemp;
    } else { // Removes reading errors of sensor
      prevTemp = CT;
    }
    dispCooldown(startTime, millis(), ceil(CT));
    //Serial.print("Temp: ");Serial.println(CT);
  }
  if (CT <= Temp_Cool) {
    parkRoaster();
    return true;
  } else {
    return false;
  }
}

/*
  // Smooth temperature signal for cooling and error which should be more precise.
  double Smooth(void){
  int readings[3] = {0, 0, 0};    // the readings from the analog input
  int average = 0;                // the average
  double total = 0;
  int i = 0;

  for (i = 0; i < 3; i++) {
    // read from the sensor:
      total = total + thermocouple.readCelsius();;
  }
  // calculate the average:
  average = total / 3;
  return average;
  }
*/

// Set roaster to correct parking position
bool parkRoaster(void) {
  dispTxt("Returning drum");
  digitalWrite(SSR_PIN, LOW);
  analogWrite(Fan_PIN, Fan_Speed);
  while (digitalRead(rotEndSw) == 1) {
    analogWrite(Drum_PIN, 80);
  }
  analogWrite(Fan_PIN, 0);
  analogWrite(Drum_PIN, 0);
  if (digitalRead(rotEndSw) == 0) {
    return true;
  } else {
    return false;
  }
}

// Wait for user input to continue
bool waitForUser(void) {
  // Wait for user input when beans are inserted into roaster
  dispTxt("Select to continue.");
  //Serial.println("waitForUser - Waiting for user input.");
  if (LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();
    return true;
  } else {
    return false;
  }
}


// Select name for FreeRoast save file
bool enterFreeRoastFileName(void) {
  if (lr < 5) { // naming until all filled in
    if (LCDML_BUTTON_checkUp()) {
      if (pos > 0) {
        pos--; // Change letters or numbers in rotation
      } else {
        pos = 36;
      } LCDML_BUTTON_resetUp();
    }
    if (LCDML_BUTTON_checkDown()) {
      if (pos < 36) {
        pos++;
      } else {
        pos = 0;
      } LCDML_BUTTON_resetDown();
    }
    if (LCDML_BUTTON_checkEnter()) {
      fileName[lr] = alphabetNr[pos];  // Write letter to file name
      if (lr < 5) {
        lr++;
      } LCDML_BUTTON_resetEnter();
    }
    if (LCDML_BUTTON_checkLeft()) {
      //Serial.println("lr should change");  // Change selected time or temp. value with blue turn
      if (lr > 0) {
        lr--;
       // Serial.println("lr changes");
      } LCDML_BUTTON_resetLeft();
    }
    if (LCDML_BUTTON_checkRight()) {
      if (lr < 5) {
        lr++;
      } LCDML_BUTTON_resetRight();
    }
    dispProfileNaming();
  } else if (lr == 5) { // last letter entered, ask to save
    if (LCDML_BUTTON_checkLeft()) {
      lr = 4;  // Down allowed to exit from save
      LCDML_BUTTON_resetLeft();
    }
    if (LCDML_BUTTON_checkEnter()) {
      LCDML_BUTTON_resetAll(); pos = 0; lr = 99;
      //Serial.println(String(fileName) + ".csv");
      // Check if file already exists
      //Serial.print("Checking SD for duplicates... filecheck= ");
      fileExist = SD.exists(String(fileName) + ".csv");
    }
  }
  if (lr == 99) {
    //Serial.print(fileExist);
    if (fileExist) {
      //Serial.print("...requesting user input...");
      if (dispFileExist()) {
        //Serial.print("user decided user="); Serial.print(SDoverride);
        if (SDoverride) {
          //Serial.print("...deleting file: "); Serial.println(String(fileName) + ".csv");
          SD.remove(String(fileName) + ".csv"); myFile = SD.open(String(fileName) + ".csv", FILE_WRITE); myFile.close();
          dispTxt("File replaced!"); wait(1); pos = 0; lr = 0; return true;
        }
        if (!SDoverride) {
          dispTxt("Change name."); wait(1); pos = 0; lr = 0; stage--; return true;
        }
      }
    }
    if (!fileExist) {
      //Serial.println("Creating new file on SD card.");
      myFile = SD.open(String(fileName) + ".csv", FILE_WRITE); myFile.close();
      myFile = SD.open("This", FILE_WRITE); myFile.close();
      pos = 0; lr = 0;
      dispTxt("File created!"); wait(1);
      return true;
    }
  }
  return false;
}

double maxValue(double Array[10][2]) {
  int max_v = 0;

  for ( int i = 0; i < 10; i++ )
  {
    if ( Array[i][0] > max_v )
    {
      max_v = Array[i][0];
    }
  }
  return max_v;
}

bool crackDetect(void) {
int counter = 0;
  
  // Add new temp value and delete oldest
  crackTempSetp[1][0] = crackTempSetp[0][0];
  crackTempSetp[2][0] = crackTempSetp[1][0];
  crackTempSetp[3][0] = crackTempSetp[2][0];
  crackTempSetp[4][0] = crackTempSetp[3][0];
  crackTempSetp[0][0] = Input;

  // Get Setpoint values to check for user input
  crackTempSetp[1][1] = crackTempSetp[0][1];
  crackTempSetp[2][1] = crackTempSetp[1][1];
  crackTempSetp[3][1] = crackTempSetp[2][1];
  crackTempSetp[4][1] = crackTempSetp[3][1];
  crackTempSetp[0][1] = Setpoint;

  // Compute temperature differences
  for(int i = 1;i<5;i++){
    if(crackTempSetp[0][0]-crackTempSetp[i][0]<-0.4){counter++;}
  }

  // Check if temp is falling in both cases
  // Measured temp. is lower than before in two test cases and
  // Set temp. is equal or rising, then a crack should be going on.
  if (counter < 2 &&
  crackTempSetp[0][1] - crackTempSetp[1][1] <= 0 && crackTempSetp[1][1] - crackTempSetp[2][1] <= 0
   && crackTempSetp[2][1] - crackTempSetp[3][1] <= 0 && crackTempSetp[3][1] - crackTempSetp[4][1] <= 0) {
    return true;
  } else {
    return false;
  }
}

