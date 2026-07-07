#pragma once

#include "Screen.h"

class Navigator {
public:
    enum ScreenId {
        WORKOUT,
        MENU,
        SPLASH,
        FLAPPY_BIRD,
        GALLERY,
        CLICK_RUSH,

        SCREEN_COUNT
    };

    Navigator();

    void registerScreen(ScreenId id, Screen* screen);

    void open(ScreenId id);

    Screen* currentScreen() const;

    void tick();

private:
    Screen* screens[SCREEN_COUNT];
    Screen* current;
};