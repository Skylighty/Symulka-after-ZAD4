#include "wireless_network.h"
#include <Windows.h>
WirelessNetwork::WirelessNetwork()
{
  channel_ = new Channel();
  TXList = new std::vector<TX*>;
  RXList = new std::vector<RX*>;
  PacketList = new std::vector<Packet*>;
  packet_count = 0;
  this->GenerateRXTX();
}

void WirelessNetwork::GenerateRXTX()
{
  for (int i = 1; i < (kDeviceCount_+1); ++i)
  {
    TX* new_tx = new TX(i);
    TXList->push_back(new_tx);
    RX* new_rx = new RX(i);
    RXList->push_back(new_rx);
  }
}

void WirelessNetwork::GeneratePacket(uint32_t did, WirelessNetwork* wn)
{
  Packet* new_packet = new Packet(did, wn);
  this->PacketList->push_back(new_packet);
  ++this->packet_count;
}

void WirelessNetwork::WaitCGPk()
{
  Sleep(5);
}

TX* WirelessNetwork::GetTX(int id)
{
  for (int i = 0; i < (TXList->size()); ++i)
  {
    TX* checked_tx = TXList->at(i);
    if (checked_tx->GetTXID() == id)
      return checked_tx;
  }
}

RX* WirelessNetwork::GetRX(int id)
{
  for (int i = 0; i < (RXList->size()); ++i)
  {
    RX* checked_rx = RXList->at(i);
    if (checked_rx->GetRXID() == id)
      return checked_rx;
  }
}