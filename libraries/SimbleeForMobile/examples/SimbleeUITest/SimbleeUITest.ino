/*
  Copyright (c) 2015 RF Digital Corp. All Rights Reserved.

  The source code contained in this file and all intellectual property embodied in
  or covering the source code is the property of RF Digital Corp. or its licensors.
  Your right to use this source code and intellectual property is non-transferable,
  non-sub licensable, revocable, and subject to terms and conditions of the
  SIMBLEE SOFTWARE LICENSE AGREEMENT.
  http://www.simblee.com/licenses/SimbleeSoftwareLicenseAgreement.txt

  THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.

  This heading must NOT be removed from this file.
*/

/*
  SimbleUITest demonstraits how multiple screens can be used in SimbleeForMobile as well
  as using multiple screens to display and interact with the various user interface
  widgets available in SimbleeForMobile.

  The application consists of a "Home" screen with buttons that open a new screen which
  displays various user interface widgets. Many different SimbleeForMobile APIs are used
  to create the widgets. Also, some wild color combinations are used to demonstrait capability,
  if not good design.
  
  The various events the widgets generate are handled in the event handling method associated 
  with each screen.
*/

#include <SimbleeForMobile.h>

#include "YellowDot.h"
#include "RedDot.h"
#include "GreenDot.h"

//
// Image numbers
//
#define YELLOW_DOT_IMAGE 1
#define RED_DOT_IMAGE 2
#define GREEN_DOT_IMAGE 3

//
// Screen numbers
//
#define HOME_SCREEN 1
#define TEXT_SCREEN 2
#define BUTTON_SCREEN 3
#define RECT_SCREEN 4
#define SWITCH_SEGMENT_SCREEN 5
#define SLIDER_STEPPER_SCREEN 6
#define TEXTFIELD_SCREEN 7
#define IMAGE_SCREEN 8

#define TITLE_TEXT_SIZE 40
#define HOME_BUTTON_LEFT_MARGIN 120
#define HOME_BUTTON_TOP_MARGIN 500
#define HOME_BUTTON_WIDTH 90
#define HOME_BUTTON_TEXT "Home"

//
// The ID of the button which displays the testing screens
//
int toTextTestingButtinID;
int toButtonTestingButtinID;
int toRectTestingButtinID;
int toSwitchTestingButtinID;
int toSlidderTestingButtinID;
int toSliderTestingButtinID;
int toStepperTestingButtinID;
int toImageTestingButtinID;

//
// The IDs of the various user interface widgets used in the screens.
//
int counterButtonID;
int buttonPressCount;

int blackSwitch;
int yellowSwitch;
int greenSwitch;
int cyanSwitch;
int blackText;
int yellowText;
int greenText;
int cyanText;
int segment;
int slaveSegment;
int segmentText;
int dragRect;
int dragRectX;
int dragRectY;
int dragRectStartX;
int dragRectStartY;

int xSlider;
int xSliderText;
int ySlider;
int ySliderText;
int stepper1;
int stepper1Text;
int stepper2;
int stepper2Text;

//
// The ID of the button which return to the home screen
// This variable is a bit unique as it is redefined on each
// test screen, but always returns to the home screen when pressed.
//
int toHomeScreenButtonID;

//
// The number of the current screen being displayed
//
int currentScreen;

/**
 * Traditional Arduino setup routine
 *
 * Initialize the SimbleeForMobile environment.
 */
void setup() {
  //
  // Enabled logging of the various events during application execution
  //
  Serial.begin(9600);
  Serial.println("Setup beginning");

  buttonPressCount = 0;

  SimbleeForMobile.deviceName = "UI Test";
  SimbleeForMobile.advertisementData = "tester";

  // use a shared cache
  SimbleeForMobile.domain = "testing.simblee.com";

  // Begin Simble UI
  SimbleeForMobile.begin();

  Serial.println("Setup completed");
}

/**
 * The traditional Arduino loop method
 *
 * Enable SimbleeForMobile functionality by calling the process method
 * each time through the loop. This method must be called regularly for
 * functionality to work.
 */
void loop() {
  SimbleeForMobile.process();
}

/**
 * Callback when a Central connects to this device
 *
 * Reset the current screen to non being displayed
 */
void SimbleeForMobile_onConnect()
{
  currentScreen = -1;
}

/**
 * Callback when a Central disconnects from this device
 *
 * Not used in this sketch. Could clean up any resources
 * no longer required.
 */
void SimbleeForMobile_onDisconnect()
{
}

/**
 * SimbleeForMobile ui callback requesting the user interface
 *
 * Check which screen is being requested.
 * If that is the current screen, simply return.
 * If it is a different screen, create the requested screen.
 * A screen request which is out of range, is logged and ignored.
 */
