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
  void GenerateRXTX();                                      //Method that creates RXs and TXs specified by constant kDeviceCount attribute
  void GeneratePacket(uint32_t did, WirelessNetwork* wn);   //Method that allows to create new packet
  void WaitCGPk();                                          //As we create packets, we also need to be able to wait CGPk time
  uint32_t GetPacketCount() { return this->packet_count; }  //Returns total count of packets transmitted by system
  TX* GetTX(int id);                                        //Returns TX of particular device ID
  RX* GetRX(int id);                                        //Same as above, but for RX
  Channel* channel_;                                        //Channel declaration (didn't want to make it private, it's much simpler and comfy this way actually)
  int const kMaxRetransmissions = 10;                       //Const representing maximal retransmission number
  int const kDeviceCount_ = 4;                              //Const representing number of RX/TX pairs
private:
  uint32_t packet_count;                                    //Variable that stores total packet count transmitted in system
  std::vector<TX*>* TXList;                                 //Vector container of all TXs
  std::vector<RX*>* RXList;                                 //Vector container of all RXs
  
};

#endif //WIRELESS_NETWORK_H_
