#ifndef _DUMMYPROJECT_DRIVER_HPP_
#define _DUMMYPROJECT_DRIVER_HPP_
#include <iodrivers_base/Driver.hpp>
#include <vector>
#include <boost/circular_buffer.hpp>
#include <vector>
#include "ModemDriverTypes.hpp"
#define BUFFER_SIZE 1000
namespace modemdriver
{
    enum DriverInternalState{
        IDLE,
        PENDING_ACK_FOR_A_SEND_MESSAGE,
        ACKED_RECEIVED_DATA};


    class Driver : public iodrivers_base::Driver
    {
        private:
            int extractPacket (uint8_t const *buffer, size_t buffer_size) const;
            base::Time last_process;
            boost::circular_buffer<uint8_t> buffer;
            boost::circular_buffer<uint8_t> send_que;
            size_t send_last_second;
            bool last_send_sequenz_number;
            bool last_recv_sequenz_number;
            std::vector<uint8_t> last_packet;
            DriverInternalState state;
            base::Time last_send;
        public: 
            Driver();
            void open(std::string const& uri);
            size_t process();
            void writeSlowly(uint8_t const *buffer, size_t buffer_size);
            void writePacket_(uint8_t payload);
            void writeModemMessage(ModemMessageHead head, std::vector<uint8_t> payload);
    };

}
#endif 
