#include <string>
#include "packet.h"
#include "channel.h"
#include "wireless_network.h"
#include <ctime>
#include <cstdlib>
#include <random>
#include <cmath>
#include "rx.h"
#include "tx.h"

Packet::Packet(uint32_t did, WirelessNetwork *network, size_t time, Agenda *agenda, Logger* log) : Process(time, network, agenda) {
    state_ = State::CREATED;
    agenda_ = agenda;
    error_ = false;
    ack_ = false;
    devices_id_ = did;
    logger_ = log;
    network_ = network;
    packet_id_ = (network_->GetPacketCount()) + 1;
    network_->IncPacketCount();
    retransmission_ = false;
    collision_ = false;
    terminated_ = false;
    active_ = false;
    r_ = 0;
    t_ = 0;
    R_ = 0;
    T_ = 0;
    ctp_time_ = GenerateCTP();
    /*std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1,10);
    std::uniform_int_distribution<int> cp_distribution(30,40);
    std::uniform_int_distribution<int> cgp_distribution (60,100);
    crp_time_ = static_cast<size_t>((rand() % 10)+1);
    cp_time_ = static_cast<size_t>((rand() % 10) + 1);
    cgp_time_ = static_cast<size_t>((rand() % 10)+15);*/
    //this->Execute();
}


Packet::~Packet() {
    //network_->PacketList->
    this->logger_->Info("Packet of ID : " + std::to_string(this->packet_id_) + " has been DELETED from system.");
}


void Packet::Execute() {
    active_ = true;
    while (active_) {
        switch (state_) {
            case State::CREATED: {
                this->StateCreated();
                break;
            }
            case State::WAITING: {
                this->StateWaiting();
                break;
            }
            case State::SENT: {
                this->StateSent();
                break;
            }
            case State::IN_CHANNEL: {
                this->StateInChannel();
                break;
            }
            case State::RECEIVED: {
                this->StateReceived();
                break;
            }
            case State::SENT_BACK: {
                this->StateSentBack();
                break;
            }
            case State::CHECK: {
                this->StateCheck();
                break;
            }
        }
    }
}


void Packet::StateCreated()
{
    TX *CurrentTX = network_->GetTX(devices_id_);
    
    //As we create new packet upon end of transmission of the old one, we need to update simulation time
    //Activate(cgp_time_); //TODO - Implement this in generate packet
    //Next packet is already being planned and placed for agenda
    
    GenerateNext();
    std::string pid = std::to_string(packet_id_); 
    logger_->Info("Packet of ID : " + pid + " has been created.");
    
    state_ = State::WAITING;  //Automatically we change state to WAITING, because here we decide what to do next
    
    if ((CurrentTX->BufferEmpty()) && (CurrentTX->GetCurrentPacket() ==
                                       nullptr)) //Checking if TX's packet buffer is empty or if it contains any packets
    {   //If the buffer is empty, packet is immediately set as TX's current and transmitted
        logger_->Info("Buffer of TX of ID : " + std::to_string(CurrentTX->GetTXID()) +
                      " is empty. Set as current packet for TX");
        CurrentTX->SetTXPacket(this);
    } else {   //If the buffer is NOT empty, the packet is put at the end of the TX's FIFO queue
        logger_->Info("Buffer of TX of ID : " + std::to_string(CurrentTX->GetTXID()) +
                      " not empty. Packet put in the buffer.");
        CurrentTX->PushToBuffer(this);
    }
    //Below line should remove process from agenda and protect us from endless channel checking
    if (this != CurrentTX->GetCurrentPacket()) { active_ = false; }
}

