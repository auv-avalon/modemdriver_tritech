#include "Driver.hpp"
#include <iostream>
#include "ModemParser.hpp"
#define BITRATE 40
using namespace modemdriver;
    Driver::Driver()
: iodrivers_base::Driver(500), buffer_(100), receive_buffer(500)
{
    send_last_second = 0;
    last_process = base::Time::now();
}

void Driver::open(std::string const& uri){
    openURI(uri);
}
int Driver::getPacket(std::vector<uint8_t> &out_bytes){
    uint8_t buffer[500];
    //if (hasPacket()){
    try {
        int ret = readPacket(&buffer[0], 500, 1, -1); 
//        std::cout << ret << " Bytes aus dem Driverbase Buffer gelesen" << std::endl;
        for (size_t i = 0; i<ret; i++){
            receive_buffer.push_back(buffer[i]);
        }
    } catch (iodrivers_base::TimeoutError){
        //std::cout << "EVERYTHING IS CATCH HERE! " << std::endl;
    }
    //        std::cout << "receive buffer is not empty" << std::endl;
    int ret_;
    do {
        if (!receive_buffer.empty()){
            ret_ = Parser::extractPacket(receive_buffer, out_bytes);
//            std::cout << "nacht dem extract Packet" << std::endl;
        } else {
            ret_ = 0;
        }
        int to_skip = ret_;
        if (ret_ < 0){
            to_skip = ret_*-1; 
        }
        for (int i=0; i < to_skip; i++){
            receive_buffer.pop_front();
        }
    } while (ret_ < 0);
    return ret_;

}

int Driver::extractPacket(uint8_t const *buffer, size_t buffer_size) const
{
    return buffer_size;
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
    //iodrivers_base::process();
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

