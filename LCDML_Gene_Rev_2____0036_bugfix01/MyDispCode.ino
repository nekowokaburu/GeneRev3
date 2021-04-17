// Drawing code and strings for short display functions
void dispTxt(const char* Name) {
  // picture loop
  u8g.firstPage();
  do {
    u8g.drawStr(0, 35, Name);
  } while ( u8g.nextPage() );
}

void dispTxtVal(const char* Name, float value, int decimals, const char* kind) {
  // picture loop
  u8g.firstPage();
  do {
    u8g.drawStr(10, 15, Name);
    dtostrf(value, 3, decimals, buf);
    u8g.drawStr(45, 40, buf);
    u8g.drawStr(70, 40, kind);
  } while ( u8g.nextPage() );
}

// display PID parameter selection (variables are global)
void dispSetPID(void) {
  // picture loop
  u8g.firstPage();
  do {
    // Draw marker for current position
    u8g.drawStr(5, 11 + pos * 15, "*");

    // Draw text and values
    u8g.drawStr(0, 10, "Set PID variables:");
    u8g.drawStr(15, 25, "Kp: "); dtostrf(Kp, 3, 0, buf); u8g.drawStr(40, 25, buf);
    u8g.drawStr(15, 40, "Ki: "); dtostrf(Ki, 3, 0, buf); u8g.drawStr(40, 40, buf);
    u8g.drawStr(15, 55, "Kd: "); dtostrf(Kd, 3, 0, buf); u8g.drawStr(40, 55, buf);

  } while ( u8g.nextPage() );
}


// Create Profile display
void dispCreateProfile(void) {
  // picture loop
  u8g.firstPage();
  do {
    u8g.drawStr(0, 10, "Create roast profile");

    if (pos == 10) {
      // Add save button to bottom
      u8g.drawStr(10, 40, "Save and continue?");
    } else {
      u8g.drawStr(0, 30, "Location "); dtostrf(pos + 1, 2, 0, buf); u8g.drawStr(53, 30, buf); u8g.drawStr(65, 30, " of 10:");
      // pos time: 13:40 temp.: 200 °C
      u8g.drawStr(10, 45, "time:"); dtostrf(floor(Marker[pos][0] / 60), 2, 0, buf); if (floor(Marker[pos][0] / 60) < 10) {
        u8g.drawStr(48, 45, "0");
      } u8g.drawStr(48, 45, buf);
      u8g.drawStr(60, 44, ":"); dtostrf(Marker[pos][0] - floor(Marker[pos][0] / 60) * 60, 2, 0, buf); if (Marker[pos][0] - floor(Marker[pos][0] / 60) * 60 < 10) {
        u8g.drawStr(66, 45, "0");
      } u8g.drawStr(66, 45, buf);
      u8g.drawStr(10, 60, "temp.:"); dtostrf(Marker[pos][1], 2, 0, buf); u8g.drawStr(50, 60, buf); u8g.drawStr(70, 60, "\260"); u8g.drawStr(75, 60, "C");
      // Mark current settings
      if (lr == 0) {
        u8g.drawStr(0, 45, "\273");
      } else {
        u8g.drawStr(0, 60, "\273");
      }
    }
  } while ( u8g.nextPage() );
}

void dispProfileNaming(void) {
  // picture loop
  u8g.firstPage();
  do {
    u8g.drawStr(0, 10, "Create roast profile");

    if (lr == 5) { // Ask if save ok.
      u8g.drawStr(0, 40, "Save as "); u8g.setPrintPos(45, 40); u8g.print(String(fileName)); u8g.drawStr(78, 40, "?");
    } else {
      // pos time: 13:40 temp.: 200 °C
      u8g.drawStr(0, 30, "Letter: "); u8g.setPrintPos(45, 30); u8g.print(alphabetNr[pos]);
      u8g.drawStr(0, 45, "Name:"); u8g.setPrintPos(35, 45); u8g.print(String(fileName));
      u8g.drawStr(35 + 6 * lr, 46, "_");
    }
  } while ( u8g.nextPage() );
}

