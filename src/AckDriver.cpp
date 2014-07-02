#include "AckDriver.hpp"
#include "Driver.hpp"
#include "ModemParser.hpp"
//The Time between two send retries is calculated as
//random from 0 to MAX_WAIT_PER_FACTOR*(send_retries%MAX_WAIT_FACTOR) in Ms
#define MAX_WAIT_PER_FACTOR 2000
#define MAX_WAIT_FACTOR 20
using namespace modemdriver;
AckDriver::AckDriver()
: payload_buffer(200), received_data(200) {
    state = INITIAL;
    send_retries = 0;
}
void AckDriver::setDriver(DriverInterface* driver_){
    driver = driver_;
}

void AckDriver::writePacket(uint8_t payload){
    if (payload & 0x80){
        throw std::runtime_error("too big payload. Payload have just 7 bits");
    }
    //if (payload_buffer.full()){
    //    throw std::runtime_error("Full Payloadbuffer");
    //} else {
        payload_buffer.push_back(payload);
    //}
}
uint8_t AckDriver::getNextReceivedData(){
    if (!received_data.empty()){
        uint8_t data = received_data[0];
        received_data.pop_front();
        return data;
    } else {
        throw std::runtime_error("No Received Data");
    }
}
bool AckDriver::hasReceivedData(){
    return !received_data.empty();
}
size_t AckDriver::process(){
    std::vector<uint8_t> packet;
    int length = driver->getPacket(packet);
    if (length){
        std::cout << "In the Ack Driver there is a valid Packet from the Modem" << std::endl;
        if ((bool) last_received_ack_bit != (bool) packet[0]&80) {
            state = RECEIVED_DATA;
            last_received_ack_bit = packet[0]&80;
            uint8_t received_payload = packet[0]&0x7FF; 
            if (length > 1) {
                throw std::runtime_error("Messages longer then one byte are not implemented yet!");
            } 
            if (received_payload){
                received_data.push_back(received_payload);
            }
        } 
    }
    switch (state){
        case INITIAL:
            std::cout << "State is Initial" << std::endl;
            //Wenn was im Payload buffer dann kommunikation beginnen
            //mit ack_bit 1
            if (!payload_buffer.empty()){ 
                pending_message.resize(1);
                pending_message[0] = (payload_buffer[0]|0x80); //set the ack_bit to initial 1
                payload_buffer.pop_front();
                Parser::packData(pending_message);
                state = PENDING_ACK;
                send_retries = 0;
            }
        case PENDING_ACK:
            //Pending message zufaellig nochmal senden
            if (send_retries == 0){
                driver->writeSlowly(&(pending_message[0]), pending_message.size());
                last_retry = base::Time::now();
                send_retries++;
                current_wait = (std::rand()%(200*send_retries));
            } else {
                if (base::Time::now().toMilliseconds() - last_retry.toMilliseconds() >= current_wait){ //TODO check send_buffersize
                    std::cout << "current wait abgelaufen" << std::endl;
                    driver->writeSlowly(&(pending_message[0]), pending_message.size());
                    send_retries++;
                    current_wait = (std::rand()%(MAX_WAIT_PER_FACTOR*(send_retries%MAX_WAIT_FACTOR)));
                    last_retry = base::Time::now();
                }
            }
            break;
        case RECEIVED_DATA:
            pending_message.resize(1);
            uint8_t payload = 0;
            if (!payload_buffer.empty()){
                payload = payload_buffer[0];
                payload_buffer.pop_front();
            }
            if (last_received_ack_bit){
                payload = payload | 0x80;
            }
            pending_message[0] = payload;
            Parser::packData(pending_message);
            state = PENDING_ACK;
            send_retries = 0;
            break;
    }
    driver->process();
    return payload_buffer.size();
}

void AckDriver::requestRange() const{
    assert(driver);
    driver->requestRange();
}
