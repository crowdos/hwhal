#include "usbhal.h"

void UsbHal::addListener(const std::function<void(bool)>&) {

}

bool UsbHal::isCableConnected() {
  return false;
}

bool UsbHal::setMode(const Mode&) {
  return false;
}