void ui()
{
  Serial.printf("Screen %d requested. Current screen is %d\n", SimbleeForMobile.screen, currentScreen);

  if (SimbleeForMobile.screen == currentScreen) return;

  currentScreen = SimbleeForMobile.screen;
  switch (SimbleeForMobile.screen)
  {
    case HOME_SCREEN:
      createHomeScreen();
      break;

    case TEXT_SCREEN:
      createTextScreen();
      break;

    case BUTTON_SCREEN:
      createButtonScreen();
      updateCounterButtonText();
      break;

    case RECT_SCREEN:
      createRectScreen();
      break;

    case SWITCH_SEGMENT_SCREEN:
      createSwitchSegmentScreen();
      break;

    case SLIDER_STEPPER_SCREEN:
      createSliderStepperScreen();
      break;

    case TEXTFIELD_SCREEN:
      createTextFieldScreen();
      break;

    case IMAGE_SCREEN:
      createImageScreen();
      break;

    default:
      Serial.print("ui: Uknown screen requested: ");
      Serial.println(SimbleeForMobile.screen);
  }
}

/**
 * SimbleeForMobile event callback method
 *
 * An event we've registered for, or a default event, has occur. Process the event.
 * Forward the event to the method which handles events for the screen currently displayed.
 */
void ui_event(event_t &event)
{
  if (event.type != EVENT_DRAG && event.type != 0) printEvent(event);
  // printEvent(event);

  switch (currentScreen)
  {
    case HOME_SCREEN:
      handleHomeScreenEvents(event);
      break;

    case TEXT_SCREEN:
      handleTextScreenEvents(event);
      break;

    case BUTTON_SCREEN:
      handleButtonScreenEvents(event);
      break;

    case SWITCH_SEGMENT_SCREEN:
      handleSwitchScreenEvents(event);
      break;

    case RECT_SCREEN:
      handleRectScreenEvents(event);
      break;

    case SLIDER_STEPPER_SCREEN:
      handleSliderStepperScreenEvents(event);
      break;

    case TEXTFIELD_SCREEN:
      handleTextFieldScreenEvents(event);
      break;

    case IMAGE_SCREEN:
      handleImageScreenEvents(event);
      break;
  }

  //
  // Could put the common "Home" button event here, but left it in each screen's handling to
  // keep each of those routines handling all the events for that screen.
  //
}

/**
 * Handle events which occur on the home screen.
 * 
 * Check which button generated the event and if it is an EVENT_RELEASE event type,
 * signifying the end of a button press, display the screen associated with the button.
 */
void handleHomeScreenEvents(event_t &event)
{
  if (event.id == toTextTestingButtinID && event.type == EVENT_RELEASE)
  {
    SimbleeForMobile.showScreen(TEXT_SCREEN);
  } else if (event.id == toButtonTestingButtinID && event.type == EVENT_RELEASE)
  {
    SimbleeForMobile.showScreen(BUTTON_SCREEN);
  } else if (event.id == toRectTestingButtinID && event.type == EVENT_RELEASE)
  {
    SimbleeForMobile.showScreen(RECT_SCREEN);
  } else if (event.id == toSwitchTestingButtinID && event.type == EVENT_RELEASE)
  {
    SimbleeForMobile.showScreen(SWITCH_SEGMENT_SCREEN);
  } else if (event.id == toSlidderTestingButtinID && event.type == EVENT_RELEASE)
  {
    SimbleeForMobile.showScreen(RECT_SCREEN);
  } else if (event.id == toSliderTestingButtinID && event.type == EVENT_RELEASE)
  {
    SimbleeForMobile.showScreen(SLIDER_STEPPER_SCREEN);
  } else if (event.id == toStepperTestingButtinID && event.type == EVENT_RELEASE)
  {
    SimbleeForMobile.showScreen(TEXTFIELD_SCREEN);
  } else if (event.id == toImageTestingButtinID && event.type == EVENT_RELEASE)
  {
    SimbleeForMobile.showScreen(IMAGE_SCREEN);
  }
}

/**
 * Handle events which occur on the Text screen.
 * 
 * The only event that is handled is the EVENT_RELEASE on the button which returns
 * to the home screen.
 */
void handleTextScreenEvents(event_t &event)
{
  if (event.id == toHomeScreenButtonID && event.type == EVENT_RELEASE) {
    SimbleeForMobile.showScreen(HOME_SCREEN);
  }
}

/**
 * Handle events which occur on the Button screen.
 * 
 * Handle returning to the home screen event as well as when the "Pressed" button
 * is pressed, update the pressed count and text on that button.
 */
void handleButtonScreenEvents(event_t &event)
{
  if (event.id == toHomeScreenButtonID && event.type == EVENT_RELEASE) {
    SimbleeForMobile.showScreen(HOME_SCREEN);
  } else if (event.id == counterButtonID && event.type == EVENT_RELEASE) {
    buttonPressCount += 1;
    updateCounterButtonText();
  }

}

/**
 * Handle events which occur on the Rectangle screen.
 * 
 * Handle returning to the home screen event.
 */
