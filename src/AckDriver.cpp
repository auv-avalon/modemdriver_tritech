#include "AckDriver.hpp"
#include "Driver.hpp"
#include "ModemParser.hpp"
#include "time.h"
//The Time between two send retries is calculated as
//random from 0 to MAX_WAIT_PER_FACTOR*(send_retries%MAX_WAIT_FACTOR) in Ms
#define MAX_WAIT_PER_FACTOR 300
#define MAX_WAIT_FACTOR 10
using namespace modemdriver;
AckDriver::AckDriver()
: payload_buffer(200), received_data(200) {
    std::srand(time(NULL));
    state = INITIAL;
    send_retries = 0;
    last_received_ack_bit = 0;
    last_send_ack_bit = 0;

    acked_data_packets = 0;
    acked_protocol_packets = 0;
    received_data_packets = 0;
    received_protocol_packets = 0;
    retries = 0;
    rejected_packets = 0;
}
void AckDriver::setDriver(DriverInterface* driver_){
    driver = driver_;
}

void AckDriver::writePacket(uint8_t payload){
    if (payload & 0x80){
        throw std::runtime_error("too big payload. Payload have just 7 bits");
    }
    if (payload_buffer.full()){
        throw std::runtime_error("Full Payloadbuffer");
    } else {
        payload_buffer.push_back(payload);
    }
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
    if (length && packet.size()){
        std::cout << "valid Packet" << std::endl;
        //std::cout << "In the Ack Driver there is a valid Packet from the Modem" << std::endl;
        //std::cout << "The data vector has the length" << packet.size() << std::endl;
        //std::cout << "The data on the first byte is: " << std::hex << (int)packet[0] << std::endl;
        //std::cout << "In the package the bit was:" << (bool)  (packet[0]&0x80) << std::endl; 
        //std::cout << "In the driver the bit was:" << (bool) last_received_ack_bit << std::endl;
        if ((bool) last_received_ack_bit != (bool) (packet[0]&0x80)) {
            last_valid_packet = base::Time::now();
            rejected_packets = 0;
            if (state == PENDING_ACK_DATA){
                acked_data_packets++;
            } else if (state == PENDING_ACK_PROTOCOL){
                acked_protocol_packets++;
            }
            state = RECEIVED_DATA;
            last_received_ack_bit = packet[0]&0x80;
            uint8_t received_payload = packet[0]&0x7F; 
            if (packet.size() > 1) {
                std::cout << "Messeges longer then one byte are not implemented yet" << packet.size() << std::endl;
                //throw std::runtime_error("Messages longer then one byte are not implemented yet!");
            } 
            if (received_payload){
                received_data.push_back(received_payload);
                received_data_packets++;
            } else {
                received_protocol_packets++;
            }
        } else {
            rejected_packets++;
        } 
    }
    switch (state){
        case INITIAL:
            //Wenn was im Payload buffer dann kommunikation beginnen
            //mit ack_bit 1
            if (!payload_buffer.empty()){ 
                pending_message.resize(1);
                pending_message[0] = (payload_buffer[0]|0x80); //set the ack_bit to initial 1
                last_send_ack_bit = 1;
                payload_buffer.pop_front();
                Parser::packData(pending_message);
                state = PENDING_ACK_DATA;
                send_retries = 0;
            }
            break;
        case PENDING_ACK_DATA:
        case PENDING_ACK_PROTOCOL:
            //Pending message zufaellig nochmal senden
            if (send_retries == 0){
                driver->writeSlowly(&(pending_message[0]), pending_message.size());
                last_retry = base::Time::now();
                send_retries++;
                std::cout << "Max wait per factor " << MAX_WAIT_PER_FACTOR << std::endl;
                std::cout << "send retries " << (int)send_retries << std::endl;
                std::cout << "MAX_WAIT FActor" << MAX_WAIT_FACTOR << std::endl;
                std::cout << "MAX_WAIT_PER_FACTOR * send_retries" << (MAX_WAIT_PER_FACTOR*send_retries)%20 << std::endl;
                std::cout << "Alles zusammen" << MAX_WAIT_PER_FACTOR*(send_retries)%MAX_WAIT_FACTOR << std::endl;
                int wait_factor = send_retries;
                if (send_retries > MAX_WAIT_FACTOR){
                    wait_factor = MAX_WAIT_FACTOR;
                }
                current_wait = (std::rand()%(MAX_WAIT_PER_FACTOR* wait_factor));//%MAX_WAIT_FACTOR)));
            } else {
                if (base::Time::now().toMilliseconds() - last_retry.toMilliseconds() >= current_wait){ //TODO check send_buffersize
                    driver->writeSlowly(&(pending_message[0]), pending_message.size());
                    send_retries++;
                    int wait_factor = send_retries;
                    if (send_retries > MAX_WAIT_FACTOR){
                        wait_factor = MAX_WAIT_FACTOR;
                    }
                    current_wait = (std::rand()%(MAX_WAIT_PER_FACTOR* wait_factor));//%MAX_WAIT_FACTOR)));
                    last_retry = base::Time::now();
                }
            }
            break;
        case RECEIVED_DATA:
            pending_message.resize(1);
            uint8_t payload = 0;
            if (!payload_buffer.empty()){
                state = PENDING_ACK_DATA;
                payload = payload_buffer[0];
                payload_buffer.pop_front();
            } else {
                state = PENDING_ACK_PROTOCOL;
            }
            if (!last_send_ack_bit){
                payload = payload | 0x80;
                last_send_ack_bit = 1;
            } else {
                last_send_ack_bit = 0;
            }
            pending_message[0] = payload;
            Parser::packData(pending_message);
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

AckDriverStats AckDriver::getDriverStats(){
    AckDriverStats stats;
    stats.last_send_ack_bit = last_send_ack_bit;
    stats.last_receive_ack_bit = last_received_ack_bit;
    stats.acked_data_packets = acked_data_packets;
    stats.acked_protocol_packets = acked_protocol_packets;
    stats.received_data_packets = received_data_packets;
    stats.received_protocol_packets = received_protocol_packets;
    stats.last_valid_packet = last_valid_packet;
    stats.retries = send_retries;
    stats.rejected_packets = rejected_packets;
    return stats;
}
