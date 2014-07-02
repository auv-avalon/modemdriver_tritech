#ifndef _DUMMYPROJECT_DRIVERINTERFACE_HPP_
#define _DUMMYPROJECT_DRIVERINTERFACE_HPP_

#include <inttypes.h>
#include <vector>
#include <cstddef>

namespace modemdriver {
    class DriverInterface
    {
        public:
            virtual size_t process() = 0;
            virtual void writeSlowly(uint8_t const *buffer, size_t buffer_size) = 0;
            virtual int getPacket(std::vector<uint8_t> &out) = 0;
            virtual ~DriverInterface() {}
            void requestRange();
            static const char* RANGE_REQUEST_STRING;
    };
}
#endif 
