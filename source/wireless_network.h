#ifndef WIRELESS_NETWORK_H_
#define WIRELESS_NETWORK_H_
#include <vector>
#include "tx.h"
#include "channel.h"
#include "rx.h"
#include "logger.h"

class WirelessNetwork
{
public:
  WirelessNetwork();
  ~WirelessNetwork();
  void GenerateRXTX();
  void GeneratePacket(uint32_t did, WirelessNetwork* wn);
  void WaitCGPk();
  uint32_t GetPacketCount() { return this->packet_count; }
  TX* GetTX(int id);
  RX* GetRX(int id);
  Channel* channel_;
  int const kMaxRetransmissions = 10;
  int const kDeviceCount_ = 4;
private:
  uint32_t packet_count;
  std::vector<TX*>* TXList;
  std::vector<RX*>* RXList;
};

#endif //WIRELESS_NETWORK_H_
