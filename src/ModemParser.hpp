#ifndef _DUMMYPROJECT_MODEM_PARSER_HPP_
#define _DUMMYPROJECT_MODEM_PARSER_HPP_
#include "stdint.h"
#include "inttypes.h"
#include "stdlib.h"
#include "stddef.h"
#include <vector>
#include <boost/dynamic_bitset.hpp>
namespace modemdriver
{
    class Parser
    {
        private:
            static uint8_t reverseByte(uint8_t b);
        public:
            static int extractPacket(const uint8_t *buffer, size_t size, std::vector<uint8_t> &out_bytes);
            static int extractPacket(const std::vector<uint8_t> &bytes, std::vector<uint8_t> &out_bytes);
            static int extractPacket(const boost::dynamic_bitset<uint8_t> &bits, boost::dynamic_bitset<uint8_t> &out);
            static int searchByte(const std::vector<uint8_t> &bytes, uint8_t search_byte, uint8_t bit_mask);
            static int stuffBit(boost::dynamic_bitset<uint8_t> &bits, size_t pos, bool stuff);
            static int stuffBits(std::vector<uint8_t> &bytes, uint8_t forbidden_byte, uint8_t bit_count);
            static int stuffBits(boost::dynamic_bitset<uint8_t> &bits, uint8_t forbidden_byte, uint8_t bit_count);
            static int stuffBits(boost::dynamic_bitset<uint8_t> &bits, boost::dynamic_bitset<uint8_t> forbidden_bits);
            static int searchByte(const boost::dynamic_bitset<uint8_t> &bits, uint8_t search_byte, uint8_t byte_mask);
            static int searchByte(const boost::dynamic_bitset<uint8_t> &bits, const boost::dynamic_bitset<uint8_t> &search_bits);
            static int searchByte(const boost::dynamic_bitset<uint8_t> &bits, const boost::dynamic_bitset<uint8_t> &search_bits, size_t start_pos);
            static int reverseSearchByte(const boost::dynamic_bitset<uint8_t> &bits, const boost::dynamic_bitset<uint8_t> &search_bits, size_t start_pos);
            static int searchByte(const boost::dynamic_bitset<uint8_t> &bits, uint8_t search_byte, uint8_t byte_mask, size_t start_pos);
            static int vectorToBitset(const std::vector<uint8_t> &src, boost::dynamic_bitset<uint8_t> &dst);
            static int bitsetToVector(const boost::dynamic_bitset<uint8_t> &src, std::vector<uint8_t> &dst);
        
            static int deStuffBits(std::vector<uint8_t> &bytes, uint8_t forbidden_byte, uint8_t bit_count);
            static int deStuffBits(boost::dynamic_bitset<uint8_t> &bits, uint8_t forbidden_byte, uint8_t bit_count);
            static int deStuffBits(boost::dynamic_bitset<uint8_t> &bits, boost::dynamic_bitset<uint8_t> forbidden_bits);
            static int deStuffBit(boost::dynamic_bitset<uint8_t> &bits, size_t pos);
            static bool checkParity(const boost::dynamic_bitset<uint8_t> &data);
            static int packData(std::vector<uint8_t> &data);
            static int packData(boost::dynamic_bitset<uint8_t> &bits);

            static bool parseDistance(const uint8_t *buffer, const size_t len, double &distance);

    };
}
#endif
