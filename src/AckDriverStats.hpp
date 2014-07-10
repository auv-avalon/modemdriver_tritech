#ifndef _DUMMYPROJECT_ACKDRIVERSTATS_HPP_
#define _DUMMYPROJECT_ACKDRIVERSTATS_HPP_
#include <inttypes.h>
#include <base/Time.hpp>
namespace modemdriver
{
    struct AckDriverStats {
        //The ack bit in the last send packet
        uint8_t last_send_ack_bit;
        //The ack bit in the last received packet
        uint8_t last_receive_ack_bit;
        //The Count of real transfered data packets
        uint8_t acked_data_packets;
        uint8_t acked_protocol_packets;
        uint8_t received_protocol_packets;
        uint8_t received_data_packets;
        //The Time of the last valid packet
        //Valid Packets are every packet
        //Protocol packets, too.
        base::Time last_valid_packet;
        //Current retry cout for the current send packet
        uint8_t retries;
        //How many packets are rejected because of a wrong ack bit,
        //since the last acepted packet.
        uint8_t rejected_packets;
    };
};
#endif 
