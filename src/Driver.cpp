#include "Driver.hpp"
#include <iostream>
#define BITRATE 40
    using namespace modemdriver;
    Driver::Driver()
        : iodrivers_base::Driver(500)
    {
        send_last_second = 0;
        last_process = base::Time::now();
}

void Driver::open(std::string const& uri){
    openURI(uri);
}

int Driver::extractPacket(uint8_t const *buffer, size_t buffer_size) const
{
    //return 0 zum warten
    //negativ zum skippen
    //positiv zum lesen
    return buffer_size;
}
void Driver::writeSlowly(uint8_t const *send_buffer, size_t buffer_size){
    buffer.resize(buffer.size()+ buffer_size);
    for (int i=0; i < buffer_size; i++){
        buffer.push_back(send_buffer[i]);
    }
}
void Driver::writePacket_(uint8_t payload){
    uint8_t first = 0x30;
    uint8_t second = 0x0C;
    first = first | (payload >> 4);
    second = second | (payload << 4);
    uint8_t buffer[2];
    buffer[0] = first;
    buffer[1] = second;
    writeSlowly(buffer, 2);

}


size_t Driver::process(){
    if ((int) last_process.toSeconds() != (int) base::Time::now().toSeconds()){
        last_process = base::Time::now();
        send_last_second = 0;
    }
    uint8_t send_buffer[BITRATE];
    size_t size = 0;
    while (send_last_second < BITRATE && !buffer.empty()){
        send_buffer[size++] = buffer[0];
        std::cout << "SEND: " << buffer[0] << std::endl;
        buffer.pop_front();
        send_last_second += 8;
    }
    if(size){
        writePacket(send_buffer, size);
    } 
    return buffer.size();
}

void Driver::writeModemMessage(ModemMessageHead head, std::vector<uint8_t> payload){
}
