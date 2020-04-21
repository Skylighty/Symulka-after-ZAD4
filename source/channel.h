#ifndef CHANNEL_H_
#define CHANNEL_H_
#include <vector>
#include "packet.h"

class WirelessNetwork;

class Channel
{
public:
  Channel();
  ~Channel();
  void PushPacketToChannel(Packet* packet) { this->packets_in_channel_->push_back(packet); }
  void RemovePacketFromChannel() { this->packets_in_channel_->pop_back(); }
  bool MoreThanOneQuery() { return this->packets_in_channel_->empty(); }
  bool ChannelBusy() { return this->busy_; }
  void SetBusy(bool state) { this->busy_ = state; }
  int CountOfPacketsInBuffer() { return static_cast<int>(this->packets_in_channel_->size()); }
private:
  bool busy_;
  std::vector<Packet*>* packets_in_channel_;
};

#endif //CHANNEL_H_
