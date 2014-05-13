#ifndef __MODEMDRIVER_TYPES__
#define __MODEMDRIVER_TYPES__
struct ModemMessageHead {
    bool sequenznumber:1;
    uint8_t data:7;
};
#endif
