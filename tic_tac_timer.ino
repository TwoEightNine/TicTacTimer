#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Navigator.h"
#include "WorkoutScreen.h"
#include "MenuScreen.h"
#include "SplashScreen.h"
#include "FlappyScreen.h"
#include "GalleryScreen.h"
#include "ClickRushScreen.h"
#include "ButtonController.h"
#include "ToneController.h"
#include "BuzzerToneController.h"

#define PIN_BTN 7
#define PIN_BUZZER 21

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BuzzerToneController buzzer(PIN_BUZZER);
ButtonController button(PIN_BTN, buzzer);

Navigator navigator;

Screen* workoutScreen = createWorkoutScreen(display, buzzer, button, &navigator);
Screen* menuScreen = createMenuScreen(display, buzzer, button, &navigator);
Screen* splashScreen = createSplashScreen(display, buzzer, button, &navigator);
Screen* flappyScreen = createFlappyBirdScreen(display, buzzer, button, &navigator);
Screen* galleryScreen = createGalleryScreen(display, buzzer, button, &navigator);
Screen* clickRushScreen = createClickRushScreen(display, buzzer, button, &navigator);

void setup() {
  Serial.begin(115200);
  while (!Serial) ;
  Serial.println(F("Setup started"));

  // wait for display, from example
  delay(500);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setRotation(0);
  display.setTextColor(SSD1306_WHITE); 

  button.begin();
  buzzer.begin();

  navigator.registerScreen(Navigator::SPLASH, splashScreen);
  navigator.registerScreen(Navigator::MENU, menuScreen);
  navigator.registerScreen(Navigator::WORKOUT, workoutScreen);
  navigator.registerScreen(Navigator::FLAPPY_BIRD, flappyScreen);
  navigator.registerScreen(Navigator::GALLERY, galleryScreen);
  navigator.registerScreen(Navigator::CLICK_RUSH, clickRushScreen);
}

void loop() {
  button.tick();
  navigator.tick();
  delay(5);
}

