#include "tx.h"

TX::TX(int id, std::queue<int>* slist)
{
  seed_list_ = slist;
  tx_id_ = id;
  buffer_ = new std::queue<Packet *>;
  current_packet_ = nullptr;
  CGPgenerator = new Generator(GetSeed());
  CPgenerator = new Generator(GetSeed());
  CTPgenerator = new Generator(GetSeed());
  CRPgenerator = new Generator(GetSeed());;
  Rgenerator = new Generator(GetSeed());
  Tgenerator = new Generator(GetSeed());;
}

TX::~TX() {

}

int TX::GetSeed() {
    int seed;
    seed = seed_list_->front();
    seed_list_->pop();
    return seed;
}