void handleRectScreenEvents(event_t &event)
{
  if (event.id == toHomeScreenButtonID && event.type == EVENT_RELEASE) {
    SimbleeForMobile.showScreen(HOME_SCREEN);
  } else if (event.id == dragRect && event.type == EVENT_PRESS) {
    dragRectStartX = event.x;
    dragRectStartY = event.y;
    Serial.printf("Start: %d,%d position: %d,%d\n", dragRectStartX, dragRectStartY, dragRectX, dragRectY);
  } else if (event.id == dragRect && event.type == EVENT_DRAG) {
    int deltaX = event.x - dragRectStartX;
    int deltaY = event.y - dragRectStartY;
    dragRectStartX = event.x;
    dragRectStartY = event.y;

    dragRectX += deltaX;
    dragRectY += deltaY;

    Serial.printf("Delta: %d,%d position: %d,%d\n", deltaX, deltaY, dragRectX, dragRectY);

  } else if (event.id == dragRect && event.type == EVENT_RELEASE) {
    Serial.printf("End: position: %d,%d\n", event.x, event.y);
  }
}

/**
 * Handle events which occur on the Switch and Segment screen.
 * 
 * Handle returning to the home screen event as well as when the switch change
 * and segments change. The switches hide and change color of the text fields
 * associated with the switches. The first segment updates the second segment
 * to display the same segment as pressed by the user. Pressing the second
 * segment doesn't cause any changes.
 */
void handleSwitchScreenEvents(event_t &event)
{
  if (event.id == toHomeScreenButtonID && event.type == EVENT_RELEASE) {
    SimbleeForMobile.showScreen(HOME_SCREEN);
  } else if (event.id == blackSwitch && event.value == 0) {
    SimbleeForMobile.setVisible(blackText, false);
  } else if (event.id == blackSwitch && event.value == 1) {
    SimbleeForMobile.setVisible(blackText, true);
  } else if (event.id == yellowSwitch && event.value == 0) {
    SimbleeForMobile.updateColor(yellowText, BLACK);
  } else if (event.id == yellowSwitch && event.value == 1) {
    SimbleeForMobile.updateColor(yellowText, YELLOW);
  } else if (event.id == greenSwitch && event.value == 0) {
    SimbleeForMobile.updateColor(greenText, BLACK);
  } else if (event.id == greenSwitch && event.value == 1) {
    SimbleeForMobile.updateColor(greenText, GREEN);
  } else if (event.id == cyanSwitch && event.value == 0) {
    SimbleeForMobile.updateColor(cyanText, BLACK);
  } else if (event.id == cyanSwitch && event.value == 1) {
    SimbleeForMobile.updateColor(cyanText, CYAN);
  } else if (event.id == segment) {
    char buffer[40];
    sprintf(buffer, "Segment: %d", event.value);
    SimbleeForMobile.updateText(segmentText, buffer);
    SimbleeForMobile.updateValue(slaveSegment, event.value);
  }
}

/**
 * Handle events which occur on the Slider and Stepper screen.
 * 
 * Handle returning to the home screen event. When a slider event occurs,
 * update the associated text to the slider's value.
 */
void handleSliderStepperScreenEvents(event_t &event)
{
  if (event.id == toHomeScreenButtonID && event.type == EVENT_RELEASE) {
    SimbleeForMobile.showScreen(HOME_SCREEN);
  } else if (event.id == xSlider && event.type == 0) {
    char myBuffer[30];
    sprintf(myBuffer, "%d", event.value);
    SimbleeForMobile.updateText(xSliderText, myBuffer);
  } else if (event.id == ySlider && event.type == 0) {
    char myBuffer[30];
    sprintf(myBuffer, "%d", event.value);
    SimbleeForMobile.updateText(ySliderText, myBuffer);
  } else if (event.id == stepper1) {
    char myBuffer[30];
    sprintf(myBuffer, "%d", event.value);
    SimbleeForMobile.updateText(stepper1Text, myBuffer);
  } else if (event.id == stepper2) {
    char myBuffer[30];
    sprintf(myBuffer, "%d", event.value);
    SimbleeForMobile.updateText(stepper2Text, myBuffer);
  }
}

/**
 * Handle events which occur on the TextField screen.
 * 
 * Handle returning to the home screen event.
 */
void handleTextFieldScreenEvents(event_t &event)
{
  printEvent(event);
  if (event.id == toHomeScreenButtonID && event.type == EVENT_RELEASE) {
    SimbleeForMobile.showScreen(HOME_SCREEN);
  }
}

/**
 * Handle events which occur on the Image screen.
 * 
 * Handle returning to the home screen event.
 */
void handleImageScreenEvents(event_t &event)
{
  if (event.id == toHomeScreenButtonID && event.type == EVENT_RELEASE) {
    SimbleeForMobile.showScreen(HOME_SCREEN);
  }
}

/**
 * Create the Home screen.
 *
 * This screen has a bunch of buttons which open subdisplays. Each subdisplay tests various
 * aspects of SimbleeUI
 */