// Displays message if file already exists and asks to override
bool dispFileExist(void) {
  if (LCDML_BUTTON_checkUp()) {
    if (!SDoverride) {
      SDoverride = !SDoverride; // Change yes and no
    } LCDML_BUTTON_resetUp();
  }
  if (LCDML_BUTTON_checkDown()) {
    if (SDoverride) {
      SDoverride = !SDoverride;
    } LCDML_BUTTON_resetDown();
  }
  if (LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();  // Accept answer
    return true;
  }

  // picture loop
  u8g.firstPage();
  do {
    u8g.drawStr(0, 10, "Filename already"); u8g.drawStr(0, 25, "exists. Do you want"); u8g.drawStr(0, 40, "to override?");
    u8g.drawStr(30, 60, "YES"); u8g.drawStr(98, 60, "NO");
    if (SDoverride) {
      u8g.drawFrame(28, 49, 21, 13);
    } else {
      u8g.drawFrame(96, 49, 15, 13);
    }
  } while ( u8g.nextPage() );
  return false;
}


// Opens selected file type from SD and displays content
bool dispFileSelect(int ftype, int selORread, int line) {
  File entry;
  myFile = SD.open("/");
  // Go to user selected filename
  myFile.rewindDirectory();
  //Serial.print("DFS 1 freeMemory()=");Serial.println(freeMemory());
  if (selORread == 1) {
    for (int i = 0; i <= pos; i++) { // Serial.print("i = ");Serial.println(i);
      do {  // Go to next file of correct type
        entry = myFile.openNextFile();
        entry.close();
      } while (!isSDtype(entry.name(), ftype)); //Serial.print("Skipping files: ");Serial.println(entry.name());}
    }
    if (ftype == 0) {
      readSDroastProfile(entry.name());
    } else if (ftype == 1) {
      readSDfreeRoast(entry.name(), line);
    }
    //Serial.print("DFS 2 freeMemory()=");Serial.println(freeMemory());
  } else {
    // Set position
    if (LCDML_BUTTON_checkUp()) {
      if (pos > 0) {
        pos--; // Browse files
      } LCDML_BUTTON_resetUp();
    }
    if (LCDML_BUTTON_checkDown()) {
      if (pos < lr - 1) {
        pos++;
      } LCDML_BUTTON_resetDown();
    }
    // Get name for current position
    for (int i = 0; i <= pos; i++) { // Serial.print("i = ");Serial.println(i);
      do {  // Go to next file of correct type
        entry = myFile.openNextFile();
        entry.close();
      } while (!isSDtype(entry.name(), ftype)); //Serial.print("Skipping files: ");Serial.println(entry.name());}
    }
    //Serial.print("pos = ");Serial.println(pos);

    // Display current filename
    //Serial.print("current entry name: ");Serial.println(entry.name());
    // Draw display
    u8g.firstPage();
    do {
      u8g.drawStr(0, 10, "Load roast profile");
      u8g.drawStr(0, 45, ">"); u8g.setPrintPos(10, 45); u8g.print(entry.name());
    } while ( u8g.nextPage() );

    if (LCDML_BUTTON_checkEnter()) {
      LCDML_BUTTON_resetEnter();
      if (ftype == 0) {
        dispTxt("Loading profile...");
        readSDroastProfile(entry.name());
      } else if (ftype == 1) {
        readSDfreeRoast(entry.name(), line);
      }
      entry.close();
      myFile.close();
      //Serial.print("DFS 3 freeMemory()=");Serial.println(freeMemory());
      return true;
    }

  }
  entry.close();
  myFile.close();
  //Serial.print("DFS 4 freeMemory()=");Serial.println(freeMemory());
  return false;
}

//_______________________________________________________________________________________________________________________________
//  ROASTING DISPLAY FUNCTIONS
//_______________________________________________________________________________________________________________________________

