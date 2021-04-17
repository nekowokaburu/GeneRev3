// =====================================================================
//
// CONTROL - Control over encoder (internal pullups enabled)
//
// =====================================================================
// *********************************************************************
// therory:
// "#if" is a preprocessor directive and no error, look here:
// (english) https://en.wikipedia.org/wiki/C_preprocessor
// (german)  https://de.wikipedia.org/wiki/C-Pr%C3%A4prozessor

// *********************************************************************
// CONTROL TASK, DO NOT CHANGE
// *********************************************************************
void LCDML_BACK_setup(LCDML_BACKEND_control)
// *********************************************************************
{
  // call setup
  LCDML_CONTROL_setup();
}
// backend loop
boolean LCDML_BACK_loop(LCDML_BACKEND_control)
{
  // call loop
  LCDML_CONTROL_loop();

  // go to next backend function and do not block it
  return true;
}
// backend stop stable
void LCDML_BACK_stable(LCDML_BACKEND_control)
{
}


// settings
#define _LCDML_CONTROL_encoder_high_active     0  // (0 = low active (pullup), 1 = high active (pulldown)) button
// // http://playground.arduino.cc/CommonTopics/PullUpDownResistor
// global defines
uint8_t  g_LCDML_CONTROL_encoder_t_prev = 0;
uint8_t  g_LCDML_CONTROL_encoder_a_prev = 0;
uint8_t  g_LCDML_CONTROL_encoder_u_prev = 0;
uint8_t  g_LCDML_CONTROL_encoder_c_prev = 0;

// *********************************************************************
// setup
void LCDML_CONTROL_setup()
{
  // set encoder update intervall time
  LCDML_BACK_dynamic_setLoopTime(LCDML_BACKEND_control, 1UL);  // 5ms

  // init pins
  pinMode(_LCDML_CONTROL_encoder_pin_a      , INPUT_PULLUP);
  pinMode(_LCDML_CONTROL_encoder_pin_b      , INPUT_PULLUP);
  pinMode(_LCDML_CONTROL_encoder_pin_buttonBlue , INPUT_PULLUP);
  pinMode(_LCDML_CONTROL_encoder_pin_c      , INPUT_PULLUP);
  pinMode(_LCDML_CONTROL_encoder_pin_d      , INPUT_PULLUP);
  pinMode(_LCDML_CONTROL_encoder_pin_buttonRed , INPUT_PULLUP);
}
// *********************************************************************
// loop
void LCDML_CONTROL_loop()
{
  // read encoder status
  unsigned char a = digitalRead(_LCDML_CONTROL_encoder_pin_a);
  unsigned char b = digitalRead(_LCDML_CONTROL_encoder_pin_b);
  unsigned char t = digitalRead(_LCDML_CONTROL_encoder_pin_buttonBlue);
  unsigned char c = digitalRead(_LCDML_CONTROL_encoder_pin_c);
  unsigned char d = digitalRead(_LCDML_CONTROL_encoder_pin_d);
  unsigned char u = digitalRead(_LCDML_CONTROL_encoder_pin_buttonRed);

  // change button status if high and low active are switched
  if (_LCDML_CONTROL_encoder_high_active == 1) {
    t != t;
    u != u;
  }

  // check encoder status and set control menu
  if (!c && g_LCDML_CONTROL_encoder_c_prev) {
    g_LCDML_CONTROL_encoder_u_prev = 1;

    if (!d) {
      LCDML_BUTTON_left();
    }
    else    {
      LCDML_BUTTON_right();
    }
  }


  if (!a && g_LCDML_CONTROL_encoder_a_prev) {
    g_LCDML_CONTROL_encoder_t_prev = 1;

    if (!b) {
      LCDML_BUTTON_up();
    }
    else    {
      LCDML_BUTTON_down();
    }
  }
  else {
    // check button press time for enter
    if ((millis() - g_LCDML_DISP_press_time) >= _LCDML_DISP_cfg_button_press_time) {
      g_LCDML_DISP_press_time = millis(); // reset button press time

      // press button once
      if (!t && g_LCDML_CONTROL_encoder_t_prev == 0) {
        LCDML_BUTTON_enter();
      }
      else {
        g_LCDML_CONTROL_encoder_t_prev = 0;
      }

      // press button once
      if (!u && g_LCDML_CONTROL_encoder_u_prev == 0) {
        LCDML_BUTTON_quit();
      }
      else {
        g_LCDML_CONTROL_encoder_u_prev = 0;
      }
    }
  }
  g_LCDML_CONTROL_encoder_c_prev = c;  // set new encoder status
  g_LCDML_CONTROL_encoder_a_prev = a;  // set new encoder status
}