void Packet::StateWaiting() {
    //PACKET-FLOOD PROTECTION
    //Here, the packet checks if the channel is busy, automatically incrementing "t" parameter - counter of channel checks
    std::string pid = std::to_string(packet_id_);
    //++t_;
    if (!network_->channel_->ChannelBusy())
    {
      //If channel ain't busy, the packet is passed to channel, simultaneously "t" counter restarts back to 0.
      //-----------------------------------------------
      //Correction here - channel should be set busy in StateSent, so we can implement collision
      //-----------------------------------------------
        
        logger_->Info("Channel free, packet of ID : " + pid + " passed to the channel.");
        t_ = 0;
        //Below line allows collision to occur - easy implementation (mentioned in line 133 too)
        network_->channel_->PushPacketToChannel(this);
        state_ = State::SENT;
        Activate(1);
        active_ = false;
    } else {
      /**bool temp = network_->channel_->ChannelBusy();
      char tempch = 'x';
      if (temp) {tempch = 'Y';}
      else {tempch = 'N';}
        //If channel busy, state is not being changed, packet should wait for next channel check.
        logger_->Debug("Channel busy?  : " + tempch);
        logger_->Debug("Number of elements in channel : " + std::to_string(network_->channel_->CountOfPacketsInBuffer()));
        logger_->Info("Channel busy, packet of ID : " + pid + " is still waiting.");**/
        ++t_;
        Activate(GenerateCP());
        active_ = false;
    }
}

void Packet::StateSent() {
    //Packet is pushed to the channel
    logger_->Info("Packet has been put in the channel successfully.");
    //!!!!!!!!
    //Correction here - packets must be able to get to channel in the same moment
    //Collision implementation here, mentioned in line 113.
    network_->channel_->SetBusy(true);
    state_ = State::IN_CHANNEL;
}

void Packet::StateInChannel() {
    //Packet is inside of the channel (it's in channel's vector container for packets too)
    std::string pid = std::to_string(packet_id_);
    //network_->channel_->RemovePacketFromChannel();
    //-----------------------------------------------
    //Correction here - packet can be removed from channel only after end of transmission!!!
    //-----------------------------------------------
    logger_->Info("Packet of ID : " + pid + " passed through channel.");
    if (CheckForErrors()) //Automatically occurance of errors IN CHANNEL is checked
    {
      error_ = true;
    }
    state_ = State::RECEIVED;
    Activate(ctp_time_);
    active_ = false;
}

void Packet::StateReceived() {
    //Packet reaches it's destination - the receiver
    std::string pid = std::to_string(packet_id_);
    RX *CurrentRX = network_->GetRX(devices_id_);
    CurrentRX->SetRXPacket(
            this);   //As packet leaves the channel it's set as a current packet serviced by particular RX
    logger_->Info("Packet of ID : " + pid + " received by RX of ID : " + std::to_string(CurrentRX->GetRXID()));
    if(collision_){
      logger_->Info("Packet of ID : " + pid + " occurred a collision. ACK FALSE.");
      ack_ = false;
      retransmission_ = true;
    }
    else if (error_)
    {
      //If error flag was set in channel - ack flag is set to false and the relay is needed
      logger_->Info("Packet of ID : " + pid + " contains errors. ACK FALSE.");
      ack_ = false;
      retransmission_ = true;
    }
    else {
        //If packet was claimed successfully ack flag set to true
        logger_->Info("Packet of ID : " + pid + " clear. ACK TRUE.");
        ack_ = true;
    }
    state_ = State::SENT_BACK;
}

void Packet::StateSentBack() {
    //Packet is sent back to TX throught channel
    std::string pid = std::to_string(packet_id_);
  //-----------------------------------------------
  //Correction here - packet still in channel
  //-----------------------------------------------
  //network_->channel_->PushPacketToChannel(this);
  //network_->channel_->SetBusy(true); //Channel busy for ACK transmission time
  logger_->Info("Packet of ID : " + pid + " returning with ACK flag set through the channel.");
  state_ = State::CHECK;
  active_ = false; //Active is false because the ACK has to be transmitted through channel here
  Activate(1); //ACK transmission time to TX is being waited
  
}