// User select Free Roast starting parameters
// Display during preheating process
bool dispFreeRoastStartParam(void) {
  if (LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();
    return true;
  }
  if (LCDML_BUTTON_checkUp()) {
    if (roastParam[1] > 0) {
      roastParam[1]--; // Change time
    } LCDML_BUTTON_resetUp();
  }
  if (LCDML_BUTTON_checkDown()) {
    if (roastParam[1] < maxTime) {
      roastParam[1]++;
    } LCDML_BUTTON_resetDown();
  }
  if (LCDML_BUTTON_checkRight()) {
    if (roastParam[2] < maxTemp) {
      roastParam[2]++; // Change temp.
    } LCDML_BUTTON_resetRight();
  }
  if (LCDML_BUTTON_checkLeft()) {
    if (roastParam[2] > 0) {
      roastParam[2]--;
    } LCDML_BUTTON_resetLeft();
  }
  // Draw display
  u8g.firstPage();
  do {
    u8g.drawStr(0, 10, "Sel. starting values:");
    u8g.drawStr(0, 30, "Start temp.:"); dtostrf(roastParam[2], 2, 0, buf); u8g.drawStr(80, 30, buf); u8g.drawStr(99, 30, "\260"); u8g.drawStr(104, 30, "C");
    u8g.drawStr(0, 45, "Est. durat.:"); dtostrf(floor(roastParam[1] / 60), 2, 0, buf); if (floor(roastParam[1] / 60) < 10) {
      u8g.drawStr(80, 45, "0");
    } u8g.drawStr(80, 45, buf);
    u8g.drawStr(91, 44, ":"); dtostrf(roastParam[1] - floor(roastParam[1] / 60) * 60, 2, 0, buf); if (roastParam[1] - floor(roastParam[1] / 60) * 60 < 10) {
      u8g.drawStr(96, 45, "0");
    } u8g.drawStr(96, 45, buf);
    u8g.drawStr(0, 63, "Select to continue.");
  } while ( u8g.nextPage() );
  return false;
}



// Ask if user wants to remove beans before cooldown
bool dispRMbeansBeforeCooldown(void) {
  if (LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();
    return true;
  }
  if (LCDML_BUTTON_checkUp()) {
    if (!rmBeansBeforeCooldown) {
      rmBeansBeforeCooldown = !rmBeansBeforeCooldown; // Change yes and no
    } LCDML_BUTTON_resetUp();
  }
  if (LCDML_BUTTON_checkDown()) {
    if (rmBeansBeforeCooldown) {
      rmBeansBeforeCooldown = !rmBeansBeforeCooldown;
    } LCDML_BUTTON_resetDown();
  }

  // picture loop
  u8g.firstPage();
  do {
    u8g.drawStr(0, 10, "Remove beans"); u8g.drawStr(0, 25, "before cooldown?");
    u8g.drawStr(30, 60, "YES"); u8g.drawStr(98, 60, "NO");
    if (rmBeansBeforeCooldown) {
      u8g.drawFrame(28, 49, 21, 13);
    } else {
      u8g.drawFrame(96, 49, 15, 13);
    }
  } while ( u8g.nextPage() );
  return false;
}

// Ask if user wants to save roast to SD card
bool dispSaveRoast(void) {
  if (LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();
    return true;
  }
  if (LCDML_BUTTON_checkUp()) {
    if (!saveRoastToSD) {
      saveRoastToSD = !saveRoastToSD; // Change yes and no
    } LCDML_BUTTON_resetUp();
  }
  if (LCDML_BUTTON_checkDown()) {
    if (saveRoastToSD) {
      saveRoastToSD = !saveRoastToSD;
    } LCDML_BUTTON_resetDown();
  }

  // picture loop
  u8g.firstPage();
  do {
    u8g.drawStr(0, 10, "Save roast"); u8g.drawStr(0, 25, "to SD card?");
    u8g.drawStr(30, 60, "YES"); u8g.drawStr(98, 60, "NO");
    if (saveRoastToSD) {
      u8g.drawFrame(28, 49, 21, 13);
    } else {
      u8g.drawFrame(96, 49, 15, 13);
    }
  } while ( u8g.nextPage() );
  return false;
}


// Display Preheat selection menue
bool dispPreheatSel(void) {
  if (LCDML_BUTTON_checkLeft()) {
    if (roastParam[0] > 0) {
      roastParam[0]--; // Change preheat temp.
    } LCDML_BUTTON_resetLeft();
  }
  if (LCDML_BUTTON_checkRight()) {
    if (roastParam[0] < maxTemp) {
      roastParam[0]++;
    } LCDML_BUTTON_resetRight();
  }
  if (LCDML_BUTTON_checkEnter()) {
    LCDML_BUTTON_resetEnter();
    return true;
  }
  // Draw display
  u8g.firstPage();
  do {
    u8g.drawStr(0, 10, "Select preheating");u8g.drawStr(0, 23, "temperature");
    u8g.drawStr(0, 45, ">"); dtostrf(roastParam[0], 2, 0, buf); u8g.drawStr(10, 45, buf); u8g.drawStr(35, 45, "\260"); u8g.drawStr(40, 45, "C");
    u8g.drawStr(0, 63, "Select to continue.");
  } while ( u8g.nextPage() );
  return false;
}

