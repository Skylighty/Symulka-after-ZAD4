#ifndef TX_H_
#define TX_H_
#include <cstdint>
#include <queue>
#include "packet.h"

class WirelessNetwork;

class TX
{
public:
  TX(int id);
  ~TX();
  void PushToBuffer(Packet* packet) { this->buffer_->push(packet); }
  void PopFromBuffer() { this->buffer_->pop(); }
  Packet* PacketFromBuffer() { return this->buffer_->front(); }
  void SetTXPacket(Packet* packet) { this->current_packet_ = packet; }
  bool BufferEmpty() { return this->buffer_->empty(); }
  uint32_t GetTXID() { return this->tx_id_; }
  
private:
  std::queue<Packet*>* buffer_;
  uint32_t tx_id_;
  Packet* current_packet_;
};

#endif //TX_H_
