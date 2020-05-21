#include "wireless_network.h"
#include <Windows.h>

WirelessNetwork::WirelessNetwork(double lambda, std::queue<int> &squeue) {
    channel_ = new Channel();
    TXList = new std::vector<TX *>;
    RXList = new std::vector<RX *>;
    packet_count = 0;
    successful_packets_ = 0;
    logger = new Logger();
    lambda_ = lambda;
    seed_queue_ = squeue;
    total_buffor_leave_delay_ = 0;
    total_success_delay_ = 0;
    retransmissions_ = 0;
    rejected_packets_ = 0;
}

double WirelessNetwork::GetAverageDelay()
{
  return static_cast<double>(total_success_delay_)/successful_packets_;
}

double WirelessNetwork::GetAverageBufforLeaveTime()
{
  return static_cast<double>(total_buffor_leave_delay_)/successful_packets_;
}

//Returns average retransmissions (only successful packets)
double WirelessNetwork::GetAverageRetransmissions()
{
  return static_cast<double>(retransmissions_)/successful_packets_;
}

//Returns average BER from all TXs
double WirelessNetwork::GetAverageBER()
{
  double sum = 0;
  for (int i = 0; i < kDeviceCount_; ++i)
  {
    sum += TXList->at(i)->GetBER();
  }
  return sum/kDeviceCount_;
}

//Returns max BER of all TXs
double WirelessNetwork::GetMaxBER()
{
  double max = 0;
  for (int i = 0; i < kDeviceCount_; ++i)
  {
    if (TXList->at(i)->GetBER() > max)
      max = TXList->at(i)->GetBER();
  }
  return max;
}

void WirelessNetwork::GenerateRXTX() {
    std::queue<int> temp;
  for (int i = 1; i < (kDeviceCount_ + 1); ++i)
    {
      for (int j = 0; j < kNecessaryGenerators; ++j)
      {
        temp.push(seed_queue_.front());
        seed_queue_.pop();
      }
      TX *new_tx = new TX(i,temp);
      TXList->push_back(new_tx);
      RX *new_rx = new RX(i);
      RXList->push_back(new_rx);
      while(!temp.empty())
        temp.pop();
    }
    for (int i = 0; i < kDeviceCount_; ++i)
      TXList->at(i)->SetNetwork(this);
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





