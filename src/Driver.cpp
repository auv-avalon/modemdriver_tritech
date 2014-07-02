#include "Driver.hpp"
#include <iostream>
#include "ModemParser.hpp"
#define BITRATE 40
using namespace modemdriver;
    Driver::Driver()
: iodrivers_base::Driver(500), buffer_(100)
{
    send_last_second = 0;
    last_process = base::Time::now();
}

void Driver::open(std::string const& uri){
    openURI(uri);
}
int Driver::getPacket(std::vector<uint8_t> &out_bytes){
    uint8_t buffer[10];
    if (hasPacket()){
        std::cout << "In the Driver there is a Packet a valid Packet from the Modem" << std::endl;
       int ret = readPacket(&buffer[0], 10, 1, -1); 
       return Parser::extractPacket(&buffer[0], ret, out_bytes);
    }
    //throw std::runtime_error("There is no Packet");
    return 0;
}

int Driver::extractPacket(uint8_t const *buffer, size_t buffer_size) const
{
    int ret = 0;
    std::vector<uint8_t> data;
    if (buffer_size){
        std::cout << "Debug Ausgabe: Auf dem Modem kamen " << buffer_size << "bytes an" << std::endl;
        ret = Parser::extractPacket(buffer, buffer_size, data);

        double distance;
        if(Parser::parseDistance(buffer, buffer_size,distance))
        {
            printf("got distance information: %f\n",distance);
        }

        if (ret == 0) {
            std::cout << "There is something like a packet but it is not complete" << std::endl;
        } else if ( ret < 0) {
            std::cout << "There is some waste in the buffer. " << ret << "bytes are skipped" << std::endl;
        } else {
            std::cout << "There is a Packet in the buffer. The Data in buffer has the length " << buffer_size << std::endl;
            std::cout << "The payload in the packet has the length "<< data.size() << std::endl;
        }
    }
    //return 0 zum warten
    //negativ zum skippen
    //positiv zum lesen
    return ret;
}
void Driver::writeSlowly(uint8_t const *send_buffer, size_t buffer_size){
    std::cout << "write somethind slowly" << std::endl;
    //if (buffer_.full()){
    //    buffer_.resize(buffer.size()+ 100); //ugly
    //}
    for (int i=0; i < buffer_size; i++){
        buffer_.push_back(send_buffer[i]);
    }
}

size_t Driver::process(){
    if ((int) last_process.toSeconds() != (int) base::Time::now().toSeconds()){
        last_process = base::Time::now();
        send_last_second = 0;
    }
    uint8_t send_buffer[BITRATE];
    size_t size = 0;
    while (send_last_second < BITRATE && !buffer_.empty()){
        send_buffer[size++] = buffer_[0];
        std::cout << "SEND: " << buffer_[0] << std::endl;
        buffer_.pop_front();
        send_last_second += 8;
    }
    if(size){
        std::cout << "write Packet" << std::endl;
        writePacket(send_buffer, size);
    } 
    return buffer_.size();
}

