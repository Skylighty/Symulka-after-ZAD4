#ifndef GENERATOR_H_
#define GENERATOR_H_
#include <string>
#include <cmath>
#include <vector>

class Generator
{
public:
    explicit Generator(int seed);
    ~Generator();
    
    double Rand();
    int Rand(int min, int max);
    int RandExp(double lambda);
    int RandZeroOne(double probability);
    
    int get_kernel() {return kernel_;}
private:
    int kernel_;
    const double M = 2147483647.0;
    static const int A = 16807;
    static const int Q = 127773;
    static const int R = 2836;
};


#endif //_SYMULKA_NOWA_GENERATOR_H
