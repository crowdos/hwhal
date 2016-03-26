#include <iostream>
#include <cassert>
#include "context.h"
#include "info.h"

int main(int argc, char *argv[]) {
  Context *ctx = Context::create(true);
  if (!ctx) {
    std::cerr << "Failed to initialize HwHal context" << std::endl;
    return 1;
  }

  std::cout << "Display " << ctx->display() << std::endl;
  std::cout << "USB " << ctx->usb() << std::endl;
  Info *info = ctx->info();
  {
    Info *info2 = ctx->info();
    assert(info == info2);
  }

  std::cout << "Info " << info << std::endl;
  std::cout << " Maker: " << info->maker() << ", Model: " << info->model() << ", Code: " << info->codeName() << std::endl;
  ctx->put(info);
  std::cout << "Battery " << ctx->battery() << std::endl;
  std::cout << "Lights " << ctx->lights() << std::endl;

  delete ctx;

  return 0;
}
