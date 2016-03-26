#include "displayhal.h"

DisplayHal::DisplayHal() :
  m_brightness(10),
  m_blank(false) {

}

int DisplayHal::minBrightness() {
  return 0;
}

int DisplayHal::maxBrightness() {
  return 100;
}

int DisplayHal::brightness() {
  return m_brightness;
}

void DisplayHal::setBrightness(int brightness) {
  m_brightness = brightness;
}

void DisplayHal::blank(bool blank) {
  m_blank = blank;
}

bool DisplayHal::isBlank() {
  return m_blank;
}
