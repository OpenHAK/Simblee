/*
 * Copyright (c) 2015 RF Digital Corp. All Rights Reserved.
 *
 * The source code contained in this file and all intellectual property embodied in
 * or covering the source code is the property of RF Digital Corp. or its licensors.
 * Your right to use this source code and intellectual property is non-transferable,
 * non-sub licensable, revocable, and subject to terms and conditions of the
 * SIMBLEE SOFTWARE LICENSE AGREEMENT.
 * http://www.simblee.com/licenses/SimbleeSoftwareLicenseAgreement.txt
 *
 * THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
 *
 * This heading must NOT be removed from this file.
 */

#ifndef SimbleeForMobile_h
#define SimbleeForMobile_h

#include <stdint.h>
#include <stdbool.h>

#define  countof(x)  (sizeof(x)/sizeof(x[0]))

typedef uint32_t color_t;

// predefined colors
#define  BLACK    ((color_t)0x000000)
#define  RED      ((color_t)0xFF0000)
#define  GREEN    ((color_t)0x00FF00)
#define  BLUE     ((color_t)0x0000FF)
#define  YELLOW   ((color_t)0xFFFF00)
#define  MAGENTA  ((color_t)0xFF00FF)
#define  CYAN     ((color_t)0x00FFFF)
#define  WHITE    ((color_t)0xFFFFFF)
#define  GRAY     ((color_t)0x808080)
#define  CLEAR    ((color_t)0xFF000000)

// image types
#define  JPG  0
#define  PNG  1
#define  TIF  2
#define  BMP  3
#define  ICO  4
#define  CUR  5
#define  XBM  6

// shipped resources
#define COLOR_WHEEL -1

// button types
#define  BOX_TYPE   0
#define  TEXT_TYPE  1

// events
#define  EVENT_PRESS      1
#define  EVENT_RELEASE    2
#define  EVENT_DRAG       4
#define  EVENT_POSITION   8
#define  EVENT_COLOR     16

//
// Bit fields for supported orientation of the UI
// on the remote device.
//
#define PORTRAIT 1 << 0
#define LANDSCAPE 1 << 1

//
// Remote device OD
//
#define REMOTE_DEVICE_OS_UNKNOWN 0
#define REMOTE_DEVICE_OS_IOS 1
#define REMOTE_DEVICE_OS_ANDROID 2

typedef struct
{
  uint8_t id;
  uint8_t type;
  int16_t value;
  const char *text;
  uint16_t x;
  uint16_t y;
  uint8_t blue;
  uint8_t green;
  uint8_t red;
  uint8_t alpha;
} event_t;

// sockets
// the arduino ethernet library (based on WIN1500) declares SOCKET as uint8_t
// the wifi module (based on WINC1500) declares SOCKET as sint8
// SOCKET should really be declared in a common arduino core file, so lets
// just stay clear of the issue now, and redefine the SimbleeForMobile
// constants with a SFM_ prefix so that SimbleeForMobile can be used with
// other modules as well
#define  SFM_MAX_SOCK_NUM  4

typedef uint8_t SFM_SOCKET;

extern void ui();
extern void ui_event(event_t &event) __attribute__((weak));

color_t rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
color_t rgb(uint8_t red, uint8_t green, uint8_t blue);

extern void SimbleeForMobile_onAdvertisement(bool start) __attribute__((weak));
extern void SimbleeForMobile_onRSSI(int rssi)            __attribute__((weak));
extern void SimbleeForMobile_onConnect(void)             __attribute__((weak));
extern void SimbleeForMobile_onDisconnect(void)          __attribute__((weak));
extern bool SimbleeForMobile_onSignal(int number, char *data, int len)  __attribute__((weak));

class SimbleeForMobileClass
{
public:
  // colorwheel.simblee.com
  const char *domain;

  // Oct 13 2014 18:19:07
  const char *baseline;

public:
  uint16_t screenWidth;
  uint16_t screenHeight;
  uint8_t remoteDeviceType;

  bool internetAccessEnabled;

public:
  // screen = 1-7
  uint8_t screen;

  bool connected;
  bool active;
  bool updatable;

public:
  SimbleeForMobileClass();

  void begin(const char *datetime = __DATE__ " " __TIME__, const char *file = __PROJECT__);
  // process must be called in the loop for SimbleeForMobile
  void process();
  void end();

  void showScreen(uint8_t screen);

  void beginScreen(color_t background_color = WHITE, uint8_t orientation = PORTRAIT);

