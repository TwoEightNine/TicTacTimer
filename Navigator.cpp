#include "Navigator.h"

Navigator::Navigator()
    : current(nullptr)
{
    for (int i = 0; i < SCREEN_COUNT; ++i)
        screens[i] = nullptr;
}

void Navigator::registerScreen(ScreenId id, Screen* screen)
{
    screens[id] = screen;

    if (current == nullptr)
        current = screen;
}

void Navigator::open(ScreenId id) {
    if (screens[id] != nullptr)
        current = screens[id];
}

Screen* Navigator::currentScreen() const {
    return current;
}

void Navigator::tick() {
    if (current != nullptr)
        current->tick();
}