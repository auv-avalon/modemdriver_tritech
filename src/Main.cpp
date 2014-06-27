#include <iostream>
#include <sstream>
#include <modemdriver_tritech/Driver.hpp>
#include <modemdriver_tritech/ModemParser.hpp>
#include <modemdriver_tritech/AckDriver.hpp>
#include <vector>
#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include <boost/circular_buffer.hpp>

int main(int argc, char** argv)
{
        boost::circular_buffer<uint8_t> buffer(1);
        std::cout << "Initial" << std::endl;
        buffer.resize(10);
        //buffer.clear();
        if (buffer.empty()){
            std::cout << "buffer leer" << std::endl;
        }
        if (buffer.full()){
            std::cout << "buffer voll" << std::endl;
        }
        buffer.push_back(0x01);
        std::cout << "Etwas rein gepusht" << std::endl;
        if (buffer.empty()){
            std::cout << "buffer leer" << std::endl;
        }
        if (buffer.full()){
            std::cout << "buffer voll" << std::endl;
        }
        buffer.pop_front();
        std::cout << "Etwas raus genommen" << std::endl;
        if (buffer.empty()){
            std::cout << "buffer leer" << std::endl;
        }
        if (buffer.full()){
            std::cout << "buffer voll" << std::endl;
        }

        
	modemdriver::AckDriver ack_driver;
        modemdriver::Driver driver;
        if (argc > 1){
            std::stringstream ss;
            ss << "serial:///" << argv[1] << ":57600";
            //driver.open(ss.str());
        } else {
            //driver.open("serial:///dev/ttyUSB0:57600");
        }
        ack_driver.setDriver(&driver);
        ack_driver.writePacket(0x01);
        while (1) {
            ack_driver.process();
        }
        //uint8_t buffer[255];
        //for (int i=0; i<255; i++){
        //    buffer[i] = 0x55;
        //}
        //driver.writeSlowly(buffer, 255);
        //while (true){
        //    driver.process();
        //}
        //boost::dynamic_bitset<uint8_t> out;
        //std::vector<uint8_t> buffer;
        //buffer.resize(2);

        //buffer[0] = 0x9C;
        //buffer[1] = 0XAD;
        //modemdriver::Parser::vectorToBitset(buffer, out);
        //std::cout << " =================== " << std::endl;
        //modemdriver::Parser::bitsetToVector(out, buffer);
        //modemdriver::Parser::vectorToBitset(buffer, out); 
        //boost::dynamic_bitset<uint8_t> search_pattern(4, 0x0E);
        //std::cout << search_pattern << std::endl;
        //std::cout << "Gefunden bei" << modemdriver::Parser::searchByte(out, 0x20, 4) << std::endl;
        //std::cout << "Stuffing" << out <<std::endl;
        //modemdriver::Parser::stuffBits(out, 0x30, 4);
        //std::cout << "Nach Stuff " << out << std::endl;
        //modemdriver::Parser::deStuffBits(out, 0x30, 4);
        //std::cout << "Nach Destuff " << out << std::endl;
        //std::cout << "EXTRACT PACKET" << std::endl;




        //std::vector<uint8_t> send_packet;
        //std::vector<uint8_t> packet_out;
        //for (uint8_t i = 0; i<255; i++){
        //    std::cout << "CHECKING " << (int)i << std::endl;
        //    send_packet.clear();
        //    packet_out.clear();
        //    send_packet.push_back(i);
        //    modemdriver::Parser::packData(send_packet);
        //    modemdriver::Parser::extractPacket(send_packet, packet_out);
        //    if (packet_out[0] != i){
        //        std::cout << "ERROR BY " << (int) i << std::endl;
        //    }

        //}




        /*buffer.resize(1);
        int result;
        std::cout << "Firtst Test" << std::endl;
        buffer[0] = 0x30;
       // std::cout << "BYTES BEVOR" << buffer.size() << std::endl;
        //modemdriver::Parser::stuffBits(buffer, 0x03, 4);
        //std::cout << "BYTES AFTER" << buffer.size() << std::endl;
          //  std::cout << "YEHAAA" <<int(buffer[0]) <<std::endl;
        result = modemdriver::Parser::searchByte(buffer, 0x03, 4);
        if (result == 0){
            std::cout << "Correct Result 0" << std::endl;
        } else {
            std::cout << "Enexpected Result expected: 0 and got: " << result << std::endl;
        }
        std::cout << "Second Test" << std::endl;
        buffer[0] = 0x0C;
        result = modemdriver::Parser::searchByte(buffer, 0x03, 4);
        if (result == 2){
            std::cout << "Correct Result 2" << std::endl;
        } else {
            std::cout << "Enexpected Result expected: 2 and got: " << result << std::endl;
        }
        buffer.resize(2);
        buffer[0] = 0x00;
        buffer[1] = 0xC0;
        result = modemdriver::Parser::searchByte(buffer, 0x03, 4);
        if (result == 6){
            std::cout << "Correct Result 6" << std::endl;
        } else {
            std::cout << "Enexpected Result expected: 6 and got: " << result << std::endl;
        }
        buffer.resize(3);
        buffer[0] = 0x55;
        buffer[1] = 0x55;
        buffer[2] = 0x55;
        result = modemdriver::Parser::searchByte(buffer, 0x03, 4);
        if (result == -1){
            std::cout << "Correct Result -1" << std::endl;
        } else {
            std::cout << "Enexpected Result expected: -1 and got: " << result << std::endl;
        }*/





	return 0;
}
