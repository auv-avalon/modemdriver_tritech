#ifndef _DUMMYPROJECT_ACKDRIVER_HPP_
#define _DUMMYPROJECT_ACKDRIVER_HPP_
#include "Driver.hpp"
#include "AckDriverStats.hpp"
namespace modemdriver
{
    enum DriverInternalState{
        INITIAL,
        PENDING_ACK_DATA,
        PENDING_ACK_PROTOCOL,
        RECEIVED_DATA
    };
    class AckDriver 
    {
        private:
            DriverInterface* driver;
            int current_wait;
            uint8_t send_retries;
            std::vector<uint8_t> pending_message;
            uint8_t last_received_ack_bit;
            uint8_t last_send_ack_bit;
            base::Time last_retry;
            boost::circular_buffer<uint8_t> payload_buffer;
            DriverInternalState state;
            boost::circular_buffer<uint8_t>  received_data;

            size_t acked_data_packets;
            size_t acked_protocol_packets;
            size_t received_data_packets;
            size_t received_protocol_packets;
            base::Time last_valid_packet;
            size_t retries;
            size_t rejected_packets;
        public:
            AckDriver();
            void setDriver(DriverInterface* driver);
            size_t process();
            void writePacket(uint8_t payload);
            uint8_t getNextReceivedData();
            bool hasReceivedData();
            void requestRange() const;
            AckDriverStats getDriverStats();
    };
}
#endif 
