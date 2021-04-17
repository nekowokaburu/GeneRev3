/* ===================================================================== *
*                                                                       *
  DISPLAY SYSTEM
*                                                                       *
  =====================================================================
  every "disp menu function" needs three functions
  - void LCDML_DISP_setup(func_name)
  - void LCDML_DISP_loop(func_name)
  - void LCDML_DISP_loop_end(func_name)

  EXAMPLE CODE:
	void LCDML_DISP_setup(..menu_func_name..)
	{
	// setup
	// is called only if it is started

	// starts a trigger event for the loop function every 100 millisecounds
	LCDML_DISP_triggerMenu(100);
	}

	void LCDML_DISP_loop(..menu_func_name..)
	{
	// loop
	// is called when it is triggert
	// - with LCDML_DISP_triggerMenu( millisecounds )
	// - with every button status change

	// check if any button is presed (enter, up, down, left, right)
	if(LCDML_BUTTON_checkAny()) {
		LCDML_DISP_funcend();
	}
	}

	void LCDML_DISP_loop_end(..menu_func_name..)
	{
	// loop end
	// this functions is ever called when a DISP function is quit
	// you can here reset some global vars or do nothing
	}
  =====================================================================
*/


// *********************************************************************
//			Roast by loading template from SD
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_Load_Template) {
  memset(rememberProfile, 0, sizeof(rememberProfile));  // Checks for file type and set number to "lr"
  stage = 0;
  pos = 0;
  lr = 0;
  validSDfiles(0);
  coolStage = 0;
}
void LCDML_DISP_loop(LCDML_FUNC_Load_Template) {
  Serial.print("Roast Marker case: "); Serial.println(stage);
  //Serial.print("freeMemory()=");Serial.println(freeMemory());

  switch (stage) {
    case 0: if (dispFileSelect(0, 0, 0)) {
        stage++;
        Main_Array[1] = maxValue(Marker);
      } break; // Select from .csv only, load Marker
    case 1: if (preheat()) {
        stage++;
      } break;  // Let user set preheat temp. and actually preheat roaster
    case 2: if (waitForUser()) {
        stage++;
        startTime = millis();
        lr = 0;
      } break; // wait for user to start roast
    case 3: if (roast(2)) {
        stage++;
        startTime = millis();
      } break;  // startTime for coolDown time display
    case 4: if (coolDown(0)) {
        LCDML_DISP_funcend();
      } break;
  }

}
void LCDML_DISP_loop_end(LCDML_FUNC_Load_Template) {
  if(dispPreheatSel()){parkRoaster();}   // On Quit park roaster
  stage = 0; pos = 0; lr = 0; slot = 0; // number of to fill time slot
  Serial.println("Loop End DONE!");
}



// *********************************************************************
//      Replicate free roast from .csv file
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_Replicate_Free_Roast) {
  stage = 0;  // Checks for file type and set number to "lr"
  pos = 0;
  lr = 0;
  validSDfiles(1);
  coolStage = 0;
  memset(rememberProfile, 0, sizeof(rememberProfile));
}
void LCDML_DISP_loop(LCDML_FUNC_Replicate_Free_Roast) {
  //Serial.print("Free roast case: "); Serial.println(stage);
  //Serial.print("freeMemory()=");Serial.println(freeMemory());
  switch (stage) {
    case 0: if (dispFileSelect(1, 0, 0)) {
        stage++;
      } break; // Select from .csv only, load preheat values
    case 1: if (preheat()) {
        stage++;
      } break;  // Let user set preheat temp. and actually preheat roaster
    case 2: if (waitForUser()) {
        stage++;
        startTime = millis();
        readEndSDfreeRoast();
        roastParam[1] = 0;
        roastParam[2] = 0;
        dispFileSelect(1, 1, 1);
      } break; // wait for user to start roast
    case 3: if (roast(1)) {
        stage++;
        startTime = millis();
      } break; // Free roast process
    case 4: if (coolDown(0)) {
        LCDML_DISP_funcend();
      } break;
  }
}
void LCDML_DISP_loop_end(LCDML_FUNC_Replicate_Free_Roast) {
  if(dispPreheatSel()){parkRoaster();}   // On Quit park roaster
  stage = 0; pos = 0; lr = 0; slot = 0; // number of to fill time slot
}



