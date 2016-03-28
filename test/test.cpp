#include <iostream>
#include <cassert>
#include "context.h"
#include "info.h"
#include "display.h"
#include "usb.h"
#include "battery.h"
#include "lights.h"
#include "keys.h"

int main(int argc, char *argv[]) {
  Context *ctx = Context::create(true);
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
