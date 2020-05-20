#include "tx.h"

TX::TX(int id)
{
  tx_id_ = id;
  buffer_ = new std::queue<Packet *>;
  current_packet_ = nullptr;
}

TX::~TX() {

}
