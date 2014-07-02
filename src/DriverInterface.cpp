#include "DriverInterface.hpp"

using namespace modemdriver; 

const char* DriverInterface::RANGE_REQUEST_STRING = {"rng\r"};

void DriverInterface::requestRange()
{
    writeSlowly((uint8_t*)RANGE_REQUEST_STRING,sizeof(RANGE_REQUEST_STRING));
}

