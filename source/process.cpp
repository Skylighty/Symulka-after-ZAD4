#include "process.h"

Process::Process(size_t time, WirelessNetwork *wireless_network, Agenda *agenda) : time_(time), wireless_network_(wireless_network), agenda_(agenda) {}

//So the basic thing about this function is to update the local time, and push "frozen" packet back to Agenda on it's place
void Process::Activate(size_t time, bool relative) {
    if (relative) {
        time_ += time;
    } else {
        time_ = time;
    }
    agenda_->push(this);
}
