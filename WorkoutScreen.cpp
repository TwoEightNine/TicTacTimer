#include "WorkoutScreen.h"
#include "Navigator.h"

#define ACTIVITY_TYPE_INIT 0
#define ACTIVITY_TYPE_WORKOUT 1
#define ACTIVITY_TYPE_REST 2

#define ACTIVITY_INIT_DURATION 5
#define ACTIVITY_REST_DURATION 10
#define ACTIVITY_WORKOUT_DURATION 24

class WorkoutScreen : public Screen {
public:
    WorkoutScreen(Adafruit_SSD1306& display, ToneController& tone, ButtonController& button, Navigator* navigator) : Screen(display, tone, button, navigator) {
        startedAt = 0;
        pausedAt = 0;
        lastTickedS = 0;
    }

    void tick() override {
        if (startedAt == 0) {
            startedAt = millis();
        }
        ButtonController::Event event = button.getEvent();
        if (event == ButtonController::SHORT_CLICK) {
            if (pausedAt == 0) {
                pausedAt = millis();
                display.fillRect(18, 18, 92, 28, SSD1306_BLACK);
                display.drawRect(18, 18, 92, 28, SSD1306_WHITE);

                display.setTextSize(2);
                display.setCursor(30, 26);
                display.println(F("PAUSED"));
                display.display();
            } else {
              unsigned long pauseDuration = millis() - pausedAt;
                pausedAt = 0;
                startedAt += pauseDuration;
            }
        }
        if (event == ButtonController::LONG_CLICK) {
            startedAt = 0;
            display.invertDisplay(false);
            navigator->open(Navigator::MENU);
            return;
        }

        if (pausedAt == 0) {
            renderWorkout();
        }
    }

private:
    unsigned long startedAt;
    unsigned long pausedAt;
    unsigned long lastTickedS;

    void renderWorkout();
    void renderInit(float secondsRaw);
    void renderActivity(float seconds);
    void printTime(uint16_t seconds);
    uint8_t getActivityType(float seconds);
    uint8_t getActivityCycle(float seconds);
    float getActivityTime(float seconds);
    float getActivityProgress(float activityTime, uint8_t activityType);
};

static float floatMod(float a, uint8_t b);
static bool isInRange(uint8_t value, uint8_t from, uint8_t to);

void WorkoutScreen::renderWorkout() {
  display.clearDisplay();
  unsigned long millisecondsRaw = millis() - startedAt;
  float secondsRaw = millisecondsRaw / 1000.0;

  if (secondsRaw < ACTIVITY_INIT_DURATION) {
    renderInit(secondsRaw);
  } else {
    float seconds = secondsRaw - ACTIVITY_INIT_DURATION;
    renderActivity(seconds);
  }
  
  display.display();
}


void WorkoutScreen::renderInit(float secondsRaw) {
  uint8_t secondsRemain = ACTIVITY_INIT_DURATION - int(secondsRaw);
  display.setCursor(52, 15);
  display.setTextSize(5);
  display.println(secondsRemain);

  display.invertDisplay(true);

  unsigned long s = int(secondsRaw);
  if (s != lastTickedS) {
    lastTickedS = s;
    // prepare always
    tone.tone(ToneController::HALF, ToneController::SHORT);
    Serial.println(F("TONE: init"));
  }
}

