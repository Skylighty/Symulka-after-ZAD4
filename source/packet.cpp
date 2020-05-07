#include <string>
#include "packet.h"
#include "channel.h"
#include "wireless_network.h"
#include <ctime>
#include <cstdlib>
#include <random>
#include "rx.h"
#include "tx.h"

Packet::Packet(uint32_t did, WirelessNetwork *network, size_t time, Agenda *agenda, Logger* log) : Process(time, network, agenda) {
    state_ = State::CREATED;
    agenda_ = agenda;
    error_ = false;
    ack_ = false;
    packet_id_ = (network->GetPacketCount())+1;
    network->IncPacketCount();
    devices_id_ = did;
    logger_ = log;
    active_ = false;
    network_ = network;
    retransmission_ = false;
    collision_ = false;
    terminated_ = false;
    r_ = 0;
    t_ = 0;
    R_ = 0;
    T_ = 0;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1,10);
    std::uniform_int_distribution<int> cp_distribution(30,40);
    std::uniform_int_distribution<int> cgp_distribution (60,100);
    crp_time_ = distribution(generator);
    cp_time_ = cp_distribution(generator);
    cgp_time_ = cgp_distribution(generator);
    ctp_time_ = distribution(generator);
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


void Packet::StateCreated() {
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
}

void Packet::StateWaiting() {
    //Here, the packet checks if the channel is busy, automatically incrementing "t" parameter - counter of channel checks
    std::string pid = std::to_string(packet_id_);
    ++t_;
    if (!network_->channel_->ChannelBusy()) {
        //If channel ain't busy, the packet is passed to channel, simultaneously "t" counter restarts back to 0.
      //-----------------------------------------------
      //Correction here - channel should be set busy in StateSent, so we can implement collision
      //-----------------------------------------------
        //network_->channel_->SetBusy(true); //Channel set to busy if we start transmitting
        logger_->Info("Channel free, packet of ID : " + pid + " passed to the channel.");
        t_ = 0;
        state_ = State::SENT;
    } else {
        //If channel busy, state is not being changed, packet should wait for next channel check.
        logger_->Info("Channel busy, packet of ID : " + pid + " is still waiting.");
        Activate(CalculateCPTime());
        active_ = false;
    }
}

void Packet::StateSent() {
    //Packet is pushed to the channel
    logger_->Info("Packet has been put in the channel successfully.");
    network_->channel_->PushPacketToChannel(this);
    //Collision implementation here, mentioned in line 113.
    network_->channel_->SetBusy(true);
    state_ = State::IN_CHANNEL;
}

void Packet::StateInChannel() {
    //Packet is inside of the channel (it's in channel's vector container for packets too)
    std::string pid = std::to_string(packet_id_);
    if (CheckForErrors()) //Automatically occurance of errors IN CHANNEL is checked
        error_ = true;
    if (CheckForCollision()) //Same goes for collisions (more than one packet in channel at the same moment)
        collision_ = true;
    //network_->channel_->RemovePacketFromChannel();
    //-----------------------------------------------
    //Correction here - packet can be removed from channel only after end of transmission!!!
    //-----------------------------------------------
    logger_->Info("Packet of ID : " + pid + " passed through channel.");
    //network_->channel_->RemovePacketFromChannel();  //Packet is removed from channel's packet container
    //network_->channel_->SetBusy(false); //Channel set to free again
    state_ = State::RECEIVED;
    active_ = false;
    Activate(ctp_time_);
}

void Packet::StateReceived() {
    //Packet reaches it's destination - the receiver
    std::string pid = std::to_string(packet_id_);
    RX *CurrentRX = network_->GetRX(devices_id_);
    CurrentRX->SetRXPacket(
            this);   //As packet leaves the channel it's set as a current packet serviced by particular RX
    logger_->Info("Packet of ID : " + pid + " received by RX of ID : " + std::to_string(CurrentRX->GetRXID()));
    if (error_) {
        //If error flag was set in channel - ack flag is set to false and the relay is needed
        logger_->Info("Packet of ID : " + pid + " contains errors. ACK FALSE.");
        ack_ = false;
        retransmission_ = true;
    }
    else if(collision_){
      logger_->Info("Packet of ID : " + pid + " occurred a collision. ACK FALSE.");
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

//TODO - There can't be just a one packet generated after this state because TX's buffer'd have gone unnecessary
//TODO - So best way to generate is generation time check after every state - ask about it.
void Packet::StateCheck() {
    //Packet is set as currently serviced in TX and it's flags are being checked
    TX *CurrentTX = network_->GetTX(devices_id_);
    std::string pid = std::to_string(packet_id_);
    CurrentTX->SetTXPacket(this);
    network_->channel_->RemovePacketFromChannel(); //Packet removed from channel again.
    network_->channel_->SetBusy(false); //Channel free after transmitting ACK
    if ((!ack_) && (retransmission_)) {
        //As my channel availability algorithim (A4) says - if retransmission is needed here we go:
        logger_->Info("Transmission of packet of ID : " + pid + " failed. Necessary retransmission.");
        ++r_; //"r" counter incremented because it enumerates number of retransmissions
        if (r_ < network_->kMaxRetransmissions) //If retransmissions peak not reached - one time CRP time is waited
        {
            //The packet state changed back to waiting, because after first channel check, it should wait CP time, not CRP again.
            Activate(crp_time_);
            state_ = State::WAITING;
            active_ = false;
        }
        else{
            //If ack flag set to true, delete packet and output a success transmission message.
            CurrentTX->SetTXPacket(nullptr);
            logger_->Info("Packet of ID : " + pid + " reached retransmission cap.");
            if (! CurrentTX->BufferEmpty()
                ) //If TX's buffer isn't empty, after deleting packet from system pop new from buffer and set it as current.
            {
                CurrentTX->SetTXPacket(CurrentTX->PacketFromBuffer());
                CurrentTX->PopFromBuffer();
                network_->IncDeadPacket();
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
    else {
      network_->IncSuccessPacket();
        CurrentTX->SetTXPacket(nullptr);
        logger_->Info("Packet of ID : " + pid + " transmitted successfully");
        if (! CurrentTX->BufferEmpty()) {
            CurrentTX->SetTXPacket(CurrentTX->PacketFromBuffer());
            CurrentTX->PopFromBuffer();
            active_ = false;
            terminated_ = true;
        } else if ((CurrentTX->BufferEmpty()) && (CurrentTX->GetCurrentPacket() == nullptr)) {
            //network_->GeneratePacket(CurrentTX->GetTXID(), network_);
            active_ = false;
            terminated_ = true;
        }
    }
}


bool Packet::CheckForErrors() {
    //TODO - Implement checking for errors - for now just a rand TER error
  srand(time(nullptr));
    int chance = (rand() % 100)+1;
    if (chance <= 20) {return true;}
    else {return false;}
}

bool Packet::CheckForCollision() {
  //TODO - Separate states between channel check and sending
  if ((network_->channel_->CountOfPacketsInBuffer()) > 1) {
    return true;
  } else
    return false;
}

void Packet::GenerateNext() {
  auto new_packet = new Packet(devices_id_, network_,time_,agenda_, logger_);
  new_packet->Activate(new_packet->cgp_time_);
}





