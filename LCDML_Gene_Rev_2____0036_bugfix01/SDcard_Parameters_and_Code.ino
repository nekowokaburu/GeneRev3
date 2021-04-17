// SD card parameter read and write
void readSDSettings() {
  char character;
  String settingName;
  String settingValue;
  myFile = SD.open("settings.txt");
  if (myFile) {
    while (myFile.available()) {
      character = myFile.read();
      while ((myFile.available()) && (character != '[')) {
        character = myFile.read();
      }
      character = myFile.read();
      while ((myFile.available()) && (character != '=')) {
        settingName = settingName + character;
        character = myFile.read();
      }
      character = myFile.read();
      while ((myFile.available()) && (character != ']')) {
        settingValue = settingValue + character;
        character = myFile.read();
      }
      if (character == ']') {

        /*
          //Debugging Printing
          Serial.print("Name:");
          Serial.println(settingName);
          Serial.print("Value :");
          Serial.println(settingValue);
        */

        // Apply the value to the parameter
        applySetting(settingName, settingValue);
        // Reset Strings
        settingName = "";
        settingValue = "";
      }
    }
    // close the file:
    myFile.close();
  }
}

/* Apply the value to the parameter by searching for the parameter name
  Using String.toInt(); for Integers
  toFloat(string); for Float
  toBoolean(string); for Boolean
  toLong(string); for Long
*/
void applySetting(String settingName, String settingValue) {
  if (settingName == "Kp") {
    Kp = settingValue.toInt();
  }
  if (settingName == "Ki") {
    Ki = settingValue.toInt();
  }
  if (settingName == "Kd") {
    Kd = settingValue.toInt();
  }
  if (settingName == "Temp_Cool") {
    Temp_Cool = settingValue.toInt();
  }
  if (settingName == "Drum_Speed") {
    Drum_Speed = settingValue.toInt();
  }
  if (settingName == "Fan_Speed") {
    Fan_Speed = settingValue.toInt();
  }
  if (settingName == "saveRoastToSD") {
    saveRoastToSD = toBoolean(settingValue);
  }
  if (settingName == "rmBeansBeforeCooldown") {
    rmBeansBeforeCooldown = toBoolean(settingValue);
  }

}

// converting string to Float
float toFloat(String settingValue) {
  char floatbuf[settingValue.length() + 1];
  settingValue.toCharArray(floatbuf, sizeof(floatbuf));
  float f = atof(floatbuf);
  return f;
}

long toLong(String settingValue) {
  char longbuf[settingValue.length() + 1];
  settingValue.toCharArray(longbuf, sizeof(longbuf));
  long l = atol(longbuf);
  return l;
}

// Converting String to integer and then to boolean
// 1 = true
// 0 = false
boolean toBoolean(String settingValue) {
  if (settingValue.toInt() == 1) {
    return true;
  } else {
    return false;
  }
}

// Writes A Configuration file
void writeSDSettings() {
  // Delete the old One
  SD.remove("settings.txt");
  // Create new one
  myFile = SD.open("settings.txt", FILE_WRITE);
  // writing in the file works just like regular print()/println() function

  myFile.print("[");
  myFile.print("Kp=");
  myFile.print(Kp);
  myFile.println("]");

  myFile.print("[");
  myFile.print("Ki=");
  myFile.print(Ki);
  myFile.println("]");

  myFile.print("[");
  myFile.print("Kd=");
  myFile.print(Kd);
  myFile.println("]");

  myFile.print("[");
  myFile.print("Temp_Cool=");
  myFile.print(Temp_Cool);
  myFile.println("]");

  myFile.print("[");
  myFile.print("Drum_Speed=");
  myFile.print(Drum_Speed);
  myFile.println("]");

  myFile.print("[");
  myFile.print("Fan_Speed=");
  myFile.print(Fan_Speed);
  myFile.println("]");

  myFile.print("[");
  myFile.print("saveRoastToSD=");
  myFile.print(saveRoastToSD);
  myFile.println("]");

  myFile.print("[");
  myFile.print("rmBeansBeforeCooldown=");
  myFile.print(rmBeansBeforeCooldown);
  myFile.println("]");

  // close the file:
  myFile.close();
  //Serial.println("Writing done.");
}

