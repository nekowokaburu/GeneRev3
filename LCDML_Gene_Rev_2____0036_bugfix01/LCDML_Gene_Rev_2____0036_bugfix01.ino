// ============================================================
//
// Example: LCDML_101_menu_with_parameter
//
// ============================================================

// include libs
#include <LCDMenuLib.h>
#include <U8glib.h>
#include <PID_v1.h>	// PID library
#include <SPI.h>
#include <SD.h>		// SD card library
#include "Adafruit_MAX31855.h"
//#include <MemoryFree.h>

// define Arduino pins, SD not needed in settings because fixed pins
/* SD card connection automatically set up if connected to pins (on Arduino Mega):
   3.3 V
   GND
** SD card attached to SPI bus as follows:
** MOSI - pin 51
** MISO - pin 50
** CLK  - pin 52
** CS   - pin 53
*/
#define Drum_PIN  7	// 2k Ohm
#define rotEndSw  A1
#define Fan_PIN   2	// 4k7 Ohm
#define SSR_PIN   11
#define thermoCLK 4
#define thermoCS  5
#define thermoDO  6
#define maxTemp   240
#define maxTime   1500
#define minCool   100
#define _LCDML_CONTROL_encoder_pin_a           20   // CLK encoder blue
#define _LCDML_CONTROL_encoder_pin_b           19    // DT encoder blue
#define _LCDML_CONTROL_encoder_pin_buttonBlue  18    // SW encoder blue
#define _LCDML_CONTROL_encoder_pin_c           13   // CLK encoder red
#define _LCDML_CONTROL_encoder_pin_d           12   // DT encoder red
#define _LCDML_CONTROL_encoder_pin_buttonRed   3    // SW encoder red


// Variable declarations
double slopePot = 1.00;
int Temp_Cool   =  80;
int Drum_Speed  = 127;
int Fan_Speed   = 255;
double crackTempSetp[5][2] = {0};
double roastParam[3] = {220, 880, 180}; // Preheat, roastTime, roastTemp
double startTime = millis();
bool rmBeansBeforeCooldown = false;
bool saveRoastToSD = true;
bool SDexist = false;
bool fileExist = true;
bool SDoverride = false;
int pos = 1; int lr = 0; int stage = 0; int coolStage = 0;  // position selection in loops
double Marker[10][2] = {{20, 70}, {44, 80}, {60, 50}};
char fileName[6] = "-----";
const char alphabetNr[38] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
double Main_Array[2] = {1500000, 100}; // Array will contain current SD values for loaded free roast protocol
int rememberProfile[110] = {};  // Saves time/temp. data during roast
int slot = 0; // number of to fill time slot in rememberProfile


// For displaying numbers
enum {BufSize = 20}; // If a is short use a smaller number, eg 5 or 6
char buf[BufSize];

// Roasting temperature PID
int Kp = 720;
int Ki = 0;
int Kd = 0;
unsigned int Wi = 5000;
unsigned long windowStartTime;
int shiftSP = 4.5;  // Increases Setpoint to reach desired temp without changing PID too often.
double Setpoint, Input, Output;
//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// Initialize thermocouple
Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);

// Define parameters to be saved and read to/from SD card
File myFile;  // File for parameters on SD



// *********************************************************************
// U8GLIB
// *********************************************************************
// setup u8g object, please remove comment from one of the following constructor calls
// IMPORTANT NOTE: The following list is incomplete. The complete list of supported
// devices with all constructor calls is here: https://github.com/olikraus/u8glib/wiki/device
U8GLIB_ST7920_128X64 u8g(15, 16, 17, U8G_PIN_NONE);

// settings for u8g lib and lcd
#define _LCDML_u8g_lcd_w       128            // lcd width
#define _LCDML_u8g_lcd_h       64             // lcd height
#define _LCDML_u8g_font        u8g_font_6x13  // u8glib font (more fonts under u8g.h line 1520 ...)
#define _LCDML_u8g_font_w      6              // u8glib font width
#define _LCDML_u8g_font_h      13             // u8glib font heigt

// nothing change here
#define _LCDML_u8g_cols_max    (_LCDML_u8g_lcd_w/_LCDML_u8g_font_w)
#define _LCDML_u8g_rows_max    (_LCDML_u8g_lcd_h/_LCDML_u8g_font_h)

// rows and cols
// when you use more rows or cols as allowed change in LCDMenuLib.h the define "_LCDML_DISP_cfg_max_rows" and "_LCDML_DISP_cfg_max_string_length"
// the program needs more ram with this changes
#define _LCDML_u8g_rows        _LCDML_u8g_rows_max  // max rows 
#define _LCDML_u8g_cols        20                   // max cols

// scrollbar width
#define _LCDML_u8g_scrollbar_w 6  // scrollbar width  

// old defines with new content
#define _LCDML_DISP_cols      _LCDML_u8g_cols
#define _LCDML_DISP_rows      _LCDML_u8g_rows

// lib config
#define _LCDML_DISP_cfg_button_press_time          200    // button press time in ms
#define _LCDML_DISP_cfg_scrollbar                  1      // enable a scrollbar
#define _LCDML_DISP_cfg_cursor                     0x7E   // cursor Symbol 

// *********************************************************************
// LCDML MENU/DISP
// *********************************************************************
// create menu
// menu element count - last element id
// this value must be the same as the last menu element
#define _LCDML_DISP_cnt    11

