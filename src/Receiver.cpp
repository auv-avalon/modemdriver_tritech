#include <iostream>
#include <sstream>
#include <modemdriver_tritech/Driver.hpp>
#include <modemdriver_tritech/ModemParser.hpp>
#include <modemdriver_tritech/AckDriver.hpp>
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
    while (1){
        if (ack_driver.hasReceivedData()){
            std::cout << "The Ack Driver tells there is valid Data from the Modem" << std::endl;
            uint8_t data = ack_driver.getNextReceivedData();
            std::cout << "The Char is" << data << std::endl; //TODO Show HEX value
        }
        ack_driver.process();
    }
    return 0;
}
