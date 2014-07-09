#include "DriverInterface.hpp"

using namespace modemdriver; 

const char* DriverInterface::RANGE_REQUEST_STRING = {"rng\n"};

void DriverInterface::requestRange()
{
    writeSlowly((uint8_t*)RANGE_REQUEST_STRING,4);
}

