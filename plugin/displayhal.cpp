#include "displayhal.h"

DisplayHal::DisplayHal() :
  m_blank(false) {

}

void DisplayHal::blank(bool blank) {
  m_blank = blank;
}

bool DisplayHal::isBlank() {
  return m_blank;
}
