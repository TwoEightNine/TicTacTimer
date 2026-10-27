// SPDX-License-Identifier: GPL-3.0-or-later
#include <Fonts/FreeSansBold9pt7b.h>

#include "AboutScreen.h"
#include "../core/Navigator.h"

class AboutScreen : public Screen {
private:

    static constexpr int LINE_COUNT = 3;

    // Baselines; glyphs reach 12 px above and 5 px below them.
    static constexpr int FIRST_BASELINE = 15;
    static constexpr int LINE_GAP = 21;

    const char* lines[LINE_COUNT] = {
        "github.com/",
        "TwoEightNine",
        "/TicTacTimer",
    };

    bool isDrawn = false;

public:

    AboutScreen(
        Adafruit_SSD1306& display,
        ToneController& tone,
        ButtonController& button,
        Navigator* navigator
    ) : Screen(display, tone, button, navigator) {
    }

    void tick() override {

        ButtonController::Event event = button.getEvent();

        if (event == ButtonController::LONG_CLICK) {
            isDrawn = false;
            navigator->open(Navigator::MENU);
            return;
        }

        // The content is static, so draw it once per visit.
        if (!isDrawn) {
            draw();
            isDrawn = true;
        }
    }

private:

    void draw() {

        display.clearDisplay();
        display.setFont(&FreeSansBold9pt7b);
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);

        for (int i = 0; i < LINE_COUNT; i++) {

            int16_t x1;
            int16_t y1;
            uint16_t width;
            uint16_t height;
            display.getTextBounds(lines[i], 0, 0, &x1, &y1, &width, &height);

            display.setCursor((display.width() - width) / 2 - x1, FIRST_BASELINE + i * LINE_GAP);
            display.print(lines[i]);
        }

        display.display();

        // The display is shared, so restore the default font for other screens.
        display.setFont();
    }
};

Screen* createAboutScreen(
    Adafruit_SSD1306& display,
    ToneController& tone,
    ButtonController& button,
    Navigator* navigator
) {
    return new AboutScreen(display, tone, button, navigator);
}