  uint8_t drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, color_t color);
  uint8_t drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, color_t color, color_t color2);

  uint8_t drawText(uint16_t x, uint16_t y, const char *title);
  uint8_t drawText(uint16_t x, uint16_t y, const char *title, color_t color);
  uint8_t drawText(uint16_t x, uint16_t y, const char *title, color_t color, uint8_t size);
  uint8_t drawText(uint16_t x, uint16_t y, int16_t value);
  uint8_t drawText(uint16_t x, uint16_t y, int16_t value, color_t color);
  uint8_t drawText(uint16_t x, uint16_t y, int16_t value, color_t color, uint8_t size);

  // image_num (shipped resources = -31 to -1, user resources = 1 to 31)
  uint8_t imageSource(int8_t image_num, uint8_t image_type, const uint8_t *image, uint16_t len);

  uint8_t drawImage(int8_t image_num, uint16_t x, uint16_t y);
  uint8_t drawImage(int8_t num, uint16_t x, uint16_t y, uint16_t repeatx, uint16_t repeaty);

  // type: 0=boxed button, 1=ios7 text button
  uint8_t drawButton(uint16_t x, uint16_t y, uint16_t w, const char *title);
  uint8_t drawButton(uint16_t x, uint16_t y, uint16_t w, const char *title, color_t color);
  uint8_t drawButton(uint16_t x, uint16_t y, uint16_t w, const char *title, color_t color, uint8_t type);

  uint8_t drawSwitch(uint16_t x, uint16_t y);
  uint8_t drawSwitch(uint16_t x, uint16_t y, color_t color);

  uint8_t drawSegment(uint16_t x, uint16_t y, uint16_t w, const char * const *title, uint8_t count);
  uint8_t drawSegment(uint16_t x, uint16_t y, uint16_t w, const char * const *title, uint8_t count, color_t color);

  uint8_t drawSlider(uint16_t x, uint16_t y, uint16_t w, int16_t min, int16_t max);
  uint8_t drawSlider(uint16_t x, uint16_t y, uint16_t w, int16_t min, int16_t max, color_t color);

  uint8_t drawStepper(uint16_t x, uint16_t y, uint16_t w, int16_t min, int16_t max);
  uint8_t drawStepper(uint16_t x, uint16_t y, uint16_t w, int16_t min, int16_t max, color_t color);

  uint8_t drawTextField(uint16_t x, uint16_t y, uint16_t w, const char *text);
  uint8_t drawTextField(uint16_t x, uint16_t y, uint16_t w, const char *text, const char *placeholder);
  uint8_t drawTextField(uint16_t x, uint16_t y, uint16_t w, const char *text, const char *placeholder, color_t color);
  uint8_t drawTextField(uint16_t x, uint16_t y, uint16_t w, const char *text, const char *placeholder, color_t color, color_t background);
  uint8_t drawTextField(uint16_t x, uint16_t y, uint16_t w, int16_t value);
  uint8_t drawTextField(uint16_t x, uint16_t y, uint16_t w, int16_t value, const char *placeholder);
  uint8_t drawTextField(uint16_t x, uint16_t y, uint16_t w, int16_t value, const char *placeholder, color_t color);
  uint8_t drawTextField(uint16_t x, uint16_t y, uint16_t w, int16_t value, const char *placeholder, color_t color, color_t background);

  void updateX(uint8_t id, uint16_t x);
  void updateY(uint8_t id, uint16_t y);
  void updateW(uint8_t id, uint16_t w);
  void updateH(uint8_t id, uint16_t h);
  void updateOrigin(uint8_t id, uint16_t x, uint16_t y);
  void updateSize(uint8_t id, uint16_t w, uint16_t h);
  void updateRect(uint8_t id, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
  void updateColor(uint8_t id, color_t color);
  void updateColor2(uint8_t id, color_t color2);

  void updateValue(uint8_t id, int16_t value);
  void updateText(uint8_t id, const char *text);

  void setEnabled(uint8_t id, bool enabled);
  void setVisible(uint8_t id, bool visible);
  void setEvents(uint8_t id, uint8_t events);

  void endScreen();


// SimbleeBLE methods
public:
  const char *deviceName;
  const char *advertisementData;

  //
  // Device Information Service
  //
  const char *manufacturerName;
  const char *modelNumber;
  const char *serialNumber;
  const char *hardwareRevision;
  const char *softwareRevision;

  int advertisementInterval;
  int txPowerLevel;
  static volatile int& radioActive;
  int getConnInterval(void);
  void updateConnInterval(int min_conn_interval_ms, int max_conn_interval_ms);

// SimbleeForMobileClient methods
public:
  bool socketUsed(SFM_SOCKET s);
  bool socketConnected(SFM_SOCKET s);
  int socketAvailable(SFM_SOCKET s);
  bool socketIsAvailable(SFM_SOCKET s, int avail);
  uint8_t socketPeek(SFM_SOCKET s);
  int socketError(SFM_SOCKET s);

  bool socketConnectv4(SFM_SOCKET s, const uint32_t ip, uint16_t port);
  bool socketConnect(SFM_SOCKET s, const char *host, uint16_t port);
  void socketSend(SFM_SOCKET s, const uint8_t *buf, uint16_t len);
  int socketReceive(SFM_SOCKET s, uint8_t *buf, uint16_t len);
  void socketClose(SFM_SOCKET s);
};

extern SimbleeForMobileClass SimbleeForMobile;

#endif  // SimbleeForMobile_h

