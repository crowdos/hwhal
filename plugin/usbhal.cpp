#include "usbhal.h"

void UsbHal::addListener(const std::function<void(bool)>& listener) {

}

bool UsbHal::isCableConnected() {
  return false;
}

bool UsbHal::setMode(const Mode& mode) {
  return false;
}
