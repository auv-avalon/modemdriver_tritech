#ifndef _DUMMYPROJECT_DRIVER_HPP_
#define _DUMMYPROJECT_DRIVER_HPP_
#include <iodrivers_base/Driver.hpp>
#include <vector>
#include <boost/circular_buffer.hpp>
#include <vector>
#include "ModemDriverTypes.hpp"
#include "DriverInterface.hpp"
#define BUFFER_SIZE 1000
namespace modemdriver
{
    class Driver : public iodrivers_base::Driver, public DriverInterface
    {
        private:
            int extractPacket (uint8_t const *buffer, size_t buffer_size) const;
            base::Time last_process;
            boost::circular_buffer<uint8_t> buffer_;
            boost::circular_buffer<uint8_t> receive_buffer;
            size_t send_last_second;
            bool last_send_sequenz_number;
            bool last_recv_sequenz_number;
            bool current_sending;
            std::vector<uint8_t> last_packet;
            std::vector<uint8_t> _internal_buffer_;
            base::Time last_send;
        public: 
            Driver();
            void open(std::string const& uri);
            virtual size_t process();
            virtual void writeSlowly(uint8_t const *buffer, size_t buffer_size);
            virtual int getPacket(std::vector<uint8_t> &out);
    };

}
#endif 
