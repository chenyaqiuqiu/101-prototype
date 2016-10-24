
#include "CurieIMU.h"

#define STEP_DEBUG 1

#ifdef STEP_DEBUG
#define StepsDebug(x) Serial.println(x) 
#else
#define StepsDebug(x) {}
#endif

static long lastStepCount = 0;              // step count on previous polling check

void CurieStepCountSetup() {
  //CurieIMU.begin();
  // turn on step detection mode:
  CurieIMU.setStepDetectionMode(CURIE_IMU_STEP_MODE_NORMAL);
  // enable step counting:
  CurieIMU.setStepCountEnabled(true);
  // attach the eventCallback function as the
  // step event handler:
  CurieIMU.attachInterrupt(eventCallback);
  CurieIMU.interrupts(CURIE_IMU_STEP);  // turn on step detection
  StepsDebug("IMU initialisation complete, waiting for events...");
}

long getStepCounts() {
  return  lastStepCount;
}

static void updateStepCount() {
  // get the step count:
  int stepCount = CurieIMU.getStepCount();

  // if the step count has changed, print it:
  if (stepCount != lastStepCount) {
         // Serial.print("Step count: ");
         // Serial.println(stepCount);
    // save the current count for comparison next check:
    lastStepCount = stepCount;
  }
}

static void eventCallback(void) {
  if (CurieIMU.stepsDetected()) {
    //Serial.print("event get\r\n");
    updateStepCount();
  }
}