void createHomeScreen()
{
  Serial.println("creating Home screen");
  
  //
  // Portrait mode is the default, so that isn't required
  // here, but shown for completeness. LANDSCAPE could be
  // used for that orientation.
  //
  SimbleeForMobile.beginScreen(YELLOW, PORTRAIT);

  Serial.printf("Screen size %d x %d\n", SimbleeForMobile.screenWidth, SimbleeForMobile.screenHeight);

  int titleLeftMargin = 50;
  int titleTopMargin = 60;
  int leftMargin = 20;
  int leftMarginButton = 240;
  int textSize = 14;
  int topMargin = 140;
  int yDelta = 50;
  int yButtonOffset = 10;
  int buttonWidth = 60;

  SimbleeForMobile.drawText(titleLeftMargin, titleTopMargin, "Home Screen", BLACK, TITLE_TEXT_SIZE);

  SimbleeForMobile.drawText(leftMargin, topMargin, "Test various text drawing APIs", BLACK, textSize);
  toTextTestingButtinID = SimbleeForMobile.drawButton(leftMarginButton, topMargin - yButtonOffset, buttonWidth, "Test");
  topMargin += yDelta;

  SimbleeForMobile.drawText(leftMargin, topMargin, "Test various button APIs", BLACK, textSize);
  toButtonTestingButtinID = SimbleeForMobile.drawButton(leftMarginButton, topMargin - yButtonOffset, buttonWidth, "Test");
  topMargin += yDelta;

  SimbleeForMobile.drawText(leftMargin, topMargin, "Test various rect APIs", BLACK, textSize);
  toRectTestingButtinID = SimbleeForMobile.drawButton(leftMarginButton, topMargin - yButtonOffset, buttonWidth, "Test");
  topMargin += yDelta;

  SimbleeForMobile.drawText(leftMargin, topMargin, "Test switch/segment APIs", BLACK, textSize);
  toSwitchTestingButtinID = SimbleeForMobile.drawButton(leftMarginButton, topMargin - yButtonOffset, buttonWidth, "Test");
  topMargin += yDelta;

  SimbleeForMobile.drawText(leftMargin, topMargin, "Test slider/stepper APIs", BLACK, textSize);
  toSliderTestingButtinID = SimbleeForMobile.drawButton(leftMarginButton, topMargin - yButtonOffset, buttonWidth, "Test");
  topMargin += yDelta;

  SimbleeForMobile.drawText(leftMargin, topMargin, "Test text field APIs", BLACK, textSize);
  toStepperTestingButtinID = SimbleeForMobile.drawButton(leftMarginButton, topMargin - yButtonOffset, buttonWidth, "Test");
  topMargin += yDelta;

  SimbleeForMobile.drawText(leftMargin, topMargin, "Test various image APIs", BLACK, textSize);
  toImageTestingButtinID = SimbleeForMobile.drawButton(leftMarginButton, topMargin - yButtonOffset, buttonWidth, "Test");
  topMargin += yDelta;

  //
  // Receive notifications when a "release" occurs on the button.
  // Other event notifications are supported and can be combined in this call.
  // I.e. to receive press and release events you'd pass:
  //
  // EVENT_PRESS | EVENT_RELEASE
  //
  // into the function:
  //
  // SimbleeForMobile.setEvents(toTextTestingButtinID, EVENT_PRESS | EVENT_RELEASE);
  //
  // However, you don't want to register for events which are not required as
  // that results in extra traffic.
  //
  SimbleeForMobile.setEvents(toTextTestingButtinID, EVENT_RELEASE);
  SimbleeForMobile.setEvents(toButtonTestingButtinID, EVENT_RELEASE);
  SimbleeForMobile.setEvents(toRectTestingButtinID, EVENT_RELEASE);
  SimbleeForMobile.setEvents(toSwitchTestingButtinID, EVENT_RELEASE);
  SimbleeForMobile.setEvents(toSliderTestingButtinID, EVENT_RELEASE);
  SimbleeForMobile.setEvents(toStepperTestingButtinID, EVENT_RELEASE);
  SimbleeForMobile.setEvents(toImageTestingButtinID, EVENT_RELEASE);
  SimbleeForMobile.endScreen();
  
  Serial.println("Home screen created");
}

/**
 * Create the text test screen.
 *
 * This screen displays multiple lines of text using the various text
 * creation calls supported by SimbleeForMoible
 */
