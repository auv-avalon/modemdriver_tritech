#include <iostream>
#include <sstream>
#include <modemdriver_tritech/Driver.hpp>
#include <modemdriver_tritech/ModemParser.hpp>
#include <modemdriver_tritech/AckDriver.hpp>
#include <modemdriver_tritech/AckDriverStats.hpp>
int main(int argc, char** argv)
{
    std::cout << "Direct Data Receiver for the Modem Protocol implemented in this Driver" << std::endl;

    modemdriver::AckDriver ack_driver;
    modemdriver::Driver driver;

    if (argc > 1){
        std::stringstream ss;
        ss << "serial:///" << argv[1] << ":57600";
        driver.open(ss.str());
    } else {
        driver.open("serial:///dev/ttyUSB0:57600");
    }
    ack_driver.setDriver(&driver);
    int count = 0;
    while (1){
        if (ack_driver.hasReceivedData()){
            std::cout << "The Ack Driver tells there is valid Data from the Modem" << std::endl;
            uint8_t data = ack_driver.getNextReceivedData();
            std::cout << "The Char is" << data << std::endl; //TODO Show HEX value
        }
        if (count >= 1000){
            count = 0;
            modemdriver::AckDriverStats s = ack_driver.getDriverStats();
            std::cout << "Last send Ack Bit:" <<(int) s.last_send_ack_bit << std::endl; 
            std::cout << "Last receive Ack Bit:" <<(int) s.last_receive_ack_bit << std::endl; 
            std::cout << "Acked Data Packets:" <<(int) s.acked_data_packets << std::endl; 
            std::cout << "Acked Protocol Packets:" <<(int) s.acked_protocol_packets << std::endl; 
            std::cout << "Received Protocol Packets:" <<(int) s.received_protocol_packets << std::endl; 
            std::cout << "Received Data Packets:" <<(int) s.received_data_packets << std::endl; 
            std::cout << "Last valid packet (since seconds):" << (base::Time::now()-s.last_valid_packet).toSeconds() << std::endl; 
            std::cout << "Retry:" <<(int) s.retries << std::endl; 
            std::cout << "Rejected Packets:" <<(int) s.rejected_packets << std::endl; 


        }
        ack_driver.process();
        count++;
    }
    return 0;
}