void WorkoutScreen::renderActivity(float seconds) {
  display.setTextSize(2); 
  display.setCursor(0, 7);
  printTime(seconds);

  uint8_t activityType = getActivityType(seconds);
  if (activityType == ACTIVITY_TYPE_INIT) {
    display.println(F(""));
  } else {
    display.print(' ');
    if (activityType == ACTIVITY_TYPE_WORKOUT) {
      display.print('W');
    } else {
      display.print('R');
    }
    display.print(' ');

    uint8_t cycleNum = getActivityCycle(seconds) % 100;
    if (cycleNum < 10) {
      display.print(' ');
    }
    display.println(cycleNum);
  }

  float activityTime = getActivityTime(seconds);
  uint8_t activityTimeInt = int(activityTime);
  float activityProgress = getActivityProgress(activityTime, activityType);
  int activityProgressInt = int(activityProgress);
  // display.println(activityProgress);

  display.setCursor(68, 5);
  display.setTextSize(5);
  if (activityTimeInt < 10) {
    display.print(' ');
  }
  display.println(activityTimeInt);

  display.invertDisplay(activityType != ACTIVITY_TYPE_WORKOUT);

  uint8_t filledProgress = activityProgress * display.width() / 100;
  display.fillRect(0, 45, filledProgress, 19, SSD1306_WHITE);
  display.fillRect(filledProgress+1, 45, display.width() - filledProgress, 19, SSD1306_BLACK);

  unsigned long s = int(seconds);
  if (s != lastTickedS) {
    lastTickedS = s;
    if (activityType == ACTIVITY_TYPE_WORKOUT) {
      if (activityTimeInt == 0) {
        // start workout
       tone.tone(ToneController::KILO, ToneController::LONG);
       Serial.println(F("TONE: start workout"));
      }
      uint8_t allowedZone = (uint8_t) int(100.0 / ACTIVITY_WORKOUT_DURATION);
      uint8_t allowedHalf = allowedZone / 2;
      if (isInRange(activityProgressInt, 33 - allowedHalf, 33 + allowedHalf) || isInRange(activityProgressInt, 66 - allowedHalf, 66 + allowedHalf)) {
        // mid activity
        tone.tone(ToneController::CHIRP, ToneController::SHORT);
        Serial.println(F("TONE: mid workout"));
      }
    } else if (activityType == ACTIVITY_TYPE_REST) {
      if (activityTimeInt == 0) {
        // rest started
        tone.tone(ToneController::KILO, ToneController::LONG);
        Serial.println(F("TONE: start rest"));
      } else {
        // prepare
        uint8_t left = ACTIVITY_REST_DURATION - activityTimeInt;
        if (left == 5 || left == 3 || left == 2 || left == 1) {
          tone.tone(ToneController::HALF, ToneController::SHORT);
          Serial.println(F("TONE: prepare to stop rest"));
        }
      }
    }
  } 
}

void WorkoutScreen::printTime(uint16_t seconds) {
  uint8_t m = seconds / 60;
  uint8_t s = seconds % 60;

  if (m < 10) {
    display.print(' ');
  }
  display.print(m);
  display.print(':');
  if (s < 10) {
    display.print('0');
  }
  display.println(s);
}

uint8_t WorkoutScreen::getActivityType(float seconds) {
  uint16_t secondsInt = int(seconds);
  uint8_t fullCycle = ACTIVITY_WORKOUT_DURATION + ACTIVITY_REST_DURATION;
  uint16_t passedInCycle = floatMod(secondsInt, fullCycle);

  if (passedInCycle < ACTIVITY_WORKOUT_DURATION) {
    return ACTIVITY_TYPE_WORKOUT;
  } else {
    return ACTIVITY_TYPE_REST;
  }
}

uint8_t WorkoutScreen::getActivityCycle(float seconds) {
  uint16_t secondsInt = int(seconds);
  uint8_t fullCycle = ACTIVITY_WORKOUT_DURATION + ACTIVITY_REST_DURATION;

  return (secondsInt / fullCycle) + 1;
}

float WorkoutScreen::getActivityTime(float seconds) {
  uint8_t fullCycle = ACTIVITY_WORKOUT_DURATION + ACTIVITY_REST_DURATION;
  float passedInCycle = floatMod(seconds, fullCycle);

  if (passedInCycle < ACTIVITY_WORKOUT_DURATION) {
    return passedInCycle;
  } else {
    return passedInCycle - ACTIVITY_WORKOUT_DURATION;
  }
}

float WorkoutScreen::getActivityProgress(float activityTime, uint8_t activityType) {
  uint8_t duration = 1;
  if (activityType == ACTIVITY_TYPE_WORKOUT) {
    duration = ACTIVITY_WORKOUT_DURATION;
  } else if (activityType == ACTIVITY_TYPE_REST) {
    duration = ACTIVITY_REST_DURATION;
  }

  return activityTime * 100 / duration;
}

static float floatMod(float a, uint8_t b) {
  unsigned long aInt = int(a);
  float aAfterPoint = a - aInt;
  return (aInt % b) + aAfterPoint;
}

static bool isInRange(uint8_t value, uint8_t from, uint8_t to) {
  return value >= from && value <= to;
}

Screen* createWorkoutScreen(Adafruit_SSD1306& display, ToneController& tone, ButtonController& button, Navigator* navigator) {
    return new WorkoutScreen(display, tone, button, navigator);
}