void createTextScreen()
{
  Serial.println("creating Text screen");

  //
  // Default to Portrait orientation
  //
  SimbleeForMobile.beginScreen(MAGENTA);

  int titleLeftMargin = 50;
  int titleTopMargin = 60;

  int topMargin = 110;
  int deltaY = 20;

  int leftMargin = 20;

  SimbleeForMobile.drawText(titleLeftMargin, titleTopMargin, "Text Screen", BLACK, TITLE_TEXT_SIZE);

  SimbleeForMobile.drawText(leftMargin, topMargin, "Text with default size and color");
  topMargin += deltaY;

  SimbleeForMobile.drawText(leftMargin, topMargin, "Red text with default size", RED);
  topMargin += deltaY;

  SimbleeForMobile.drawText(leftMargin, topMargin, "Yellow text with default size", YELLOW);
  topMargin += deltaY;

  SimbleeForMobile.drawText(leftMargin, topMargin, "12pt Blue text", BLUE, 12);
  topMargin += deltaY;

  SimbleeForMobile.drawText(leftMargin, topMargin, "18pt White text", WHITE, 18);
  topMargin += deltaY;

  int textID = SimbleeForMobile.drawText(leftMargin, topMargin, "26pt Cyan text", CYAN, 26);
  SimbleeForMobile.setEvents(textID, EVENT_PRESS | EVENT_POSITION);
  topMargin += 30;

  textID = SimbleeForMobile.drawText(leftMargin, topMargin, "50pt Green text", GREEN, 50);
  SimbleeForMobile.setEvents(textID, EVENT_RELEASE);
  topMargin += 50;

  textID = SimbleeForMobile.drawText(leftMargin, topMargin, "70pt White", WHITE, 70);
  SimbleeForMobile.setEvents(textID, EVENT_PRESS | EVENT_RELEASE | EVENT_POSITION);
  topMargin += 80;

  int centerTextLeftMargin = 150;
  int rightTextLeftMargin = 220;

  SimbleeForMobile.drawText(leftMargin, topMargin, 1200);
  SimbleeForMobile.drawText(centerTextLeftMargin, topMargin, 80, BLUE);
  SimbleeForMobile.drawText(rightTextLeftMargin, topMargin, 333, RED);
  topMargin += deltaY;

  SimbleeForMobile.drawText(leftMargin, topMargin, 1200, YELLOW, 30);
  SimbleeForMobile.drawText(centerTextLeftMargin, topMargin, 80, BLUE, 30);
  SimbleeForMobile.drawText(rightTextLeftMargin, topMargin, 333, RED, 30);
  topMargin += 2 * deltaY;

  SimbleeForMobile.drawText(leftMargin, topMargin, 1200, YELLOW, 50);
  SimbleeForMobile.drawText(centerTextLeftMargin, topMargin, 80, BLUE, 50);
  SimbleeForMobile.drawText(rightTextLeftMargin, topMargin, 333, RED, 50);

  toHomeScreenButtonID = SimbleeForMobile.drawButton(HOME_BUTTON_LEFT_MARGIN, HOME_BUTTON_TOP_MARGIN, HOME_BUTTON_WIDTH, HOME_BUTTON_TEXT);

  SimbleeForMobile.setEvents(toHomeScreenButtonID, EVENT_RELEASE);
  SimbleeForMobile.endScreen();
  
  Serial.println("Text screen created");
}

/**
 * Create the button test screen.
 *
 * This screen displays multiple buttons using the various button
 * creation calls supported by SimbleeForMoible
 */
void createButtonScreen()
{
  Serial.println("creating Button screen");
  
  //
  // Default to Portrait orientation
  //
  SimbleeForMobile.beginScreen(GRAY);

  int titleLeftMargin = 45;
  int titleTopMargin = 60;

  int buttonLeftMargin = 40;
  int buttonTopMargin = 120;
  int buttonWidth = 120;
  int buttonYDelta = 60;

  SimbleeForMobile.drawText(titleLeftMargin, titleTopMargin, "Button Screen", BLACK, TITLE_TEXT_SIZE);

  int buttonID = SimbleeForMobile.drawButton(buttonLeftMargin, buttonTopMargin, buttonWidth, "Press");
  buttonTopMargin += buttonYDelta;
  SimbleeForMobile.setEvents(buttonID, EVENT_PRESS);

  buttonID = SimbleeForMobile.drawButton(buttonLeftMargin, buttonTopMargin, buttonWidth, "Release", BLUE);
  buttonTopMargin += buttonYDelta;
  SimbleeForMobile.setEvents(buttonID, EVENT_RELEASE);

  buttonID = SimbleeForMobile.drawButton(buttonLeftMargin, buttonTopMargin, buttonWidth, "Position", RED, BOX_TYPE);
  buttonTopMargin += buttonYDelta;
  SimbleeForMobile.setEvents(buttonID, EVENT_PRESS | EVENT_RELEASE | EVENT_POSITION);

  buttonID = SimbleeForMobile.drawButton(buttonLeftMargin, buttonTopMargin, buttonWidth, "Text Button", YELLOW, TEXT_TYPE);
  SimbleeForMobile.setEvents(buttonID, EVENT_PRESS | EVENT_RELEASE | EVENT_POSITION);
  buttonTopMargin += buttonYDelta;

  counterButtonID = SimbleeForMobile.drawButton(buttonLeftMargin, buttonTopMargin, buttonWidth, "Pressed: 0", GREEN);
  SimbleeForMobile.setEvents(counterButtonID, EVENT_RELEASE);

  toHomeScreenButtonID = SimbleeForMobile.drawButton(HOME_BUTTON_LEFT_MARGIN, HOME_BUTTON_TOP_MARGIN, HOME_BUTTON_WIDTH, HOME_BUTTON_TEXT);

  SimbleeForMobile.setEvents(toHomeScreenButtonID, EVENT_RELEASE);
  SimbleeForMobile.endScreen();
  
  Serial.println("Button screen created");
}