// Writes a custom roast profile to SD
void writeSDprofile(char* fileN) {

  // Create new one
  File datafile = SD.open(String(fileName) + ".txt", FILE_WRITE);
  // writing in the file works just like regular print()/println() function
  datafile.print("["); datafile.print("t1="); datafile.print(Marker[0][0]);  datafile.println("]");
  datafile.print("["); datafile.print("T1="); datafile.print(Marker[0][1]);  datafile.println("]");
  datafile.print("["); datafile.print("t2="); datafile.print(Marker[1][0]);  datafile.println("]");
  datafile.print("["); datafile.print("T2="); datafile.print(Marker[1][1]);  datafile.println("]");
  datafile.print("["); datafile.print("t3="); datafile.print(Marker[2][0]);  datafile.println("]");
  datafile.print("["); datafile.print("T3="); datafile.print(Marker[2][1]);  datafile.println("]");
  datafile.print("["); datafile.print("t4="); datafile.print(Marker[3][0]);  datafile.println("]");
  datafile.print("["); datafile.print("T4="); datafile.print(Marker[3][1]);  datafile.println("]");
  datafile.print("["); datafile.print("t5="); datafile.print(Marker[4][0]);  datafile.println("]");
  datafile.print("["); datafile.print("T5="); datafile.print(Marker[4][1]);  datafile.println("]");
  datafile.print("["); datafile.print("t6="); datafile.print(Marker[5][0]);  datafile.println("]");
  datafile.print("["); datafile.print("T6="); datafile.print(Marker[5][1]);  datafile.println("]");
  datafile.print("["); datafile.print("t7="); datafile.print(Marker[6][0]);  datafile.println("]");
  datafile.print("["); datafile.print("T7="); datafile.print(Marker[6][1]);  datafile.println("]");
  datafile.print("["); datafile.print("t8="); datafile.print(Marker[7][0]);  datafile.println("]");
  datafile.print("["); datafile.print("T8="); datafile.print(Marker[7][1]);  datafile.println("]");
  datafile.print("["); datafile.print("t9="); datafile.print(Marker[8][0]);  datafile.println("]");
  datafile.print("["); datafile.print("T9="); datafile.print(Marker[8][1]);  datafile.println("]");
  datafile.print("["); datafile.print("t10="); datafile.print(Marker[9][0]); datafile.println("]");
  datafile.print("["); datafile.print("T10="); datafile.print(Marker[9][1]); datafile.println("]");

  // close the file:
  datafile.close();
  //Serial.println("Writing done.");
}


// Load selected roast profile
void readSDroastProfile(char* selectedProfile) {
  char character;
  String settingName;
  String settingValue;
  myFile = SD.open(selectedProfile);
  if (myFile) {
    while (myFile.available()) {
      character = myFile.read();
      while ((myFile.available()) && (character != '[')) {
        character = myFile.read();
      }
      character = myFile.read();
      while ((myFile.available()) && (character != '=')) {
        settingName = settingName + character;
        character = myFile.read();
      }
      character = myFile.read();
      while ((myFile.available()) && (character != ']')) {
        settingValue = settingValue + character;
        character = myFile.read();
      }
      if (character == ']') {
        // Apply the value to the parameter
        applyProfileSetting(settingName, settingValue);
        // Reset Strings
        settingName = "";
        settingValue = "";
      }
    }

    //Serial.println("Loading roast profile to array");
    /*for(int i = 0;i<10;i++){
      Serial.print(Marker[i][0]);Serial.print(" ");Serial.println(Marker[i][1]);
      }
    */
    // close the file:
    myFile.close();
  }
}

/* Apply the value to the parameter by searching for the parameter name
  Using String.toInt(); for Integers
  toFloat(string); for Float
  toBoolean(string); for Boolean
  toLong(string); for Long
*/
void applyProfileSetting(String settingName, String settingValue) {
  if (settingName == "t1") {
    Marker[0][0] = toFloat(settingValue);
  }
  if (settingName == "t2") {
    Marker[1][0] = toFloat(settingValue);
  }
  if (settingName == "t3") {
    Marker[2][0] = toFloat(settingValue);
  }
  if (settingName == "t4") {
    Marker[3][0] = toFloat(settingValue);
  }
  if (settingName == "t5") {
    Marker[4][0] = toFloat(settingValue);
  }
  if (settingName == "t6") {
    Marker[5][0] = toFloat(settingValue);
  }
  if (settingName == "t7") {
    Marker[6][0] = toFloat(settingValue);
  }
  if (settingName == "t8") {
    Marker[7][0] = toFloat(settingValue);
  }
  if (settingName == "t9") {
    Marker[8][0] = toFloat(settingValue);
  }
  if (settingName == "t10") {
    Marker[9][0] = toFloat(settingValue);
  }
  if (settingName == "T1") {
    Marker[0][1] = toFloat(settingValue);
  }
  if (settingName == "T2") {
    Marker[1][1] = toFloat(settingValue);
  }
  if (settingName == "T3") {
    Marker[2][1] = toFloat(settingValue);
  }
  if (settingName == "T4") {
    Marker[3][1] = toFloat(settingValue);
  }
  if (settingName == "T5") {
    Marker[4][1] = toFloat(settingValue);
  }
  if (settingName == "T6") {
    Marker[5][1] = toFloat(settingValue);
  }
  if (settingName == "T7") {
    Marker[6][1] = toFloat(settingValue);
  }
  if (settingName == "T8") {
    Marker[7][1] = toFloat(settingValue);
  }
  if (settingName == "T9") {
    Marker[8][1] = toFloat(settingValue);
  }
  if (settingName == "T10") {
    Marker[9][1] = toFloat(settingValue);
  }
}