// Display during preheating process
void dispPreheat(void) {
  if (LCDML_BUTTON_checkLeft()) {
    if (roastParam[0] > 0) {
      roastParam[0]--; // Browse files
    } LCDML_BUTTON_resetLeft();
  }
  if (LCDML_BUTTON_checkRight()) {
    if (roastParam[0] < maxTemp) {
      roastParam[0]++;
    } LCDML_BUTTON_resetRight();
  }
  // Draw display
  u8g.firstPage();
  do {
    u8g.drawStr(0, 10, "Preheating...");
    u8g.drawStr(0, 30, "Current temp.:"); dtostrf(Input, 2, 0, buf); u8g.drawStr(92, 30, buf); u8g.drawStr(117, 30, "\260"); u8g.drawStr(122, 30, "C");
    u8g.drawStr(0, 45, "Sel. temp.:"); dtostrf(roastParam[0], 2, 0, buf); u8g.drawStr(92, 45, buf); u8g.drawStr(117, 45, "\260"); u8g.drawStr(122, 45, "C");
    u8g.drawStr(0, 63, "Select to continue.");
  } while ( u8g.nextPage() );

}

// Display cooldown data
void dispCooldown(uint32_t startDisp, uint32_t nowDisp, double tempDisp) {
  // Draw display
  u8g.firstPage();
  do {
    u8g.drawStr(0, 10, "Cooling roast to"); dtostrf(Temp_Cool, 2, 0, buf); u8g.drawStr(0, 25, buf); u8g.drawStr(24, 25, "\260"); u8g.drawStr(32, 25, "C");
    // time: 13:40 temp.: 200 °C
    u8g.drawStr(10, 45, "time:"); dtostrf(floor((nowDisp - startDisp) / 60000), 2, 0, buf); if (floor((nowDisp - startDisp) / 60000) < 10) {
      u8g.drawStr(48, 45, "0");
    } u8g.drawStr(48, 45, buf);
    u8g.drawStr(60, 44, ":"); dtostrf((nowDisp - startDisp) / 1000 - floor((nowDisp - startDisp) / 60000) * 60, 2, 0, buf); if ((nowDisp - startDisp) / 1000 - floor((nowDisp - startDisp) / 60000) * 60 < 10) {
      u8g.drawStr(66, 45, "0");
    } u8g.drawStr(66, 45, buf); u8g.drawStr(80, 45, "min");
    u8g.drawStr(10, 60, "temp.:"); dtostrf(tempDisp, 2, 0, buf); u8g.drawStr(50, 60, buf); u8g.drawStr(80, 60, "\260"); u8g.drawStr(85, 60, "C");
  } while ( u8g.nextPage() );
}





