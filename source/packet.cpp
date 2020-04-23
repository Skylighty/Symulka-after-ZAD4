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
  terminated_ = false;
  r_ = 0;
  t_ = 0;
  //this->Execute();
}

//TODO - Safe deleting packet from vector after hitting retransmission cap or successful transmission end. 
Packet::~Packet()
{
  //network_->PacketList->
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
    switch (state_)
    {
    case State::CREATED:
    {
      this->StateCreated();
      break;
    }
    case State::WAITING:
    {
      this->StateWaiting();
      break;
    }
    case State::SENT:
    {
      this->StateSent();
      break;
    }
    case State::IN_CHANNEL:
    {
      this->StateInChannel();
      break;
    }
    case State::RECEIVED:
    {
      this->StateReceived();
      break;
    }
    case State::SENT_BACK:
    {
      this->StateSentBack();
      break;
    }
    case State::CHECK:
    {
      this->StateCheck();
      break;
    }
    }
  }
}

void Packet::Activate(double time)
{
  this->active_ = true;
}

void Packet::StateCreated()
{
  TX* CurrentTX = this->network_->GetTX(this->devices_id_);
  std::string pid = std::to_string(this->packet_id_);
  logger_->Info("Packet of ID : " + pid + " has been created.");
  state_ = State::WAITING;  //Automatically we change state to WAITING, because here we decide what to do next
  if ((CurrentTX->BufferEmpty()) && (CurrentTX->GetCurrentPacket() == nullptr)) //Checking if TX's packet buffer is empty or if it contains any packets
  {   //If the buffer is empty, packet is immediately set as TX's current and transmitted 
    logger_->Info("Buffer of TX of ID : " + std::to_string(CurrentTX->GetTXID()) + " is empty. Set as current packet for TX");
    CurrentTX->SetTXPacket(this);
  }
  else
  {   //If the buffer is NOT empty, the packet is put at the end of the TX's FIFO queue
    logger_->Info("Buffer of TX of ID : " + std::to_string(CurrentTX->GetTXID()) + " not empty. Packet put in the buffer.");
    CurrentTX->PushToBuffer(this);
  }
}

void Packet::StateWaiting()
{
  //Here, the packet checks if the channel is busy, automatically incrementing "t" parameter - counter of channel checks
  std::string pid = std::to_string(this->packet_id_);
  ++this->t_;
  if (this->network_->channel_->ChannelBusy() == false)
  {
    //If channel ain't busy, the packet is passed to channet, simultaneously "t" counter resters back to 0.
    this->network_->channel_->SetBusy(true); //Channel set to busy if we start transmitting
    logger_->Info("Channel free, packet of ID : " + pid + " passed to the channel.");
    this->t_ = 0; 
    state_ = State::SENT;
  }
  else
  {
    //If channel busy, state is not being changed, packet should wait for next channel check.
    logger_->Info("Channel busy, packet of ID : " + pid + " is still waiting.");
    this->WaitCP();
  }
}

void Packet::StateSent()
{
  //Packet is pushed to the channel
  this->logger_->Info("Packet has been put in the channel successfully.");
  this->network_->channel_->PushPacketToChannel(this);
  state_ = State::IN_CHANNEL;
}

void Packet::StateInChannel()
{
  //Packet is inside of the channel (it's in channel's vector container for packets too)
  std::string pid = std::to_string(this->packet_id_);
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

void Packet::StateReceived()
{
  //Packet reaches it's destination - the receiver
  std::string pid = std::to_string(this->packet_id_);
  RX* CurrentRX = this->network_->GetRX(this->devices_id_);
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

void Packet::StateSentBack()
{
  //Packet is sent back to TX throught channel
  std::string pid = std::to_string(this->packet_id_);
  network_->channel_->PushPacketToChannel(this);
  network_->channel_->SetBusy(true); //Channel busy for ACK transmission time
  this->WaitCTIZ(); //ACK transmission time to TX is being waited
  logger_->Info("Packet of ID : " + pid + " returning with ACK flag set through the channel.");
  state_ = State::CHECK;
}

void Packet::StateCheck()
{
  //Packet is set as currently serviced in TX and it's flags are being checked
  TX* CurrentTX = this->network_->GetTX(this->devices_id_);
  std::string pid = std::to_string(this->packet_id_);
  CurrentTX->SetTXPacket(this);
  network_->channel_->RemovePacketFromChannel(); //Packet removed from channel again.
  network_->channel_->SetBusy(false); //Channel free after transmitting ACK
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
    {
      //If ack flag set to true, delete packet and output a success transmission message.
      this->terminated_ = true;
      CurrentTX->SetTXPacket(nullptr);
      logger_->Info("Packet of ID : " + pid + " reached retransmission cap.");
      if (CurrentTX->BufferEmpty() == false) //If TX's buffer isn't empty, after deleting packet from system pop new from buffer and set it as current.
      {
        CurrentTX->SetTXPacket(CurrentTX->PacketFromBuffer());
        CurrentTX->PopFromBuffer();
      }
      else if ((CurrentTX->BufferEmpty() == true) && (CurrentTX->GetCurrentPacket() == nullptr))
        network_->GeneratePacket(CurrentTX->GetTXID(), network_);
      delete this;
    }
  }
  else
  {
    this->terminated_ = true;
    CurrentTX->SetTXPacket(nullptr);
    logger_->Info("Packet of ID : " + pid + " transmitted successfully");
    //delete this;
    if (CurrentTX->BufferEmpty() == false)
    {
      CurrentTX->SetTXPacket(CurrentTX->PacketFromBuffer());
      CurrentTX->PopFromBuffer();
    }
    else if ((CurrentTX->BufferEmpty() == true) && (CurrentTX->GetCurrentPacket() == nullptr))
      network_->GeneratePacket(CurrentTX->GetTXID(), network_);
    delete this;
  }
}


bool Packet::CheckForErrors(Channel* channel)
{
  //TODO - Implement checking for errors
  return false;
}

//TODO - Implement waiting these times or do it through "Activate(double time)"
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




