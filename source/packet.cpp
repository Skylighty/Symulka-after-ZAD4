#include <string>
#include "packet.h"
#include "channel.h"
#include "wireless_network.h"
#include "rx.h"
#include "tx.h"

Packet::Packet(uint32_t did, WirelessNetwork* network)
{
  state_ = State::CREATED;
  error_ = false;
  ack_ = false;
  packet_id_ = network->GetPacketCount() + 1;
  devices_id_ = did;
  logger_ = new Logger();
  active_ = false;
  network_ = network;
  retransmission_ = false;
  collision_ = false;
  r_ = 0;
  t_ = 0;
  this->Execute();
}

Packet::~Packet()
{
  this->logger_->Info("Packet of ID : " + std::to_string(this->packet_id_) + " has been DELETED from system.");
}

bool Packet::CheckForCollision(Channel* channel)
{
  if ((this->network_->channel_->CountOfPacketsInBuffer()) > 1)
  {
    return true;
  }
  else
    return false;
}


void Packet::Execute()
{
  active_ = true;
  if (active_ == true)
  {
    TX* CurrentTX = this->network_->GetTX(this->devices_id_);
    RX* CurrentRX = this->network_->GetRX(this->devices_id_);
    std::string pid = std::to_string(this->packet_id_);
    switch (state_)
    {
    case State::CREATED:  //State that packet process is in just after being created
    {
      logger_->Info("Packet of ID : " + pid + " has been created.");
      state_ = State::WAITING;  //Automatically we change state to WAITING, because here we decide what to do next
      if (CurrentTX->BufferEmpty()) //Checking if TX's packet buffer is empty or if it contains any packets
      {   //If the buffer is empty, packet is immediately set as TX's current and transmitted 
        logger_->Info("Buffer of TX of ID : " + std::to_string(CurrentTX->GetTXID()) + " is empty. Packet passed.");
        CurrentTX->SetTXPacket(this);
      }
      else
      {   //If the buffer is NOT empty, the packet is put at the end of the TX's FIFO queue
        logger_->Info("Buffer of TX of ID : " + std::to_string(CurrentTX->GetTXID()) + " not empty. Packet put in the buffer.");
        CurrentTX->PushToBuffer(this);
      }
    }
    case State::WAITING:
    {
        //Here, the packet checks if the channel is busy, automatically incrementing "t" parameter - counter of channel checks
      ++this->t_;
      if (this->network_->channel_->ChannelBusy() == true)
      {
        //If channel busy, state is not being changed, packet should wait for next channel check.
        this->network_->channel_->SetBusy(true); //Channel set to busy if we start transmitting
        logger_->Info("Channel busy, packet of ID : " + pid + " is still waiting.");
        this->WaitCP();
      }
      else
      {
        //If channel ain't busy, the packet is passed to channet, simultaneously "t" counter resters back to 0.
        logger_->Info("Channel free, packet of ID : " + pid + " passed to channel.");
        this->t_ = 0;
        state_ = State::SENT;
      }
    }
    case State::SENT:
    {
       //Packet is pushed to the channel
      this->network_->channel_->PushPacketToChannel(this);
      state_ = State::IN_CHANNEL;
    }
    case State::IN_CHANNEL:
    {
      //Packet is inside of the channel (it's in channel's vector container for packets too)
      this->WaitCTP();  //Trasmission time should be added to global time counter
      if (this->CheckForErrors(this->network_->channel_)) //Automatically occurance of errors IN CHANNEL is checked
        this->error_ = true;
      if (this->CheckForCollision(this->network_->channel_)) //Same goes for collisions (more than one packet in channel at the same moment)
        this->collision_ = true;
      //network_->channel_->RemovePacketFromChannel();
      logger_->Info("Packet of ID : " + pid + " passed through channel, REMOVED from channel.");
      this->network_->channel_->RemovePacketFromChannel();  //Packet is removed from channel's packet container
      this->network_->channel_->SetBusy(false); //Channel set to free again
      state_ = State::RECEIVED;
    }
    case State::RECEIVED:
    {
      //Packet reaches it's destination - the receiver
      CurrentRX->SetRXPacket(this);   //As packet leaves the channel it's set as a current packet serviced by particular RX
      logger_->Info("Packet of ID : " + pid + " received by RX of ID : " + std::to_string(CurrentRX->GetRXID()));
      if ((this->error_ == true) || (this->collision_ == true))
      {
        //If error/collisions flags were set in channel - ack flag is set to false and the relay is needed
        logger_->Info("Packet of ID : " + pid + " contains errors. ACK FALSE.");
        ack_ = false;
        retransmission_ = true;
      }
      else
      {
        //If packet was claimed successfully ack flag set to true
        logger_->Info("Packet of ID : " + pid + " clear. ACK TRUE.");
        ack_ = true;
      }
      state_ = State::SENT_BACK;
    }
    case State::SENT_BACK:
    {
      //Packet is sent back to TX throught channel
      network_->channel_->PushPacketToChannel(this);
      //TODO - Should channel be set as busy during ACK feedback? Also Ph.D Sroka said to ask about that.
      this->WaitCTIZ(); //ACK transmission time to TX is being waited
      logger_->Info("Packet of ID : " + pid + " returning with ACK flag set through the channel.");
      network_->channel_->RemovePacketFromChannel(); //Packet removed from channel again.
      state_ = State::CHECK;
    }
    case State::CHECK:
    {
      //Packet is set as currently serviced in TX and it's flags are being checked
      CurrentTX->SetTXPacket(this);
      if ((this->ack_ == false) && (this->retransmission_ == true))
      {
        //As my channel availability algorithim (A4) says - if retransmission is needed here we go: 
        logger_->Info("Transmission of packet of ID : " + pid + " failed. Necessary retransmission.");
        ++this->r_; //"r" counter incremented because it enumerates number of retransmissions 
        if (this->r_ < network_->kMaxRetransmissions) //If retransmissions peak not reached - one time CRP time is waited
        {
          //The packet state changed back to waiting, because after first channel check, it should wait CP time, not CRP again.
          WaitCRP();
          state_ = State::WAITING;
        }
        else
          //If ack flag set to true, delete packet and output a success transmission message.
          delete this;
      }
      else
      {
        logger_->Info("Packet of ID : " + pid + " transmitted successfully");
        delete this;
      }
    }
    }
  }
}

void Packet::Activate(double time)
{
}

bool Packet::CheckForErrors(Channel* channel)
{
  return false;
}

void Packet::WaitCP()
{
  
}

void Packet::WaitCRP()
{
  
}

void Packet::WaitCTIZ()
{
  
}

void Packet::WaitCTP()
{
  
}