/**
 * Create the rectangle test screen.
 *
 * This screen displays multiple rectangles using the various rectangle
 * creation calls supported by SimbleeForMoible
 */
void createRectScreen()
{
  Serial.println("creating Rect screen");

  //
  // Default to Portrait orientation
  //
  SimbleeForMobile.beginScreen(CLEAR);

  int titleLeftMargin = 60;
  int titleTopMargin = 60;

  int leftMargin = 30;
  int rectLeftMargin = 150;
  int topMargin = 120;
  int deltaY = 50;
  int textSize = 18;
  int rect1Width = 270;
  int rect1Height = 50;
  int rect2Width = 50;
  int rect2Height = 300;
  int rect3Width = 50;
  int rect3Height = 300;
  int centerRectWidth = 70;
  int centerRectHeight = 70;

  int textID = SimbleeForMobile.drawText(titleLeftMargin, titleTopMargin, "Rect Screen", YELLOW, TITLE_TEXT_SIZE);

  // TODO: This color demonstraits the problem with iOS handling of alpha
  // SimbleeForMobile.drawRect(leftMargin, topMargin, rect1Width, rect1Height, 0xffff0000);
  SimbleeForMobile.drawRect(leftMargin, topMargin, rect1Width, rect1Height, RED);
  topMargin += rect1Height;

  SimbleeForMobile.drawRect(leftMargin, topMargin, rect2Width, rect2Height, WHITE, BLACK);
  SimbleeForMobile.drawRect(leftMargin + rect1Width - rect3Width, topMargin, rect3Width, rect3Height, BLACK, WHITE);

  topMargin += deltaY;
  dragRectX = leftMargin + rect2Width + centerRectWidth / 2;
  dragRectY = topMargin;
  dragRect = SimbleeForMobile.drawRect(dragRectX, dragRectY, centerRectWidth, centerRectHeight, YELLOW);

  toHomeScreenButtonID = SimbleeForMobile.drawButton(HOME_BUTTON_LEFT_MARGIN, HOME_BUTTON_TOP_MARGIN, HOME_BUTTON_WIDTH, HOME_BUTTON_TEXT);

  SimbleeForMobile.setEvents(toHomeScreenButtonID, EVENT_RELEASE);
  SimbleeForMobile.setEvents(dragRect, EVENT_DRAG | EVENT_POSITION | EVENT_PRESS | EVENT_RELEASE);
  SimbleeForMobile.endScreen();
  
  Serial.println("Rect screen created");
}

/**
 * Create the Switch and Rectangle test screen.
 *
 * This screen displays multiple switches and segment controls using the various switch
 * and segment creation calls supported by SimbleeForMoible
 */
void createSwitchSegmentScreen()
{
  Serial.println("creating Switch and Segment screen");

  //
  // Default to Portrait orientation
  //
  SimbleeForMobile.beginScreen(0xFF112244);

  int titleLeftMargin = 20;
  int titleTopMargin = 60;

  int leftMargin = 30;
  int switchLeftMargin = 150;
  int topMargin = 120;
  int deltaY = 50;
  int textSize = 20;

  int segmentWidth = 250;

  SimbleeForMobile.drawText(titleLeftMargin, titleTopMargin, "Switch/Segments", BLACK, TITLE_TEXT_SIZE);

  blackText = SimbleeForMobile.drawText(leftMargin, topMargin, "Hide me", BLACK, textSize);
  blackSwitch = SimbleeForMobile.drawSwitch(switchLeftMargin, topMargin);
  SimbleeForMobile.updateValue(blackSwitch, 1);
  topMargin += deltaY;

  yellowText = SimbleeForMobile.drawText(leftMargin, topMargin, "Yellow", BLACK, textSize);
  yellowSwitch = SimbleeForMobile.drawSwitch(switchLeftMargin, topMargin, YELLOW);
  topMargin += deltaY;

  greenText = SimbleeForMobile.drawText(leftMargin, topMargin, "Green", BLACK, textSize);
  greenSwitch = SimbleeForMobile.drawSwitch(switchLeftMargin, topMargin, GREEN);
  topMargin += deltaY;

  cyanText = SimbleeForMobile.drawText(leftMargin, topMargin, "Cyan", BLACK, textSize);
  cyanSwitch = SimbleeForMobile.drawSwitch(switchLeftMargin, topMargin, CYAN);
  topMargin += deltaY;

  char *titles[] = { "One", "Two", "Three", "Four" };
  segment = SimbleeForMobile.drawSegment(leftMargin, topMargin, segmentWidth, titles, countof(titles));
  SimbleeForMobile.updateValue(segment, 1);

  topMargin += deltaY;
  segmentText = SimbleeForMobile.drawText(leftMargin, topMargin, "Segment: ", BLACK, textSize);
  topMargin += 3 * deltaY / 4;
  char *numbers[] = { "1", "2", "3", "4" };
  slaveSegment = SimbleeForMobile.drawSegment(leftMargin, topMargin, segmentWidth, numbers, countof(numbers), WHITE);

  toHomeScreenButtonID = SimbleeForMobile.drawButton(HOME_BUTTON_LEFT_MARGIN, HOME_BUTTON_TOP_MARGIN, HOME_BUTTON_WIDTH, HOME_BUTTON_TEXT);

  SimbleeForMobile.setEvents(toHomeScreenButtonID, EVENT_RELEASE);
  SimbleeForMobile.endScreen();

  Serial.println("Switch and Segment screen created");
}

