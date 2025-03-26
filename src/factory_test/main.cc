#include <iostream>
#include "factory.h"
#include "derivedA.h"

int main(int argc, char** argv) {
  auto p = BaseFactory::instance().Create("A");
  std::cout << p->type() << std::endl;
  p = BaseFactory::instance().Create("B");
  std::cout << p->type() << std::endl;

  return 0;
}