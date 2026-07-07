#include "GalleryScreen.h"
#include "Navigator.h"
// #include "n_pics.h"
// #include "neo.h"


class GalleryScreen : public Screen {
public:

    GalleryScreen(Adafruit_SSD1306& display, ToneController& tone, ButtonController& button, Navigator* navigator) : Screen(display, tone, button, navigator) {
        isDirty = true;
        animationIndex = 0;
        frameIndex = 0;
        lastFrameDrawn = 0;
    }

    void tick() override {
      ButtonController::Event event = button.getEvent();
      if (event == ButtonController::SHORT_CLICK) {
        animationIndex++;
        frameIndex = 0;
        if (animationIndex == ANIMATIONS_COUNT) {
            animationIndex = 0;
            isDirty = true;
        }
      }
      if (event == ButtonController::LONG_CLICK) {
          navigator->open(Navigator::MENU);
          animationIndex = 0;
          frameIndex = 0;
          isDirty = true;
          lastFrameDrawn = 0;
          return;
      }

        const Animation& animation = animations[animationIndex];
        uint16_t framePassed = millis() - lastFrameDrawn;
        if (framePassed >= animation.frame_duration_ms) {
            frameIndex++;
            if (frameIndex == animation.frame_count) {
                frameIndex = 0;
            }
            isDirty = true;
        }
        if (!isDirty) return;
        
        drawFrameFromProgmem(animation, frameIndex);
        lastFrameDrawn = millis();
        isDirty = false;
    }
  
private:
    bool isDirty = true;
    int animationIndex = 0;
    int frameIndex = 0;
    unsigned long lastFrameDrawn = 0;

    void drawFrameFromProgmem(const Animation& animation, int frameIndex) {
      if (frameIndex < 0 || frameIndex >= animation.frame_count) {
        return;
      }

      uint8_t* displayBuffer = display.getBuffer();

      memcpy_P(
        displayBuffer,
        animation.frames[frameIndex],
        ANIMATION_FRAME_SIZE
      );

      display.display();
    }
};

Screen* createGalleryScreen(Adafruit_SSD1306& display, ToneController& tone, ButtonController& button, Navigator* navigator) {
  return new GalleryScreen(display, tone, button, navigator);
}