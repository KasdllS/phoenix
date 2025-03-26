#include <unistd.h>
#include <iostream>

#include "core/thread.h"

int main(int argc, char** argv) {
  {
    auto trigger = core::Thread::this_thread()->addEvent(
        core::Events::Execute,
        [](const core::Event*) { std::cout << "handle" << std::endl; });
    trigger.trigger();
  }
  return core::Application::exec();
}