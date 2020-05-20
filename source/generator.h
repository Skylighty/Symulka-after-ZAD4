#ifndef GENERATOR_H_
#define GENERATOR_H_


#include <string>
#include <cmath>
#include <vector>

class Generator
{

public:
    Generator(int seed);
    ~Generator();
    
    double Rand();
    int Rand(int min, int max);
    int RandEXP(double lambda);
    int RandZeroOne(double probability);
    
    
    int get_kernel()
    {
      return kernel_;
    }

protected:
    
    int kernel_;
    const double M = 2147483647.0;
    static const int A = 16807;
    static const int Q = 127773;
    static const int R = 2836;
  
  
  
  
};

#endif //_SYMULKA_NOWA_GENERATOR_H
