#include "control.h"

Control::~Control() {
  m_context = nullptr;
}

Context *Control::context() {
  return m_context;
}

Control::Control(Context *ctx) :
  m_context(ctx) {

}

bool Control::init(const std::string& name) {
  // TODO:

  return true;
}
