#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <vector>
#include "packet.h"

class WirelessNetwork;

class Channel {
public:
    Channel();

    ~Channel();

    void  PushPacketToChannel(Packet* packet);    //Method allowing to push the packet to channel's list
    void  RemovePacketFromChannel(Packet *p);                                                               //Method allowing to remove the packet from channel's list
    bool  ChannelBusy() { return this->busy_; }                                                    //Checks if channel is busy, returns it's state
    void  SetBusy(bool state) { this->busy_ = state; }                                             //Sets the channel busy_ attribute
    int  CountOfPacketsInBuffer() { return static_cast<int>(this->packets_in_channel_->size()); }  //Returns number of elements in channel's list. Static cast, cause of size_t type.
    bool  ChannelEmpty() {return packets_in_channel_->empty();}
private:
    bool busy_ = false;
    std::vector<Packet *> *packets_in_channel_;                                                    //List of packets currently contained by channel
};

#endif //CHANNEL_H_
