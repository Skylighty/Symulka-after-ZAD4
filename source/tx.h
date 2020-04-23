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
  void PushToBuffer(Packet* packet) { this->buffer_->push(packet); }      //Method that allows to push packet to TX's buffer
  void PopFromBuffer() { this->buffer_->pop(); }                          //Method that allows to remove longest waiting packet from buffer
  Packet* PacketFromBuffer() { return this->buffer_->front(); }           //Method that gives us the access to longest waiting packet in buffer
  void SetTXPacket(Packet* packet) { this->current_packet_ = packet; }    //Sets packet as currently serviced by TX
  bool BufferEmpty() { return this->buffer_->empty(); }                   //Returns whether buffer is empty or not
  uint32_t GetTXID() { return this->tx_id_; }                             //Returns very own TX's ID
  Packet* GetCurrentPacket() { return this->current_packet_; }
  
private:
  std::queue<Packet*>* buffer_;               //Packet buffer declaration
  uint32_t tx_id_;                            //Very own TX's ID
  Packet* current_packet_;                    //Currently serviced packet
};

#endif //TX_H_
