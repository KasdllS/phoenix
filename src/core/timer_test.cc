#include <unistd.h>
#include <chrono>
#include <iostream>

#include "core/thread.h"
#include "core/timer.h"

int main(int argc, char** argv) {
  core::Timer timer([](const core::Event*) {
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(
                     std::chrono::steady_clock::now().time_since_epoch())
                     .count()
              << std::endl;
  });
  timer.start(1e3);
  return core::Application::exec();
}