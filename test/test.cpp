#include <iostream>
#include <cassert>
#include "hwhal/context.h"
#include "hwhal/info.h"
#include "hwhal/display.h"
#include "hwhal/usb.h"
#include "hwhal/battery.h"
#include "hwhal/lights.h"
#include "hwhal/keys.h"

int main(int argc, char *argv[]) {
  Context *ctx = Context::create(true);
  if (!ctx) {
    std::cerr << "Failed to initialize HwHal context" << std::endl;
    return 1;
  }

  std::cout << "Display " << ctx->control<Display>(nullptr, ControlId::Display) << std::endl;
  std::cout << "USB " << ctx->control<Usb>(nullptr, ControlId::Usb) << std::endl;
  Info *info = ctx->control<Info>(nullptr, ControlId::Info);
  {
    Info *info2 = ctx->control<Info>(nullptr, ControlId::Info);
    assert(info == info2);
  }

  std::cout << "Info " << info << std::endl;
  std::cout << " Maker: " << info->maker() << ", Model: " << info->model() << ", Code: " << info->codeName() << std::endl;
  ctx->put(info);
  std::cout << "Battery " << ctx->control<Battery>(nullptr, ControlId::Battery) << std::endl;
  std::cout << "Lights " << ctx->control<Lights>(nullptr, ControlId::Lights) << std::endl;

  delete ctx;

  return 0;
}
