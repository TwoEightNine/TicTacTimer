# Architecture

Tic Tac Timer is an Arduino/C++ application for the ESP32-C3 with a 128x64 SSD1306-compatible OLED, one button and a buzzer. The UI is a set of independent screens managed by a central navigator.

Follow the conventions below when adding a screen or changing an existing one.

## Layout

```
tic_tac_timer.ino      creates the hardware objects and screens, registers them, runs the loop
src/core/Screen.h      Screen base class
src/core/Navigator.*   owns the screens and ticks the current one
src/hal/               ButtonController, ToneController and its implementations
src/screens/           one .h/.cpp pair per screen
src/assets/            bitmap frame headers + Animations.h (gallery list)
```

Arduino compiles everything under `src/` recursively. Files include each other with relative paths (`"../core/Navigator.h"`).

## Main loop

```c++
void loop()
{
    button.tick();
    navigator.tick();
    delay(5);
}
```

## Screen base class

Every screen derives from `Screen`:

```c++
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

* `tick()` is called continuously from the main loop, and a screen does all its work inside it.
* Screens don't own a display object; they use the reference from `Screen`.
* Screens should never block. Prefer `millis()` for timing; use `delay()` only for tiny visual effects when unavoidable.
* All drawing goes directly to `display`, and every frame ends with `display.display();`.

## Screen factory pattern

Every screen is exactly two files in `src/screens/`, for example `MenuScreen.h` and `MenuScreen.cpp`.

The header only declares a factory:

```c++
#pragma once

#include "../core/Screen.h"

Screen* createMenuScreen(
    Adafruit_SSD1306& display,
    ToneController& tone,
    ButtonController& button,
    Navigator* navigator
);
```

The `.cpp` defines the class internally (it is not declared in the header) and implements the factory at the end:

```c++
class MenuScreen : public Screen
{
    ...
};

Screen* createMenuScreen(...)
{
    return new MenuScreen(...);
}
```

To add a screen:

1. Create `src/screens/FooScreen.h/.cpp` following the pattern above.
2. Add `FOO` to `Navigator::ScreenId` (before `SCREEN_COUNT`).
3. In `tic_tac_timer.ino`, include the header, create the screen and `registerScreen()` it.
4. Add a menu entry in `MenuScreen.cpp` if it should be reachable from the menu.

## Navigator

The navigator owns every screen, remembers the current one and calls `current->tick()` on every loop.

Navigation happens only through `navigator->open(Navigator::MENU);` with an id from the `Navigator::ScreenId` enum (`MENU`, `SPLASH`, `WORKOUT`, ...).

Screens never create or destroy other screens; they only request navigation. Screens are not notified when they are opened or left, so a screen must reset its own state when it exits.

## ButtonController

`ButtonController` is non-blocking: the main loop calls `button.tick();` and screens read events with

```c++
ButtonController::Event event = button.getEvent();
```

`getEvent()` consumes the event. A press is classified on release:

| Event              | Press duration |
|--------------------|----------------|
| `SHORT_CLICK`      | < 700 ms       |
| `LONG_CLICK`       | 700 – 2000 ms  |
| `EXTRA_LONG_CLICK` | ≥ 2000 ms      |

Never call `digitalRead()` from a screen.

## ToneController

Screens never touch the buzzer directly. They call

```c++
tone.tone(ToneController::KILO, ToneController::SHORT);
```

with a `Tone` (`HALF` 500 Hz, `KILO` 1 kHz, `CHIRP` 1.5 kHz) and a `Duration` (`TICK` 62 ms, `SHORT` 125 ms, `MEDIUM` 250 ms, `LONG` 500 ms).

There are two implementations of the interface: `BuzzerToneController`, and `NoopToneController`, which ignores every request.

## Display

The display type is `Adafruit_SSD1306`, with `Adafruit_GFX` for graphics. The origin is top-left and the screen is 128x64.

```c++
display.clearDisplay();
display.setTextSize(2);
display.setCursor(x, y);
display.print(...);
display.drawRect(...);
display.fillRect(...);
display.display();
```

Full-screen bitmaps are raw 1024-byte display buffers stored in PROGMEM and copied with `memcpy_P(display.getBuffer(), frame, ANIMATION_FRAME_SIZE)`. The buffer is 8 pages of 128 columns, where each byte holds 8 vertical pixels with the least significant bit on top.

## Input conventions

Unless a screen has a reason to differ:

* `SHORT_CLICK` performs the primary action.
* `LONG_CLICK` returns to the menu with `navigator->open(Navigator::MENU);`.

The menu is the exception: `SHORT_CLICK` moves to the next item and `LONG_CLICK` opens it.

Games usually have three states, `WAITING`, `PLAYING` and `GAME_OVER`:

* `SHORT_CLICK` starts the game, performs the gameplay action and restarts after game over.
* `LONG_CLICK` returns to the menu at any time.

## UI and coding conventions

UI:

* Monochrome graphics; avoid unnecessary flicker.
* Prefer integer arithmetic.
* No heap allocations inside `tick()`.
* Avoid `String`; prefer `const char*` or flash strings `F("text")`.
* Game loops use `millis()`, and animations should be frame-rate independent where possible.

Code style:

* Braces on separate lines.
* Explicit constructors.
* Private members first, then public methods.
* `static constexpr` for constants.
* Descriptive names.
* No macros unless required, no global variables, no lambdas unless necessary, no STL containers.

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

## Goals

Every screen should be self-contained, lightweight, non-blocking and deterministic. It should be easy to add to the navigator, depend on other screens only through the navigator, fit the ESP32-C3's memory and CPU limits, and be production-quality Arduino C++.