// *********************************************************************
//      Roast freestyle
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_Free_Roast) {
  stage = 0;
  pos = 0;
  lr = 0;
  coolStage = 0;
  memset(rememberProfile, 0, sizeof(rememberProfile));
}
void LCDML_DISP_loop(LCDML_FUNC_Free_Roast) {
  //Serial.print("Free roast case: ");Serial.println(stage);
  //Serial.print("freeMemory()=");Serial.println(freeMemory());
  //Serial.print("SD ?: ");Serial.print(SDexist);
  //Serial.print("saveRoastToSD: ");Serial.print(saveRoastToSD);//if(safeRoastToSD){Serial.println("TRUE");}else{Serial.println("FALSE");}
  switch (stage) {
    case 0: if (dispFreeRoastStartParam()) {
        stage++;
      } break; // User sets end time and first temp for roast  IMPLEMENTIEREN!! auch abkühlen mit bohnen ja/nein
    case 1: if (dispRMbeansBeforeCooldown()) {
        stage++;
      } break; //
    case 2: if (dispSaveRoast()) {
        if (SDexist && saveRoastToSD) {
          stage++;
        } else {
          stage = stage + 2;;
        }
      } break;
    case 3: if (enterFreeRoastFileName()) {
        stage++;
      } break;
    case 4: if (dispPreheatSel()) {
        stage++;
        if (saveRoastToSD) {
          writeSDfreeRoast(999, roastParam[0]);
        }
      } break;  // Select preheat temp and write it to first line of LOG
    case 5: if (preheat()) {
        stage++;
      } break;  // Let user set preheat temp. and actually preheat roaster
    case 6: if (waitForUser()) {
        stage++;
        startTime = millis();
        roastParam[0] = 0;
      } break; // wait for user to start roast
    case 7: if (roast(0)) {
        stage++;
        startTime = millis();
      } break;  // Free roast process
    case 8: if (coolDown(0)) {
        LCDML_DISP_funcend();
      } break;
  }
}
void LCDML_DISP_loop_end(LCDML_FUNC_Free_Roast) {
  if(dispPreheatSel()){parkRoaster();}   // On Quit park roaster
  stage = 0; pos = 0; lr = 0; slot = 0; // number of to fill time slot
}



// ____________________________________________________________________________________________________________________
//      OPTIONS
// ____________________________________________________________________________________________________________________
// *********************************************************************
//      Start Cooling
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_Cooling) {
  startTime = millis();
  stage = 0;
  coolStage = 0;
}
void LCDML_DISP_loop(LCDML_FUNC_Cooling) {
  if (coolDown(1)) {
    LCDML_DISP_funcend();
  }
}
void LCDML_DISP_loop_end(LCDML_FUNC_Cooling) {
  stage = 0;
  analogWrite(Fan_PIN, 0);
  analogWrite(Drum_PIN, 0);
}

