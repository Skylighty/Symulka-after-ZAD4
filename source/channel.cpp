#include "channel.h"

Channel::Channel()
{
  busy_ = false;
  packets_in_channel_ = new std::vector<Packet*>;
}

Channel::~Channel()
{
  
}

