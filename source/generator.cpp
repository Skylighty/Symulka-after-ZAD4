#include "generator.h"

Generator::Generator(int seed) {
  kernel_ = seed;
}

Generator::~Generator(){}

double Generator::Rand()
{
  int h = floor(kernel_/Q);
  kernel_ = A*(kernel_-Q*h)-R*h;
  if (kernel_ < 0)
    kernel_ = kernel_ + static_cast<int>(M);
  return kernel_/M;
}

int Generator::Rand(int min, int max) {
  return Rand() * (max-min)+min;
}

int Generator::RandExp(double lambda) {
  auto k = Rand();
  return -(1.0/lambda) * log(k);
}

int Generator::RandZeroOne(double probability) {
  auto k = Rand();
  if(k < probability)
    return 1;
  else
    return 0;
}