// Display during roasting phase
void dispRoast(double currTime, double endTime, double currTemp, double setTemp, int mode) {
  //Serial.print("currTime: ");Serial.print(int(floor(currTime/60)));Serial.print(":");Serial.println(int(currTime)-int(floor(currTime/60))*60);
  //Serial.print("endTime: ");Serial.print(int(floor(endTime/60)));Serial.print(":");Serial.println(int(endTime)-int(floor(endTime/60))*60);
  // picture loop
  u8g.firstPage();
  do {
    // Graph
    u8g.drawFrame(0, 0, 114, 64);
    u8g.drawHLine(71, 2, 45); // 240 °C advance line for better reference
    u8g.drawHLine(54, 17, 62); // 220 °C advance line for better reference
    u8g.drawHLine(20, 32, 96); // 200 °C advance line for better reference
    u8g.drawHLine(5, 47, 111); // 180 °C
    u8g.drawHLine(114, 62, 2); // 160 °C

    u8g.setFont(u8g_font_4x6);   // Same as standard from LCDML
    u8g.drawStr(117, 7, "240");
    u8g.drawStr(117, 21, "220");
    u8g.drawStr(117, 36, "200");
    u8g.drawStr(117, 51, "180");
    u8g.drawStr(117, 64, "160");

    // Time display
    u8g.setFont(u8g_font_5x8);   // Same as standard from LCDML
    dtostrf(int(floor(currTime / 60)), 2, 0, buf); if (int(floor(currTime / 60)) < 10) {
      u8g.drawStr(2, 9, "0");
    } u8g.drawStr(2, 9, buf); u8g.drawStr(12, 9, ":"); dtostrf(int(currTime) - int(floor(currTime / 60)) * 60, 2, 0, buf); if (int(currTime) - int(floor(currTime / 60)) * 60 < 10) {
      u8g.drawStr(17, 9, "0");
    } u8g.drawStr(17, 9, buf);
    u8g.drawStr(28, 9, "\273");
    dtostrf(int(floor(endTime / 60)), 2, 0, buf); if (int(floor(endTime / 60)) < 10) {
      u8g.drawStr(32, 9, "0");
    } u8g.drawStr(32, 9, buf); u8g.drawStr(42, 9, ":"); dtostrf(int(endTime) - int(floor(endTime / 60)) * 60, 2, 0, buf); if (int(endTime) - int(floor(endTime / 60)) * 60 < 10) {
      u8g.drawStr(47, 9, "0");
    } u8g.drawStr(47, 9, buf);

    // Temperature display
    dtostrf(int(currTemp), 3, 0, buf); u8g.drawStr(2, 17, buf);
    u8g.drawStr(18, 17, "\273");
    dtostrf(int(setTemp), 3, 0, buf); u8g.drawStr(23, 17, buf); u8g.drawStr(38, 17, "\260"); u8g.drawStr(43, 17, "C");

    // Display user changed temperature to cancel it out later if desired
    if(mode==1){
    u8g.drawStr(2, 25, "dT:");
    dtostrf(int(roastParam[2]), 2, 0, buf); u8g.drawStr(22, 25, buf);
    u8g.drawStr(38, 25, "\260"); u8g.drawStr(43, 25, "C");
    }

    // Check if new location can be displayed due to small display
    if (currTime >= ((1 + slot)*endTime / 110) && slot < 110) {
      if(currTemp>160){rememberProfile[slot] = 64 - int(((currTemp-160) * 0.75 + 2));}
      else {
        rememberProfile[slot] = 64;
      }
      slot++;
    }


    if (mode == 2) { // mode = 0 == FreeRoast mode, no marker needed
      // Draw up to 10 markers for interesting temperatures/preset temperatures
      int X = 0; int Y = 0; int offsetX = 9; int offsetY = 9;
      u8g.setFont(u8g_font_4x6);
      for (int i = 0; i < 10; i++) {
        if (Marker[i][1] > 0) {
          X = int(Marker[i][0] / (endTime / 110));
          Y = 64 - int((Marker[i][1]-160) *0.75 +2);

          // Set backgrond to clear for better readability if lines cross
          u8g.setColorIndex(0);
          u8g.drawBox(X - offsetX - 1, Y + offsetY - 7, 20, 8);
          u8g.setColorIndex(1);

          // Draw marker locations and timestamps
          u8g.drawCircle(X, Y, 2); dtostrf(int(Marker[i][0] / 60), 2, 0, buf); u8g.drawStr(X - offsetX, Y + offsetY, buf); if (int(Marker[i][0] / 60) < 10) {
            u8g.drawStr(X - offsetX, Y + offsetY, "0");
          }
          u8g.drawStr(X - offsetX + 7, Y + offsetY, ":");
          dtostrf(int(Marker[i][0]) - int(Marker[i][0] / 60) * 60, 2, 0, buf); if (int(Marker[i][0]) - int(Marker[i][0] / 60) * 60 < 10) {
            u8g.drawStr(X - offsetX + 11, Y + offsetY, "0");
          } u8g.drawStr(X - offsetX + 11, Y + offsetY, buf);
        }
      }
    }

    // Fill all measured locations e.g. Mark current temperature
    bool check = false;
    for (int i = 0; i < 110; i++) {
      if(mode != 2){
        u8g.drawPixel(1 + i, rememberProfile[i]);
        if(mode==0 && rememberProfile[i]==0 && check == false){
          u8g.drawCircle(1+i, 64-int((roastParam[2]-160)*0.75+2), 2);
          check = true;
        }
      } else {    // Also scale by last Marker in Marker mode
        if (Marker[lr + 1][0] == 0 || lr == 9) {  // Last marker reached
        u8g.drawPixel(1 + i + round(Main_Array[1]-Marker[lr][0])/110, rememberProfile[i]);
        }else {
          u8g.drawPixel(1 + i, rememberProfile[i]);
          }
      }
    }

    // If a crack can be detected, mark it as vertical line
    // Roast must be running for at least 8 min
    if (crackDetect() == true && currTime > 480) {
      u8g.drawVLine(int(currTime / (endTime / 110)), 0, 64);
    }

    // Set font to original
    u8g.setFont(u8g_font_6x13);   // Same sas standard from LCDML
  } while ( u8g.nextPage() );
}
















