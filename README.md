# Tic Tac Timer

## What it is

This is a workout timer I made for myself. I found it more convenient to program the time intervals I need and to have sound indication.

Its initial purpose defined its hardware: monochrome 128x64 display, one button, buzzer.

## Current features

 - Workout timer;
 - Flappy bird game;
 - Click rush game.

## Later improvements

I found it interesting to implement screen-based architecture and vibe-code a lot of other useful or fun functions. One button is not as limiting as it might seem.

## Why Tic Tac?

First version was fit in a Tic Tac box. Although now it is larger than a Tic Tac box and doesn't have a body, it's still the same idea. 

# Project Architecture Reference (ESP32-C3 UI Framework)

This project is an Arduino/C++ application for ESP32-C3 with an SSD1306 OLED. The UI is implemented as a collection of independent screens managed by a central navigator.

The following conventions must be followed when generating new screens or modifying existing ones.

---

# Screen Base Class

Every screen derives from Screen.
```cpp
class Screen {
public:
    explicit Screen(
        Adafruit_SSD1306& display,
        ToneController& tone,
        ButtonController& button,
        Navigator* navigator
    );

    virtual ~Screen() = default;

    virtual void tick() = 0;

protected:
    Adafruit_SSD1306& display;
    ToneController& tone;
    ButtonController& button;
    Navigator* navigator;
};
```

Rules:

* `tick()` is called continuously from the main loop.
* Every screen performs all updates inside `tick()`.
* Screens own no display object; they use the reference provided by Screen.
* Screens should never block execution.
* Do not use `delay()` except for tiny visual effects if absolutely unavoidable.
* Prefer `millis()` for timing.
* All drawing is done directly on display.
* Every frame should end with `display.display();`

---

# Screen Factory Pattern

Every screen consists of exactly two files.

Example:

```
MenuScreen.h
MenuScreen.cpp
```

The header contains only:
```c++
#pragma once

#include "Screen.h"

Screen* createMenuScreen(
    Adafruit_SSD1306& display,
    ToneController& tone,
    ButtonController& button,
    Navigator* navigator
);
```

The cpp defines an internal class:

```c++
class MenuScreen : public Screen
{
    ...
};
```

The class is NOT declared in the header.

At the end:

```c++
Screen* createMenuScreen(...)
{
    return new MenuScreen(...);
}
```

This factory pattern is used for every screen.

---

# Navigator

Navigator owns every screen.

Responsibilities:

* stores pointers to all screens
* remembers current screen
* calls `currentScreen->tick();` every loop

Navigation happens only through

```c++
navigator->open(Navigator::MENU);
```

or similar.

Navigator exposes an enum:

```c++
Navigator::MENU
Navigator::SPLASH
Navigator::GAME
...
```

Screens never create or destroy other screens.

Screens only request navigation.

---

# ButtonController

ButtonController is non-blocking.

The application calls `button.tick();` every loop.

Screens obtain events only by

```c++
ButtonController::Event event = button.getEvent();
```

Supported events:

```c++
NONE
SHORT_CLICK
LONG_CLICK
EXTRA_LONG_CLICK
```

`getEvent()` consumes the event.

Typical usage:

```c++
ButtonController::Event event = button.getEvent();

if (event == ButtonController::SHORT_CLICK)
{
    ...
}

if (event == ButtonController::LONG_CLICK)
{
    ...
}
```

Never use digitalRead() directly inside screens.

---

# ToneController

Screens never manipulate buzzers directly.

Instead they call `tone.tone(Tone, Durarion);`

There are two implementations:
```c++
BuzzerToneController
NoopToneController
```

Both implement the same interface.

Noop version ignores requests.

---

# Display

Display type: `Adafruit_SSD1306`

Graphics library: `Adafruit_GFX`

Typical drawing:
```c++
display.clearDisplay();

display.setTextSize(2);
display.setCursor(x, y);
display.print(...);

display.drawRect(...);
display.fillRect(...);
display.drawLine(...);

display.display();
```

Coordinate system:

* origin: top-left
* width: 128
* height: 64

---

# UI Conventions

The UI uses monochrome graphics.

Avoid unnecessary screen flickering.

Prefer integer arithmetic.

Avoid heap allocations inside `tick()`.

Avoid String where possible.

Prefer `const char*` or flash strings `F("text")`

Game loops should use `millis()`.

Animations should be frame-independent whenever possible.

---

# Coding Style

Use:

* braces on separate lines
* explicit constructors
* private members first
* public methods after private members
* static constexpr for constants
* descriptive names
* no macros unless required
* no global variables
* no lambdas unless necessary
* avoid STL containers

Example style:

```c++
class ExampleScreen : public Screen {
private:

    static constexpr int SPEED = 4;

    int x = 0;

public:

    ExampleScreen(...);

    void tick() override;

private:

    void update();
    void draw();
};
```

---

# Screen Input Convention

Unless specified otherwise:

* `SHORT_CLICK` performs the primary action.
* `LONG_CLICK` exits to `navigator->open(Navigator::MENU);`.

Games usually have three states:

```c++
WAITING
PLAYING
GAME_OVER
```

where

* `SHORT_CLICK` starts game
* `SHORT_CLICK` performs gameplay action
* `SHORT_CLICK` restarts after game over
* `LONG_CLICK` returns to menu at any time

---

# General Goal

Every screen should be:

* self-contained
* lightweight
* non-blocking
* deterministic
* easy to add to Navigator
* independent of other screens except through Navigator
* suitable for ESP32-C3 memory and CPU constraints
* written as production-quality Arduino C++ code