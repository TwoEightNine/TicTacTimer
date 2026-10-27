// SPDX-License-Identifier: GPL-3.0-or-later
#include "SplashScreen.h"
#include "../core/Navigator.h"
#include "../assets/neo.h"

class SplashScreen : public Screen {
public:

    SplashScreen(Adafruit_SSD1306& display, ToneController& tone, ButtonController& button, Navigator* navigator) : Screen(display, tone, button, navigator) {
    }

    void tick() override {
        for (int frameIndex = 0; frameIndex < NEO_FRAME_COUNT; frameIndex++) {
          uint8_t* displayBuffer = display.getBuffer();
          memcpy_P(
            displayBuffer,
            neo_frames[frameIndex],
            ANIMATION_FRAME_SIZE
          );
          display.display();
          delay(60);
        }
        navigator->open(Navigator::MENU);
    }
};

Screen* createSplashScreen(Adafruit_SSD1306& display, ToneController& tone, ButtonController& button, Navigator* navigator) {
  return new SplashScreen(display, tone, button, navigator);
}