void Packet::StateCheck() {
    //Packet is set as currently serviced in TX and it's flags are being checked
    TX *CurrentTX = network_->GetTX(devices_id_);
    std::string pid = std::to_string(packet_id_);
    network_->channel_->RemovePacketFromChannel(this); //Packet removed from channel again.
    
    //!!!!!!!!!!!!
    //CORRECTION - Channel cant be set as free here because it gets too loop crazy
    //network_->channel_->SetBusy(false); //Channel free after transmitting ACK
    
    if ((!ack_) && (retransmission_)) {
        //As my channel availability algorithim (A4) says - if retransmission is needed here we go:
        
        logger_->Info("Transmission of packet of ID : " + pid + " failed. Necessary retransmission.");
        ++r_; //"r" counter incremented because it enumerates number of retransmissions
        if (r_ < static_cast<uint32_t>(network_->kMaxRetransmissions)) //If retransmissions peak not reached - one time CRP time is waited
        {
            //The packet state changed back to waiting, because after first channel check, it should wait CP time, not CRP again.
            state_ = State::WAITING;
            
            //!!!!!!!!!!!!!!!
            //Correction here! Retransmission flag has to be resetted after this operations
            
            retransmission_ = false;
            collision_ = false;
            error_ = false;
            ack_ = false;
            Activate(GenerateCRP());
            active_ = false;
            
        }
        else{
            CurrentTX->SetTXPacket(nullptr);
            logger_->Info("Packet of ID : " + pid + " reached retransmission cap.");
            
          //If TX's buffer isn't empty, after deleting packet from system pop new from buffer and set it as current.
          
            if (! CurrentTX->BufferEmpty())
            {
                CurrentTX->SetTXPacket(CurrentTX->PacketFromBuffer());
                CurrentTX->PopFromBuffer();
                network_->IncDeadPacket();
                CurrentTX->GetCurrentPacket()->Activate(time_,false);
                active_ = false;
                terminated_ = true;
            } else if ((CurrentTX->BufferEmpty()) && (CurrentTX->GetCurrentPacket() == nullptr)) {
                //network_->GeneratePacket(CurrentTX->GetTXID(), network_);
                network_->IncDeadPacket();
                active_ = false;
                terminated_ = true;
            }
        }
    }
      //If ack flag set to true, delete packet and output a success transmission message.
    else
      {
      network_->IncSuccessPacket();
        CurrentTX->SetTXPacket(nullptr);
        logger_->Info("Packet of ID : " + pid + " transmitted successfully");
        active_ = false; 
        terminated_ = true;
        //!!!!!!!!!! Correction for channel end
        if (! CurrentTX->BufferEmpty()) {
            CurrentTX->SetTXPacket(CurrentTX->PacketFromBuffer());
            CurrentTX->PopFromBuffer();
            CurrentTX->GetCurrentPacket()->Activate(time_,false);
        }
    }
    //!!!!!!!!!!!!!!!!!!!!!!
    //Setting channel free only at THIS point is mandatory!!!!!
    
  if (network_->channel_->ChannelEmpty()) {network_->channel_->SetBusy(false);}
}


bool Packet::CheckForErrors() {
  TX *CurrentTX = network_->GetTX(devices_id_);
  return (CurrentTX->GeneratorTER->RandZeroOne(0.8));
}

void Packet::GenerateNext() {
  TX *CurrentTX = network_->GetTX(devices_id_);
  auto new_packet = new Packet(devices_id_, network_,time_, agenda_, logger_);
  int cgp = CurrentTX->GeneratorCGP->RandEXP(network_->GetLambda());
  new_packet->Activate(cgp);
}

void Packet::GenerateR() {
  TX *CurrentTX = network_->GetTX(devices_id_);
  R_ = (CurrentTX->GeneratorR->Rand(0,((pow(2,r_))-1)));
}

void Packet::GenerateT() {
  TX *CurrentTX = network_->GetTX(devices_id_);
  T_ = CurrentTX->GeneratorT->Rand(0,((pow(2,t_))-1));
}

int Packet::GenerateCTP(){
  TX *CurrentTX = network_->GetTX(devices_id_);
  return CurrentTX->GeneratorCTP->Rand(1,10);
}

int Packet::GenerateCP() {
  GenerateT();
  return (T_*ctp_time_);
}

int Packet::GenerateCRP() {
  GenerateR();
  return (R_*ctp_time_);
}






