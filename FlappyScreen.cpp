#include "FlappyScreen.h"
#include "Navigator.h"

class FlappyBirdScreen : public Screen {
private:

    static constexpr int SCREEN_W = 128;
    static constexpr int SCREEN_H = 64;

    static constexpr int BIRD_X = 24;
    static constexpr int BIRD_SIZE = 4;

    static constexpr int PIPE_WIDTH = 14;
    static constexpr int GAP_HEIGHT = 22;
    static constexpr int PIPE_SPEED = 2;

    enum State {
        WAITING,
        PLAYING,
        GAME_OVER
    };

    State state = WAITING;

    int birdY = SCREEN_H / 2;
    int velocity = 0;

    int pipeX = SCREEN_W;
    int gapY = 18;

    int score = 0;
    bool scored = false;

public:

    FlappyBirdScreen(
        Adafruit_SSD1306& display,
        ToneController& tone,
        ButtonController& button,
        Navigator* navigator
    ) : Screen(display, tone, button, navigator)
    {
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
                    startGame();
                }
                drawWaiting();
                break;

            case PLAYING:
                if (event == ButtonController::SHORT_CLICK) {
                    velocity = -5;
                    // tone.tone(1500, 20);
                }
                updateGame();
                drawGame();
                break;

            case GAME_OVER:
                if (event == ButtonController::SHORT_CLICK) {
                    startGame();
                }
                if (event == ButtonController::LONG_CLICK) {
                    navigator->open(Navigator::MENU);
                }
                drawGameOver();
                break;
        }

        delay(30);
    }

private:

    void startGame() {
        state = PLAYING;

        birdY = SCREEN_H / 2;
        velocity = 0;

        pipeX = SCREEN_W;
        gapY = random(10, SCREEN_H - GAP_HEIGHT - 10);

        score = 0;
        scored = false;
    }

    void updateGame() {
        velocity += 1;
        if (velocity > 4)
            velocity = 4;

        birdY += velocity;

        pipeX -= PIPE_SPEED;

        if (pipeX + PIPE_WIDTH < 0) {

            pipeX = SCREEN_W;
            gapY = random(8, SCREEN_H - GAP_HEIGHT - 8);
            scored = false;
        }

        if (!scored && pipeX + PIPE_WIDTH < BIRD_X) {
            score++;
            scored = true;
            // tone.tone(2000, 30);
        }

        if (birdY < 0 || birdY + BIRD_SIZE >= SCREEN_H) {
            gameOver();
            return;
        }

        if (BIRD_X + BIRD_SIZE >= pipeX &&
            BIRD_X <= pipeX + PIPE_WIDTH) {

            if (birdY < gapY ||
                birdY + BIRD_SIZE > gapY + GAP_HEIGHT) {

                gameOver();
            }
        }
    }

    void gameOver() {
        // tone.tone(400, 250);
        state = GAME_OVER;
    }

    void drawGame() {
        display.clearDisplay();
        display.fillRect(
            BIRD_X,
            birdY,
            BIRD_SIZE,
            BIRD_SIZE,
            SSD1306_WHITE
        );

        display.fillRect(
            pipeX,
            0,
            PIPE_WIDTH,
            gapY,
            SSD1306_WHITE
        );

        display.fillRect(
            pipeX,
            gapY + GAP_HEIGHT,
            PIPE_WIDTH,
            SCREEN_H - gapY - GAP_HEIGHT,
            SSD1306_WHITE
        );

        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print(score);

        display.display();
    }

    void drawWaiting() {
        display.clearDisplay();

        display.setTextSize(2);
        display.setCursor(20, 10);
        display.println(F("FLAPPY"));

        display.setTextSize(1);
        display.setCursor(14, 42);
        display.println(F("Short: Start"));

        display.display();
    }

    void drawGameOver() {
        // drawGame();

        display.fillRect(18, 18, 92, 28, SSD1306_BLACK);
        display.drawRect(18, 18, 92, 28, SSD1306_WHITE);

        display.setTextSize(1);
        display.setCursor(32, 24);
        display.println(F("GAME OVER"));

        display.setCursor(42, 36);
        display.print(score);

        display.display();
    }
};

Screen* createFlappyBirdScreen(
    Adafruit_SSD1306& display,
    ToneController& tone,
    ButtonController& button,
    Navigator* navigator
) {
    return new FlappyBirdScreen(
        display,
        tone,
        button,
        navigator
    );
}