/**
 * Create the Slider and Stepper test screen.
 *
 * This screen displays multiple sliders and stepper controls using the various slider
 * and stepper creation calls supported by SimbleeForMoible
 */
void createSliderStepperScreen()
{
  Serial.println("creating Slider Stepper screen");

  int titleLeftMargin = 30;
  int titleTopMargin = 60;

  int leftMargin = 30;
  int topMargin = 120;
  int sliderLeftMargin = 50;
  int sliderWidth = 150;
  int deltaY = 50;
  int textSize = 20;
  int stepperWidth = 100;
  int stepperLeftMargin = 50;

  unsigned long backgroundColor = 0xffffffff;

  //
  // Default to Portrait orientation
  //
  SimbleeForMobile.beginScreen(backgroundColor);
  
  SimbleeForMobile.drawText(titleLeftMargin, titleTopMargin, "Slider/Stepper", BLACK, TITLE_TEXT_SIZE);

  SimbleeForMobile.drawText(leftMargin, topMargin, "X: ");
  xSlider = SimbleeForMobile.drawSlider(sliderLeftMargin, topMargin, sliderWidth, 0, 100);
  xSliderText = SimbleeForMobile.drawText(leftMargin + sliderLeftMargin + sliderWidth, topMargin, "0");
  topMargin += deltaY;

  SimbleeForMobile.drawText(leftMargin, topMargin, "Y: ");
  ySlider = SimbleeForMobile.drawSlider(sliderLeftMargin, topMargin, sliderWidth, 0, 100, YELLOW);
  ySliderText = SimbleeForMobile.drawText(leftMargin + sliderLeftMargin + sliderWidth, topMargin, "0");

  topMargin += 2 * deltaY;
  SimbleeForMobile.drawText(leftMargin, topMargin, "Stepper range (0 - 10)");
  topMargin += deltaY;
  stepper1 = SimbleeForMobile.drawStepper(leftMargin, topMargin, stepperWidth, 0, 10);
  SimbleeForMobile.updateValue(stepper1, 5);
  stepper1Text = SimbleeForMobile.drawText(leftMargin + stepperLeftMargin + stepperWidth, topMargin, "5");
  
  topMargin += deltaY;
  SimbleeForMobile.drawText(leftMargin, topMargin, "Stepper range (-10 - 10)");
  topMargin += deltaY;
  stepper2 = SimbleeForMobile.drawStepper(leftMargin, topMargin, stepperWidth, -10, 10, YELLOW);
  stepper2Text = SimbleeForMobile.drawText(leftMargin + stepperLeftMargin + stepperWidth, topMargin, "0");

  toHomeScreenButtonID = SimbleeForMobile.drawButton(HOME_BUTTON_LEFT_MARGIN, HOME_BUTTON_TOP_MARGIN, HOME_BUTTON_WIDTH, HOME_BUTTON_TEXT);

  SimbleeForMobile.setEvents(toHomeScreenButtonID, EVENT_RELEASE);
  SimbleeForMobile.endScreen();

  SimbleeForMobile.imageSource(YELLOW_DOT_IMAGE, YellowDot_Type, YellowDot, YellowDot_Len);

  Serial.println("Slider Stepper screen created");
}

/**
 * Create the TextField test screen.
 *
 * This screen displays multiple text field controls using the various text field
 * creation calls supported by SimbleeForMoible
 */
