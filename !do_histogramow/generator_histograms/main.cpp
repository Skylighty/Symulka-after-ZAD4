#include <vector>
#include <fstream>
#include <iterator>
#include <string>
#include "generator.h"


int main(int argc, char* argv[])
{
  int number_of_rands = 100000;
  int seed = 12554568;
  auto generator = Generator(seed);
  std::vector<double> numbers_uniform;
  std::vector<double> numbers_uniform_min_max;
  std::vector<double> numbers_exp;
  std::vector<double> numbers_zero_one;

  for (int i = 0; i < number_of_rands; ++i)
  {
    //numbers_uniform.push_back(generator.Rand());
    //numbers_uniform_min_max.push_back(generator.Rand(0.5, 3));
    //numbers_exp.push_back(generator.RandEXP(0.05));
    numbers_zero_one.push_back(generator.RandZeroOne(0.2));
  }

  std::ofstream output_file("./example_zero_one.txt");
  std::ostream_iterator<double> output_iterator(output_file, "\n");
  std::copy(numbers_zero_one.begin(), numbers_zero_one.end(), output_iterator);
}