// *********************************************************************
//      Create and save new roast profile (on SD)
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_Create_Profile) {
  pos = 0;
  lr = 0;
  stage = 0;
  LCDML_BUTTON_resetAll();
}
void LCDML_DISP_loop(LCDML_FUNC_Create_Profile) {
  //Serial.print("Stage = "); Serial.print(stage); Serial.print(" pos = "); Serial.print(pos); Serial.print(" lr = "); Serial.println(lr);
  if (stage == 0) { // Select 10 values
    if (pos == 10) { // If done go to next stage (naming and save to SD)
      if (LCDML_BUTTON_checkEnter()) {
        LCDML_BUTTON_resetEnter();
        pos = 0;
        lr = 0;
        stage++;
      }
      if (LCDML_BUTTON_checkLeft()) {
        if (pos > 0) {
          pos--;  // move up again
          lr = 1;
        } LCDML_BUTTON_resetLeft();
      }
    } else {
      if (LCDML_BUTTON_checkEnter()) {
        if (lr == 1) {
          if (pos >= 10) {
            pos = 10; // Press blue button switches to next selectable value
          } else {
            pos++;
            lr--;
          }
        } else {
          lr++;
        } LCDML_BUTTON_resetEnter();
      }
      if (LCDML_BUTTON_checkLeft()) {
        if (pos > 0 && lr == 1) {
          lr = 0; // move throug different pairs of time and temp. with red turn
        } else if (pos > 0 && lr == 0) {
          pos--;
          lr = 1;
        } LCDML_BUTTON_resetLeft();
      }
      if (LCDML_BUTTON_checkRight()) {
        if (pos < 10 && lr == 1) {
          pos++;
          lr = 0;
        } else if (pos < 10 && lr == 0) {
          lr = 1;
        } LCDML_BUTTON_resetRight();
      }
      if (LCDML_BUTTON_checkUp()) {
        if (Marker[pos][lr] > 0) {
          Marker[pos][lr]--; // Change selected time or temp. value with blue turn
        } LCDML_BUTTON_resetUp();
      }
      if (LCDML_BUTTON_checkDown()) {
        if (Marker[pos][lr] < 1500) {
          Marker[pos][lr]++;
        } LCDML_BUTTON_resetDown();
      }
    }
    dispCreateProfile();  // Display current values on lcd
  }
  if (stage == 1) { // Enter name for created profile and save
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
          //Serial.println("lr changes");
        } LCDML_BUTTON_resetLeft();
      }
      if (LCDML_BUTTON_checkRight()) {
        if (lr < 5) {
          lr++;
        } LCDML_BUTTON_resetRight();
      }
      dispProfileNaming();
    } else {  // last letter entered, ask to save
      if (LCDML_BUTTON_checkLeft()) {
        lr--;  // Down allowed to exit from save
        LCDML_BUTTON_resetLeft();
      }
      if (LCDML_BUTTON_checkEnter()) {
        LCDML_BUTTON_resetEnter();  // Write letter to file name
        pos = 0;
        lr = 0;
        stage++;
      }
    }
  }
  if (stage == 2) {
    // Check if file already exists
   // Serial.print("Checking SD for duplicates... filecheck= ");
    boolean filecheck = SD.exists(String(fileName) + ".txt");
    //Serial.print(filecheck);
    if (filecheck) {
      //Serial.print("...requesting user input...");
      if (dispFileExist()) {
        //Serial.print("user decided user="); Serial.print(SDoverride);
        if (SDoverride) {
          SD.remove(String(fileName) + ".txt"); //Serial.print("...deleting file...");
          writeSDprofile(fileName); dispTxt("Profile saved!"); wait(1); pos = 0; lr = 0; stage = 0; stage++; LCDML_DISP_funcend();
        }
        if (!SDoverride) {
          dispTxt("Profile discarded!"); wait(1); pos = 0; lr = 0; stage = 0; stage++; LCDML_DISP_funcend();
        }
      }
    }
    if (!filecheck) {
      //Serial.println("Writing new file to SD card.");
      writeSDprofile(fileName); pos = 0; lr = 0; stage = 0; stage++; LCDML_DISP_funcend();
      dispTxt("Profile saved!"); wait(1);
    }
  }
}
void LCDML_DISP_loop_end(LCDML_FUNC_Create_Profile) {}


// ------------------------------------------------------------------------------------------
//      Create and save new roast profile (on SD)
// ------------------------------------------------------------------------------------------
// *********************************************************************
//      Set shutdown temperature when cool enough
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_Set_Temp_Cool) {}
void LCDML_DISP_loop(LCDML_FUNC_Set_Temp_Cool) {
  if (LCDML_BUTTON_checkUp())   {
    Temp_Cool--;
  }
  if (LCDML_BUTTON_checkDown()) {
    Temp_Cool++;
  }

  if (LCDML_BUTTON_checkEnter()) {
    writeSDSettings();
    dispTxt("Settings saved!"); wait(1);
    LCDML_BUTTON_resetAll();
    LCDML_DISP_funcend();
  }
  dispTxtVal("Cool shutoff temp.:", Temp_Cool, 0, "\260C");
  LCDML_BUTTON_resetAll();
}
void LCDML_DISP_loop_end(LCDML_FUNC_Set_Temp_Cool) {}

// *********************************************************************
//      Select if beans should be removed before coolDown process
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_rmBeansBeforeCooldown) {}
void LCDML_DISP_loop(LCDML_FUNC_rmBeansBeforeCooldown) {
  if (dispRMbeansBeforeCooldown()) {
    writeSDSettings();
    LCDML_DISP_funcend();
  }
}
void LCDML_DISP_loop_end(LCDML_FUNC_rmBeansBeforeCooldown) {}

