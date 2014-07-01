#include <iostream>
#include <sstream>
#include <string>
#include <modemdriver_tritech/Driver.hpp>
#include <modemdriver_tritech/ModemParser.hpp>
#include <modemdriver_tritech/AckDriver.hpp>
int main(int argc, char** argv)
{
    std::cout << "Direct Data Sender for the Modem Protocol implemented in this Driver" << std::endl;

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
    std::string input;
    int digits;
    while (1){
        do {

            std::cout << "Please Enter a Number to send (from 1  to 127)" << std::endl;
            std::cin >> input;
            std::stringstream sstr(input);
            sstr >> digits;
        } while(!(digits >= 1 && digits <=127));
        std::cout << "Send " << digits << "now" << std::endl;

        ack_driver.writePacket((uint8_t) digits); //TODO STDin benutzen
        while (1){
            ack_driver.process();
        }
    }
    return 0;
}
