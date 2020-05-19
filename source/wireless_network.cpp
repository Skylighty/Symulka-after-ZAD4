#include "wireless_network.h"
#include <Windows.h>

WirelessNetwork::WirelessNetwork(double lambda,  std::vector<std::queue<int>*>* svector) {
    channel_ = new Channel();
    TXList = new std::vector<TX *>;
    RXList = new std::vector<RX *>;
    packet_count = 0;
    successful_packets_ = 0;
    dead_packets_ = 0;
    logger = new Logger();
    //this->GenerateRXTX();
    lambda_ = lambda;
    seed_vector_ = svector;
}

void WirelessNetwork::GenerateRXTX() {
    for (int i = 1; i < (kDeviceCount_ + 1); ++i) {
        TX *new_tx = new TX(i,seed_vector_->at(i-1));
        TXList->push_back(new_tx);
        RX *new_rx = new RX(i);
        RXList->push_back(new_rx);
    }
}



//Correction - fixed warning of case that function returns nothing
//Kinda dangerous to make it this way, but I don't know how to make it other way, will work properly anyway
//Same goes for GetRX function.
TX *WirelessNetwork::GetTX(int id) {
  TX *checked_tx = nullptr;
  //Starting at 0, because here we check the vector, not index concretely
  for (int i = 0; i < (kDeviceCount_); ++i) {
        checked_tx = TXList->at(i);
        if (checked_tx->GetTXID() == id) {
          break;
        }
    }
  return checked_tx;
}

RX *WirelessNetwork::GetRX(int id) {
  RX *checked_rx = nullptr;
  for (int i = 0; i < (kDeviceCount_); ++i) {
        checked_rx = RXList->at(i);
        if (checked_rx->GetRXID() == id)
            break;
  }
  return checked_rx;
}