// Read selected line from free roast protocol
void readSDfreeRoast(char* selectedFile, int line) {
  //Serial.print("Attempting .csv read of parameters for free roast replica... ");
  //Serial.print("freeMemory()="); Serial.println(freeMemory());
  File myFile2 = SD.open(selectedFile);
  const size_t BUFFER_SZ = 7;
  char Buffer[BUFFER_SZ];
  size_t bos = 0;    // current write position in buffer
  int c, col = 0;
  //Serial.print("reading ");Serial.println(selectedFile);
  while ((c = myFile2.read()) != -1) {

    if (col == line) {
      // If we get a delimiter, then we have a complete element.
      if (c == ';') {
        Buffer[bos] = '\0';         // terminate the string
        Main_Array[0] = atoi(Buffer);
        //Serial.print("Main_Array[0] = "); Serial.println(Main_Array[0]);
        bos = 0;                    // ready for next element
      } else if (c == '\n') {
        Buffer[bos] = '\0';         // terminate the string
        Main_Array[1] = atoi(Buffer);
        //Serial.print("Main_Array[1] = "); Serial.println(Main_Array[1]);
        bos = 0;                    // ready for next element
        break;
      }

      // Otherwise, store the character in the buffer.
      else if (bos < BUFFER_SZ - 1) {
        Buffer[bos++] = c;
      }
    } else {
      // If we get a delimiter, then we have a complete element.
      if (c == ';' || c == '\n') {
        Buffer[bos] = '\0';         // terminate the string
        // Serial.println(Buffer);
        bos = 0;                    // ready for next element
        if (c == '\n') {
          col++;
        }
      }
      // Otherwise, store the character in the buffer.
      else if (bos < BUFFER_SZ - 1) {
        Buffer[bos++] = c;
      }
    }
  }
  //Serial.print("Done reading line parameters. "); Serial.print("freeMemory()="); Serial.println(freeMemory());
  myFile2.close();
}


//  Check number of valid SD files
void validSDfiles(int ftype) {
  lr = 0;
  myFile = SD.open("/");
  myFile.rewindDirectory();
  File entry =  myFile.openNextFile();
  while (entry) {
    while (entry && isSDtype(entry.name(), ftype)) {
      entry.close();
      entry = myFile.openNextFile();
      lr++;
    } entry.close();
    entry =  myFile.openNextFile();
  }//Serial.print("Checking for valid file number... elements = ");Serial.println(lr);
  entry.close();
  myFile.rewindDirectory();
  myFile.close();
}

// SD file select according to file type
bool isSDtype(char* fullName, int ftype) {
  int8_t len = strlen(fullName);
  bool result;
  if (ftype == 0) {
    if (  strstr(strlwr(fullName + (len - 4)), ".txt")) {
      result = true;
    } else {
      result = false;
    }
  } else if (ftype == 1) {
    if (  strstr(strlwr(fullName + (len - 4)), ".csv")) {
      result = true;
    } else {
      result = false;
    }
  }
  return result;
}

// Log whole free roast process to specified file
void writeSDfreeRoast(int TIME, double TEMP) {
  String LOGname = fileName;
  LOGname += ".csv";
  if (SD.exists(LOGname)) {
    myFile = SD.open(LOGname, FILE_WRITE);
    if (myFile) {
      myFile.print(TIME);
      myFile.print(";");
      myFile.println(TEMP); // Input temp.
      myFile.close();
      //  Serial.print(TIME);
      //  Serial.print(";");
      //  Serial.println(TEMP);
    }
  }
}


// Get ent time of roast from selected protocol. This assumes that pos is still at user specified value
void readEndSDfreeRoast() {
  File entry; myFile = SD.open("/");
  // Go to user selected filename
  myFile.rewindDirectory();
  for (int i = 0; i <= pos; i++) { // Serial.print("i = ");Serial.println(i);
    do {  // Go to next file of correct type
      entry = myFile.openNextFile();
      entry.close();
    } while (!isSDtype(entry.name(), 1)); //Serial.print("Skipping files: ");Serial.println(entry.name());}
  }
  File myFile2 = SD.open(entry.name());
  const size_t BUFFER_SZ = 7;
  char Buffer[BUFFER_SZ];
  size_t bos = 0;    // current write position in buffer
  int c;
  // Read in whole file, last time parameter will be stored
  while ((c = myFile2.read()) != -1) {
    // If we get a delimiter, then we have a complete element.
    if (c == '\n') {
      Buffer[bos] = '\0';         // terminate the string
      Main_Array[0] = atoi(Buffer);
      // Serial.print("Main_Array[0] = ");Serial.println(Main_Array[0]);  // send to the RPi
      bos = 0;                    // ready for next element
    }
    // Otherwise, store the character in the buffer.
    else if (bos < BUFFER_SZ - 1) {
      Buffer[bos++] = c;
    }
  }
  roastParam[0] = Main_Array[0];
  //Serial.print("End time of log: "); Serial.println(roastParam[0]);
  entry.close();
  myFile.close();
  myFile2.close();
}
