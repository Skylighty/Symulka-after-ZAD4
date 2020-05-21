#include "tx.h"

TX::TX(int id, std::queue<int> &sq)
{
  success_packets_ = 0;
  rejected_packets_ = 0;
  tx_id_ = id;
  buffer_ = new std::queue<Packet *>;
  current_packet_ = nullptr;
  seeds = sq;
  GeneratorCGP = new Generator(GetSeed());
  GeneratorCP = new Generator(GetSeed());
  GeneratorCRP = new Generator(GetSeed());
  GeneratorCTP = new Generator(GetSeed());
  GeneratorR = new Generator(GetSeed());
  GeneratorT = new Generator(GetSeed());
  GeneratorTER = new Generator(GetSeed());
}

TX::~TX() {

}

int TX::GetSeed()
{
  int seed;
  seed = seeds.front();
  seeds.pop();
  return seed;
}

double TX::GetBER()
{
  if (success_packets_ != 0)
    return static_cast<double>(rejected_packets_)/success_packets_;
  else
    return 0;
}


