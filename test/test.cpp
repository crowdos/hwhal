#include <iostream>
#include <cassert>
#include "hwhal/context.h"
#include "hwhal/info.h"
#include "hwhal/display.h"
#include "hwhal/usb.h"
#include "hwhal/battery.h"
#include "hwhal/lights.h"
#include "hwhal/keys.h"
#include "hwhal/loopintegration.h"

int main(int argc, char *argv[]) {
  Context *ctx = Context::create(nullptr, true);
  if (!ctx) {
    std::cerr << "Failed to initialize HwHal context" << std::endl;
    return 1;
  }

  std::cout << "Display " << ctx->control<Display>(ControlId::Display) << std::endl;
  std::cout << "USB " << ctx->control<Usb>(ControlId::Usb) << std::endl;
  Info *info = ctx->control<Info>(ControlId::Info);
  {
    Info *info2 = ctx->control<Info>(ControlId::Info);
    assert(info == info2);
  }

  std::cout << "Info " << info << std::endl;
  std::cout << " Maker: " << info->maker() << ", Model: " << info->model() << ", Code: " << info->codeName() << std::endl;
  ctx->put(info);
  std::cout << "Battery " << ctx->control<Battery>(ControlId::Battery) << std::endl;
  std::cout << "Lights " << ctx->control<Lights>(ControlId::Lights) << std::endl;

  delete ctx;

  return 0;
}
