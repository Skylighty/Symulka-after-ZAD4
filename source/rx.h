#ifndef RX_H_
#define RX_H_
#include <cstdint>
#include "packet.h"

class WirelessNetwork;

class RX
{
public:
  RX(int id);
  ~RX();
  uint32_t GetRXID() { return this->rx_id_; };
  void SetRXPacket(Packet* packet) { this->current_packet_ = packet; };
private:
  uint32_t rx_id_;
  Packet* current_packet_;
};

#endif //RX_H_