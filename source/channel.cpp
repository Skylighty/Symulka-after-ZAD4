#include "channel.h"


Channel::Channel() {
    busy_ = false;
    packets_in_channel_ = new std::vector<Packet *>;
}

Channel::~Channel() {

}

void Channel::PushPacketToChannel(Packet* packet)
{
  this->packets_in_channel_->push_back(packet);
  if (packets_in_channel_->size() > 1)
  {
    for (int i = 0; i < packets_in_channel_->size(); ++i)
    {
      packets_in_channel_->at(i)->SetCollision(true);
    }
  }
}

void Channel::RemovePacketFromChannel(Packet* p)
{
  auto packet = nullptr;
  for (int i = 0; i < packets_in_channel_->size(); ++i) {
    if (packets_in_channel_->at(i)->GetPacketId() == p->GetPacketId()) {
      packets_in_channel_->erase(packets_in_channel_->begin() + i);
    }
    if (packets_in_channel_->size() == 0) {packets_in_channel_->clear();}
  }
}

