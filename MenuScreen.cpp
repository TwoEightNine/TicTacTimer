#include "MenuScreen.h"
#include "Navigator.h"

class MenuScreen : public Screen {
private:

    enum MenuItem {
        ITEM_WORKOUT,
        ITEM_FLAPPY_BIRD,
        ITEM_CLICK_RUSH,
        ITEM_ABOUT,
        ITEM_COUNT
    };

    static constexpr int VISIBLE_ITEMS = 3;

    const char* names[ITEM_COUNT] = {
        "Workout",
        "Flappy",
        "Click rush",
        "About",
    };

    MenuItem selected = ITEM_WORKOUT;
    int page = 0;

public:

    MenuScreen(
        Adafruit_SSD1306& display,
        ToneController& tone,
        ButtonController& button,
        Navigator* navigator
    )
        : Screen(display, tone, button, navigator) {
    }

    void tick() override {

        ButtonController::Event event = button.getEvent();

        if (event == ButtonController::SHORT_CLICK) {
            // tone.tone(1600, 30);
            if (selected == ITEM_COUNT - 1) {
                // Wrap to the beginning.
                selected = ITEM_WORKOUT;
                page = 0;
            } else {
                selected = static_cast<MenuItem>(selected + 1);
                page = selected / VISIBLE_ITEMS;
            }
        }

        if (event == ButtonController::LONG_CLICK) {
            // tone.tone(2200, 60);
            switch (selected) {

                case ITEM_WORKOUT:
                    navigator->open(Navigator::WORKOUT);
                    break;

                case ITEM_FLAPPY_BIRD:
                    navigator->open(Navigator::FLAPPY_BIRD);
                    break;

                case ITEM_CLICK_RUSH:
                    navigator->open(Navigator::CLICK_RUSH);
                    break;

                case ITEM_ABOUT:
                    // navigator->open(Navigator::ABOUT);
                    break;

                default:
                    break;
            }
        }

        if (event == ButtonController::EXTRA_LONG_CLICK) {
            if (selected == ITEM_ABOUT) {
                navigator->open(Navigator::GALLERY);
            }
        }

        draw();
    }

private:

    void draw() {

        display.clearDisplay();

        display.setTextSize(2);

        int first = page * VISIBLE_ITEMS;
        int count = min(VISIBLE_ITEMS, ITEM_COUNT - first);

        // Evenly distribute the visible items vertically.
        int topMargin;
        int gap;

        switch (count) {

            case 1:
                topMargin = 24;
                gap = 0;
                break;

            case 2:
                topMargin = 10;
                gap = 24;
                break;

            default: // 3 items
                topMargin = 2;
                gap = 20;
                break;
        }

        for (int i = 0; i < count; i++) {

            int index = first + i;
            bool isSelected = (index == selected);

            int y = topMargin + i * gap;

            drawItem(index, y, isSelected);
        }

        display.display();
    }

    void drawItem(int index, int y, bool selectedItem) {

        if (selectedItem) {

            display.fillRect(0, y - 1, 128, 17, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);

        } else {

            display.setTextColor(SSD1306_WHITE);
        }

        display.setCursor(2, y);
        display.print(names[index]);

        display.setTextColor(SSD1306_WHITE);
    }
};

Screen* createMenuScreen(
    Adafruit_SSD1306& display,
    ToneController& tone,
    ButtonController& button,
    Navigator* navigator
) {
    return new MenuScreen(display, tone, button, navigator);
}