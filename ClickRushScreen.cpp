#include "ClickRushScreen.h"
#include "Navigator.h"
#include "ClickRushPic.h"

class ClickRushScreen : public Screen {
private:

    static constexpr int MAX_SCORE = 18;

    enum State {
        WAITING,
        PLAYING,
        GAME_OVER
    };

    State state = WAITING;

    int score = 0;
    int bestScore = 0;
    bool picMode = false;

    unsigned long lastDecay = 0;

public:

    ClickRushScreen(
        Adafruit_SSD1306& display,
        ToneController& tone,
        ButtonController& button,
        Navigator* navigator
    ) : Screen(display, tone, button, navigator) {
    }

    void tick() override {

        ButtonController::Event event = button.getEvent();

        if (event == ButtonController::LONG_CLICK) {
            navigator->open(Navigator::MENU);
            return;
        }

        switch (state) {

            case WAITING:

                if (event == ButtonController::SHORT_CLICK) {
                    picMode = false;
                    startGame();
                }
                if (event == ButtonController::EXTRA_LONG_CLICK) {
                    picMode = true;
                    startGame();
                }

                drawWaiting();
                break;

            case PLAYING:

                if (event == ButtonController::SHORT_CLICK) {

                    if (score < MAX_SCORE)
                        score++;

                    // tone.tone(1800, 8);
                }

                updateDecay();

                if (score == 0) {
                    state = GAME_OVER;
                    // tone.tone(300, 250);
                }
                bestScore = max(bestScore, score);
                drawGame();
                break;

            case GAME_OVER:

                if (event == ButtonController::SHORT_CLICK) {
                    startGame();
                }
                if (event == ButtonController::EXTRA_LONG_CLICK) {
                  picMode = !picMode;
                  startGame();
                }

                drawGameOver();
                break;
        }
    }

private:

    void startGame() {

        score = 1;
        lastDecay = millis();
        state = PLAYING;
    }

    void updateDecay() {

        unsigned long now = millis();

        float progress = (float)score / MAX_SCORE;

        // Non-linear:
        // 600ms at score=0
        // ~100ms near MAX_SCORE

        float t = 1.0f - progress;

        unsigned long interval =
            100 + (unsigned long)(500.0f * (0.6f * t + 0.4f * t * t));

        if (interval < 100)
            interval = 100;

        if (now - lastDecay >= interval) {

            lastDecay = now;

            if (score > 0)
                score--;
        }
    }

    void drawWaiting() {

        display.clearDisplay();

        display.setTextSize(2);
        display.setCursor(12, 10);
        display.print(F("CLICK!"));

        display.setTextSize(1);
        display.setCursor(18, 40);
        display.print(F("Short to start"));

        display.display();
    }

    void drawGame() {
        if (picMode) {
          drawFrameFromProgmem(score / 2);
        } else {
          display.clearDisplay();

          display.setTextSize(2);
          display.setCursor(0, 0);
          display.print(score);

          display.setTextSize(1);
          display.setCursor(48, 6);
          display.print("/");
          display.print(MAX_SCORE);

          int barWidth = map(score, 0, MAX_SCORE, 0, 124);

          display.drawRect(2, 46, 124, 12, SSD1306_WHITE);
          display.fillRect(3, 47, barWidth, 10, SSD1306_WHITE);

          display.display();
        }
    }

    void drawGameOver() {

        display.clearDisplay();

        display.setTextSize(2);
        display.setCursor(12, 0);
        display.print(F("LOSE"));

        display.setTextSize(1);

        display.setCursor(8, 28);
        display.print(F("Best: "));
        display.print(bestScore);

        display.setCursor(8, 44);
        display.print(F("Short: retry"));

        display.display();
    }

    void drawFrameFromProgmem(int frameIndex) {
      if (frameIndex < 0 || frameIndex >= CRP_FRAME_COUNT) {
        return;
      }

      uint8_t* displayBuffer = display.getBuffer();

      memcpy_P(
        displayBuffer,
        crp_frames[frameIndex],
        ANIMATION_FRAME_SIZE
      );

      display.display();
    }
};

Screen* createClickRushScreen(
    Adafruit_SSD1306& display,
    ToneController& tone,
    ButtonController& button,
    Navigator* navigator
) {
    return new ClickRushScreen(
        display,
        tone,
        button,
        navigator
    );
}