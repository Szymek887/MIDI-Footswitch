// Include all libraries
#include <Arduino.h>
#include <MIDI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>

// Initialize OLED display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Initialize MIDI
MIDI_CREATE_DEFAULT_INSTANCE();

// Maximum time before long press in miliseconds
const int SHORT_PRESS_TIME = 1000;

// Variables for different modes
String midiCommand = "CC";
String buttonMode = "DT";
int preset = 10;

// Variables for defining if signal was sent
bool control1Pressed = false;
bool control2Pressed = false;
bool control3Pressed = false;
bool control4Pressed = false;
bool control5Pressed = false;

// Declaring buttons
// Button 1 = Change MIDI command (PC, CC) 
int button1Pin = 2;
int button1StateOld;
int button1StateNew;
bool button1IsClicked = false;
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;
long pressDuration = 0;

// Button 2 = Change mode (SPST, DPDT)
int button2Pin = 3;
int button2StateOld;
int button2StateNew;
bool button2IsClicked = false;

// Button 3-7 = Send signals
int button3Pin = 4;
int button3StateOld;
int button3StateNew;
bool button3IsClicked = false;

int button4Pin = 5;
int button4StateOld;
int button4StateNew;
bool button4IsClicked = false;

int button5Pin = 6;
int button5StateOld;
int button5StateNew;
bool button5IsClicked = false;

int button6Pin = 7;
int button6StateOld;
int button6StateNew;
bool button6IsClicked = false;

int button7Pin = 8;
int button7StateOld;
int button7StateNew;
bool button7IsClicked = false;

// Default MIDI channel
#define defaultChannel 1

// Displaying text centered on screen
void oledDisplayCenter(String text)
{
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  display.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);

  // display on horizontal and vertical center
  display.setCursor((SCREEN_WIDTH - width) / 2, 0);
  display.println(text); // text to display
}

// Check state of every button
void checkButtonState()
{
  button1StateNew = digitalRead(button1Pin);
  button2StateNew = digitalRead(button2Pin);
  button3StateNew = digitalRead(button3Pin);
  button4StateNew = digitalRead(button4Pin);
  button5StateNew = digitalRead(button5Pin);
  button6StateNew = digitalRead(button6Pin);
  button7StateNew = digitalRead(button7Pin);
}

// Change info displayed on OLED
void updateInfoOnDisplay()
{
  display.clearDisplay();
  display.setCursor(0,0);
  oledDisplayCenter(midiCommand);
  display.print("P");
  display.print(preset / 10);
  display.print(" ");
  display.println(buttonMode);
}

// Initialization of Arduino
void setup() {
  // Initialize serial connection
  Serial.begin(9600);

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  // Initialize MIDI
  MIDI.begin(MIDI_CHANNEL_OFF);

  // Change pin modes
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  pinMode(button4Pin, INPUT_PULLUP);
  pinMode(button5Pin, INPUT_PULLUP);
  pinMode(button6Pin, INPUT_PULLUP);
  pinMode(button7Pin, INPUT_PULLUP);
}

void loop() {
  // Update button states
  checkButtonState();

  // Check if MIDI command or button mode was changed
  if (button1StateNew == 0 && button1IsClicked == false)
  {
    pressedTime = millis();

    button1IsClicked = true;
  } else if (button1StateNew == 1 && button1IsClicked == true)
  {
    releasedTime = millis();

    pressDuration = releasedTime - pressedTime;

    if (pressDuration < SHORT_PRESS_TIME)
    {
      if (midiCommand == "CC")
      {
        midiCommand = "PC";
      } else
      {
        midiCommand = "CC";
      }
    } else
    {
      if (buttonMode == "DT")
      {
        buttonMode = "ST";
      } else
      {
        buttonMode = "DT";
      }
    }

    button1IsClicked = false;
  }

  // Check if preset was changed
  if (button2StateNew == 0 && button2IsClicked == false)
  {
    preset += 10;
    if (preset == 90)
    {
      preset = 10;
    }
    button2IsClicked = true;
  } else if (button2StateNew == 1)
  {
    button2IsClicked = false;
  }

  // Send Control Change signals
  if (midiCommand == "CC")
  {
    // Button 1
    if (button3StateNew == 0 && button3IsClicked == false)
    {
      if (control1Pressed == false)
      {
        MIDI.sendControlChange(1 + (preset / 2),127,defaultChannel);
        control1Pressed = true;
      } else if (control1Pressed == true && buttonMode == "DT")
      {
        MIDI.sendControlChange(1 + (preset / 2),0,defaultChannel);
        control1Pressed = false;
      }

      button3IsClicked = true;
    } else if (button3StateNew == 1)
    {
      if (buttonMode == "ST" && control1Pressed == true)
      {
        MIDI.sendControlChange(1 + (preset / 2),0,defaultChannel);
        control1Pressed = false;
      }
      button3IsClicked = false;
    }

    // Button 2
    if (button4StateNew == 0 && button4IsClicked == false)
    {
      if (control2Pressed == false)
      {
        MIDI.sendControlChange(2 + (preset / 2),127,defaultChannel);
        control2Pressed = true;
      } else if (control2Pressed == true && buttonMode == "DT")
      {
        MIDI.sendControlChange(2 + (preset / 2),0,defaultChannel);
        control2Pressed = false;
      }

      button4IsClicked = true;
    } else if (button4StateNew == 1)
    {
      if (buttonMode == "ST" && control2Pressed == true)
      {
        MIDI.sendControlChange(2 + (preset / 2),0,defaultChannel);
        control2Pressed = false;
      }
      button4IsClicked = false;
    }

  } 
  // Send Program change signals
  else if (midiCommand == "PC")
  {
    // Button 1, PC 0
    if (button3StateNew == 0 && button3IsClicked == false)
    {
      MIDI.sendProgramChange(0,defaultChannel);

      button3IsClicked = true;
    } else if (button3StateNew == 1)
    {
      button3IsClicked = false;
    }
    
    // Button 2, PC 1
    if (button4StateNew == 0 && button4IsClicked == false)
    {
      MIDI.sendProgramChange(1,defaultChannel);

      button4IsClicked = true;
    } else if (button4StateNew == 1)
    {
      button4IsClicked = false;
    }
  }

  // Update display values
  updateInfoOnDisplay();
  display.display();
  delay(5);
}