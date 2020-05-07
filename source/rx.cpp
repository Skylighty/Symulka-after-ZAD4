#include "rx.h"

RX::RX(int id) {
    rx_id_ = id;
    current_packet_ = nullptr;
}

RX::~RX() {

}
