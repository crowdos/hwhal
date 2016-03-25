#include "displayhal.h"

int DisplayHal::brightness() {
  return 10;
}

void DisplayHal::setBrightness(int brightness) {
  // Nothing
}

void DisplayHal::blank(bool blank) {
  // Nothing
}

bool DisplayHal::isBlank() {
  return false;
}