void createTextFieldScreen()
{
  Serial.println("creating Textfield screen");

  //
  // Default to Portrait orientation
  //
  SimbleeForMobile.beginScreen(WHITE);

  int titleLeftMargin = 20;
  int titleTopMargin = 60;

  int leftMargin = 30;
  int topMargin = 120;
  int deltaY = 40;
  int textfieldWidth = 220;

  SimbleeForMobile.drawText(titleLeftMargin, titleTopMargin, "Textfield Screen", BLACK, TITLE_TEXT_SIZE);

  SimbleeForMobile.drawTextField(leftMargin, topMargin, textfieldWidth, "");
  topMargin += deltaY;

  SimbleeForMobile.drawTextField(leftMargin, topMargin, textfieldWidth, "", "Enter Text Here");
  topMargin += deltaY;

  SimbleeForMobile.drawTextField(leftMargin, topMargin, textfieldWidth, "Green", "Green text", GREEN );
  topMargin += deltaY;

  SimbleeForMobile.drawTextField(leftMargin, topMargin, textfieldWidth, "Yellow", "Yellow text", YELLOW, BLUE );
  topMargin += deltaY;

  int zero = 0;
  SimbleeForMobile.drawTextField(leftMargin, topMargin, textfieldWidth, zero);
  topMargin += deltaY;

  SimbleeForMobile.drawTextField(leftMargin, topMargin, textfieldWidth, 34, "Thirty Four");
  topMargin += deltaY;

  SimbleeForMobile.drawTextField(leftMargin, topMargin, textfieldWidth, 45, "", RED);
  topMargin += deltaY;

  SimbleeForMobile.drawTextField(leftMargin, topMargin, textfieldWidth, 12, "Background", RED, YELLOW);

  toHomeScreenButtonID = SimbleeForMobile.drawButton(HOME_BUTTON_LEFT_MARGIN, HOME_BUTTON_TOP_MARGIN, HOME_BUTTON_WIDTH, HOME_BUTTON_TEXT);

  SimbleeForMobile.setEvents(toHomeScreenButtonID, EVENT_RELEASE);
  SimbleeForMobile.endScreen();
  
  Serial.println("Textfield screen created");
}

/**
 * Create the Image test screen.
 *
 * This screen displays multiple images using the image
 * creation and loading calls supported by SimbleeForMoible. The image
 * loading and placement on the UI functions are unique API calls such that
 * the images can be loaded before or after the screen is defined. Also, the
 * Z order of the images on the screen is defined by the drawImage calls
 * independant of the loading of the images using the imageSource call.
 */
void createImageScreen()
{
  Serial.println("creating Image screen");

  //
  // Default to Portrait orientation
  //
  SimbleeForMobile.beginScreen();

  int titleLeftMargin = 45;
  int titleTopMargin = 60;

  int leftMargin = 25;
  int topMargin = 120;
  int deltaY = 40;
  int repeatX = 7;
  int repeatY = 7;

  SimbleeForMobile.drawText(titleLeftMargin, titleTopMargin, "Image Screen", BLACK, TITLE_TEXT_SIZE);

  SimbleeForMobile.drawImage(YELLOW_DOT_IMAGE, leftMargin, topMargin);

  SimbleeForMobile.drawImage(YELLOW_DOT_IMAGE, 6 * leftMargin, topMargin);

  topMargin += deltaY;
  SimbleeForMobile.drawImage(RED_DOT_IMAGE, leftMargin, topMargin, repeatX, 1);

  topMargin += deltaY;
  SimbleeForMobile.drawImage(GREEN_DOT_IMAGE, leftMargin, topMargin, repeatX, repeatY);

  toHomeScreenButtonID = SimbleeForMobile.drawButton(HOME_BUTTON_LEFT_MARGIN, HOME_BUTTON_TOP_MARGIN, HOME_BUTTON_WIDTH, HOME_BUTTON_TEXT);

  SimbleeForMobile.setEvents(toHomeScreenButtonID, EVENT_RELEASE);

  //
  // Upload the images
  //
  SimbleeForMobile.imageSource(YELLOW_DOT_IMAGE, YellowDot_Type, YellowDot, YellowDot_Len);
  SimbleeForMobile.imageSource(RED_DOT_IMAGE, RedDot_Type, RedDot, RedDot_Len);
  SimbleeForMobile.imageSource(GREEN_DOT_IMAGE, GreenDot_Type, GreenDot, GreenDot_Len);
  
  SimbleeForMobile.endScreen();
  Serial.println("Image screen created");
}

/**
 * Utility method to update the text on the counter button.
 */
void updateCounterButtonText()
{
  char myBuffer[20];
  sprintf(myBuffer, "Pressed: %d", buttonPressCount);
  SimbleeForMobile.updateText(counterButtonID, myBuffer);
}

/*
  Utility method to print information regarding the given event
*/
void printEvent(event_t &event)
{
  char eventType[10];

  switch (event.type)
  {
    case EVENT_PRESS:
      strcpy(eventType, "Press");
      break;
    case EVENT_RELEASE:
      strcpy(eventType, "Release");
      break;
    case EVENT_DRAG:
      strcpy(eventType, "Drag");
      break;
    case EVENT_POSITION:
      strcpy(eventType, "Position");
      break;
    case EVENT_COLOR:
      strcpy(eventType, "Color");
      break;
    default:
      strcpy(eventType, "Unknown");
      break;
  }

  Serial.printf("Event ID: %d Type: %d (%s) Value: %d Text: %s Coords: %d,%d\n",
                event.id, event.type, eventType, event.value, event.text, event.x, event.y);
}