// LCDML_root        => layer 0
// LCDML_root_X      => layer 1
// LCDML_root_X_X    => layer 2
// LCDML_root_X_X_X  => layer 3
// LCDML_root_... 	 => layer ...

// LCDMenuLib_add(id, group, prev_layer_element, new_element_num, lang_char_array, callback_function)
//LCDML_DISP_init(_LCDML_DISP_cnt);
LCDML_DISP_initParam(_LCDML_DISP_cnt); // enbable parameters (needs one byte per menu element)
LCDML_DISP_add      (0  , _LCDML_G1  , LCDML_root        , 1  , "Select Template"           , LCDML_FUNC_Load_Template);
LCDML_DISP_add      (1  , _LCDML_G1  , LCDML_root        , 2  , "Replicate free roast"           , LCDML_FUNC_Replicate_Free_Roast);
LCDML_DISP_add      (2  , _LCDML_G2  , LCDML_root        , 3  , "Free Roast "         , LCDML_FUNC_Free_Roast);
LCDML_DISP_add      (3  , _LCDML_G2  , LCDML_root      , 4  , "Options"        , LCDML_FUNC);
LCDML_DISP_add      (4  , _LCDML_G2  , LCDML_root_4      , 1  , "Start cooling"          , LCDML_FUNC_Cooling);
LCDML_DISP_add      (5  , _LCDML_G1  , LCDML_root_4      , 2  , "Create Profile"          , LCDML_FUNC_Create_Profile);
LCDML_DISP_add      (6  , _LCDML_G2  , LCDML_root_4      , 3  , "Settings"          , LCDML_FUNC);
LCDML_DISP_add      (7  , _LCDML_G2  , LCDML_root_4_3      , 1  , "Temp. cool"          , LCDML_FUNC_Set_Temp_Cool);
LCDML_DISP_add      (8  , _LCDML_G2  , LCDML_root_4_3      , 2  , "Cool beans?"          , LCDML_FUNC_rmBeansBeforeCooldown);
LCDML_DISP_add      (9  , _LCDML_G2  , LCDML_root_4_3      , 3  , "Drum speed"          , LCDML_FUNC_Set_Drum_Speed);
LCDML_DISP_add      (10  , _LCDML_G2  , LCDML_root_4_3      , 4  , "Fan speed"          , LCDML_FUNC_Set_Fan_Speed);
LCDML_DISP_add      (11  , _LCDML_G2  , LCDML_root_4_3      , 5  , "PID parameters"          , LCDML_FUNC_Set_PID_Parameters);
LCDML_DISP_createMenu(_LCDML_DISP_cnt);




// *********************************************************************
// LCDML BACKEND (core of the menu, do not change here anything yet)
// *********************************************************************
// define backend function
#define _LCDML_BACK_cnt    2  // last backend function id

LCDML_BACK_init(_LCDML_BACK_cnt);
LCDML_BACK_new_timebased_dynamic (0  , ( 20UL )         , _LCDML_start  , LCDML_BACKEND_control);
LCDML_BACK_new_timebased_static (1  , ( 500UL )         , _LCDML_start  , LCDML_BACKEND_position_drum);
LCDML_BACK_new_timebased_dynamic (2  , ( 1000UL )       , _LCDML_stop   , LCDML_BACKEND_menu);
LCDML_BACK_create();


// *********************************************************************
// SETUP
// *********************************************************************
void setup()
{
  // PID brew setup
  pinMode(SSR_PIN, OUTPUT); digitalWrite(SSR_PIN, LOW); // Disable heater until PID runs
  pinMode(Drum_PIN, OUTPUT);
  pinMode(Fan_PIN, OUTPUT);
  pinMode(rotEndSw, INPUT); digitalWrite(rotEndSw, HIGH);

  // serial init; only be needed if serial control is used
  //while (!Serial);                   // wait until serial ready
  //Serial.begin(9600);                // start serial
  // Serial.println("Enabling Groups...");

  // Enable all valid starting groups
  LCDML_DISP_groupDisable(_LCDML_G1); // disable group 1
  LCDML_DISP_groupEnable(_LCDML_G2); // enable group 3


  //Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(53, OUTPUT);
  if (!SD.begin(53)) {
    //Serial.println("initialization failed!");
    dispTxt("No SD found!");
  } else {
    //Serial.println("initialization done.");
    // Load user SD-Settings
    readSDSettings();
    LCDML_DISP_groupEnable(_LCDML_G1); // enable group 1
    SDexist = true;
  }


  // Set up PID
  Setpoint = 100;
  Input = 205;  // For startup
  windowStartTime = millis();  // Set PID start time
  myPID.SetOutputLimits(0, Wi);
  myPID.SetMode(AUTOMATIC);

  // LCDMenu Setup
  LCDML_setup(_LCDML_BACK_cnt);
}

// Reset function
void(* resetFunc) (void) = 0;  // declare reset fuction at address 0

// *********************************************************************
// LOOP
// *********************************************************************
void loop()
{
  // this function must called here, do not delete it
  LCDML_run(_LCDML_priority);
}




// *********************************************************************
// check some errors - do not change here anything
// *********************************************************************
# if(_LCDML_DISP_rows > _LCDML_DISP_cfg_max_rows)
# error change value of _LCDML_DISP_cfg_max_rows in LCDMenuLib.h
# endif
# if(_LCDML_DISP_cols > _LCDML_DISP_cfg_max_string_length)
# error change value of _LCDML_DISP_cfg_max_string_length in LCDMenuLib.h
# endif