// *********************************************************************
//      Set constant drum speed
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_Set_Drum_Speed) {}
void LCDML_DISP_loop(LCDML_FUNC_Set_Drum_Speed) {
  if (LCDML_BUTTON_checkUp())   {
    Drum_Speed--;
    if (Drum_Speed <= 0) {
      Drum_Speed = 0;
    }
  }
  if (LCDML_BUTTON_checkDown()) {
    Drum_Speed++;
    if (Drum_Speed >= 255) {
      Drum_Speed = 255;
    }
  }

  if (LCDML_BUTTON_checkEnter()) {
    writeSDSettings();
    dispTxt("Settings saved!"); wait(1);
    LCDML_BUTTON_resetAll();
    LCDML_DISP_funcend();
  }
  dispTxtVal("Drum Speed: ", float(Drum_Speed) * 100 / 255, 2, " %");
  LCDML_BUTTON_resetAll();
}
void LCDML_DISP_loop_end(LCDML_FUNC_Set_Drum_Speed) {}

// *********************************************************************
//      Set constant fan speed
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_Set_Fan_Speed) {}
void LCDML_DISP_loop(LCDML_FUNC_Set_Fan_Speed) {
  if (LCDML_BUTTON_checkUp())   {
    Fan_Speed--;
    if (Fan_Speed <= 0) {
      Fan_Speed = 0;
    }
  }
  if (LCDML_BUTTON_checkDown()) {
    Fan_Speed++;
    if (Fan_Speed >= 255) {
      Fan_Speed = 255;
    }
  }

  if (LCDML_BUTTON_checkEnter()) {
    writeSDSettings();
    dispTxt("Settings saved!"); wait(1);
    LCDML_BUTTON_resetAll();
    LCDML_DISP_funcend();
  }
  dispTxtVal("Fan Speed: ", float(Fan_Speed) * 100 / 255, 2, "  %");
  LCDML_BUTTON_resetAll();
}
void LCDML_DISP_loop_end(LCDML_FUNC_Set_Fan_Speed) {}

// *********************************************************************
//      Set PID parameters and save on SD if aviable to load on next startup
// *********************************************************************
void LCDML_DISP_setup(LCDML_FUNC_Set_PID_Parameters) {}
void LCDML_DISP_loop(LCDML_FUNC_Set_PID_Parameters) {
  // Select Kp, Ki or Kd
  if (LCDML_BUTTON_checkLeft())   {
    pos--;
    if (pos <= 1) {
      pos = 1;
    }
  }
  if (LCDML_BUTTON_checkRight()) {
    pos++;
    if (pos >= 3) {
      pos = 3;
    }
  }

  if (pos == 1) {
    if (LCDML_BUTTON_checkUp()) {
      Kp--; // Set Kp
    } if (LCDML_BUTTON_checkDown()) {
      Kp++;
    }
  }
  if (pos == 2) {
    if (LCDML_BUTTON_checkUp()) {
      Ki--; // Set Ki
    } if (LCDML_BUTTON_checkDown()) {
      Ki++;
    }
  }
  if (pos == 3) {
    if (LCDML_BUTTON_checkUp()) {
      Kd--; // Set Kd
    } if (LCDML_BUTTON_checkDown()) {
      Kd++;
    }
  }

  if (LCDML_BUTTON_checkEnter()) {
    writeSDSettings();
    dispTxt("Settings saved!"); wait(1);
    LCDML_BUTTON_resetAll();
    LCDML_DISP_funcend();
  }
  LCDML_BUTTON_resetAll();
  dispSetPID();
}
void LCDML_DISP_loop_end(LCDML_FUNC_Set_PID_Parameters) {}




// ________________________________________________________________________
// Functions for general purpose
//_________________________________________________________________________
//:::::::::::::::::::::::::::INERESTING STUFF:::::::::::::::::::::::::::::::::::::::
// LCDML.goRoot(); // go to root element (first element of this menu with id=0)

// Wait without delay
//  if (currentMillis - previousMillis >= interval) {
//    previousMillis = currentMillis;
//  }

// check if any button is presed (enter, up, down, left, right)
//if(LCDML_BUTTON_checkAny()) {
//  LCDML_DISP_funcend();
//}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
