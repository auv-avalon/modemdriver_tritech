#include "ModemParser.hpp"
#include <iostream>
#include <iterator>
#include <sstream>
#include <string.h>
#include <stdio.h>

//Attention: If the flags are conflicting you can reach a endless recursion 
#define SYN_FLAG 0x30
#define SYN_BIT_COUNT 4
#define END_FLAG 0xC0
#define END_BIT_COUNT 4
#define MAX_PACKET_BITS 24
using namespace modemdriver;
int Parser::extractPacket(const uint8_t *buffer, size_t size, std::vector<uint8_t> &out_bytes){
    std::vector<uint8_t> bytes(&buffer[0], &buffer[size-1]);
    return extractPacket(bytes, out_bytes);
}
int Parser::extractPacket(const std::vector<uint8_t> &bytes, std::vector<uint8_t> &out_bytes){
    boost::dynamic_bitset<uint8_t> bits;
    boost::dynamic_bitset<uint8_t> out_bits;
    vectorToBitset(bytes, bits);
    int ret =  extractPacket(bits, out_bits);
    bitsetToVector(out_bits, out_bytes);
    return ret;
}
int Parser::extractPacket(const boost::dynamic_bitset<uint8_t> &bits, boost::dynamic_bitset<uint8_t> &out){ 
    //std::cout << "Starting extract Packt" << std::endl;
    int found_start = searchByte(bits, SYN_FLAG, SYN_BIT_COUNT);
    if (found_start < 0){
        //std::cout << " ... abort. No Start-Flag in the while data" << std::endl;
        //There is no start flag
        //The whole data is waste and is to be skipped
        return (bits.size()/8*-1);
    } else if (found_start > 7){
        //std::cout << "... abort. No Start-Flag in the first byte" << std::endl;
        //The start flag is not found in the first byte
        //The bytes until the start-flag-byte is waste and to be skiped
        return (found_start/8*-1);
    }
    //std::cout << "! Found the Start-Flag at " << found_start << std::endl;
    //Start flag found in the first byte
    int found_end = searchByte(bits, END_FLAG, END_BIT_COUNT, found_start+SYN_BIT_COUNT);
    if (found_end < 0){
        //No End was found
        if (bits.size()-found_start > MAX_PACKET_BITS){
            //If there had to be an end because the packet is so too big
            //The whole data is waste and is to be skipped
            //std::cout << "... abort. No End-Flag was found. Skipping data" << std::endl;
            return (bits.size()/8*-1);
        } else { 
            //std::cout << "... abort. No End-Flag was found. Waiting for more data ..." << std::endl;
            //its could become a packet so we waitung for more data
            return 0;
        }
    }
    //std::cout << "! Found the End-Flag at " << found_end << std::endl;
    /*if ((found_end-found_start+END_BIT_COUNT)%8 != 0){
        std::cout << "...abort. Data is not a byte format. We have " << (found_end-found_start+END_BIT_COUNT) << " data bytes." << std::endl;
        
        //Its can't be a packet just whole bytes can be send by sender
        //The date until the detected end have to be waste and skipped
        return ((found_end+END_BIT_COUNT)/8*-1);
    }*/
    //Possible end found
    //Check the parity
    out = boost::dynamic_bitset<uint8_t>(bits);
    out >>= found_start+SYN_BIT_COUNT;
    out.resize(found_end-found_start-SYN_BIT_COUNT);
    //Destuffing
    //std::cout << "! Destuff " << deStuffBits(out, 0x55, 8) << " Surface bits" << std::endl;
    //std::cout << "! Destuff " << deStuffBits(out, 0xAA, 8) << " Surface bits" << std::endl;
    deStuffBits(out, SYN_FLAG, SYN_BIT_COUNT);
    deStuffBits(out, END_FLAG, SYN_BIT_COUNT);

    if (!checkParity(out)){
        //std::cout << "...abort. The parity check failed. This is the data:"<< out << std::endl;
        //The Parity is incorrect
        //Its can't be a correct Packet
        //The whole packet is waste and is to be skipped
        return ((found_end+END_BIT_COUNT)/8*-1);
    };
    //std::cout << "! Parity Check done" << std::endl;
    out.resize(out.size()-1);
    if (out.size()%8 != 0){
        //std::cout << "...abort. Its not a byte format" << std::endl;
        return ((found_end+END_BIT_COUNT)/8*-1);
    }
    //std::cout << "Found a packet and extracted it" << std::endl;
    return ((found_end+END_BIT_COUNT)/8);
}

bool Parser::checkParity(const boost::dynamic_bitset<uint8_t> &data){
    return data.count()%2==0;
}

int Parser::vectorToBitset(const std::vector<uint8_t> &src, boost::dynamic_bitset<uint8_t> &dst){
    std::stringstream ss;
    for (int i=src.size()-1; i>=0; i--){
        uint8_t mask = 1;
        for (int a=0; a<8; a++){
            ss << (((mask<<a) & src[i])>>a);
        }
    }
    dst = boost::dynamic_bitset<uint8_t>(ss.str());
} 
int Parser::bitsetToVector(const boost::dynamic_bitset<uint8_t> &src, std::vector<uint8_t> &dst){
    dst.clear();
    boost::to_block_range(src, std::back_inserter(dst));
    for (std::vector<uint8_t>::iterator it = dst.begin(); it != dst.end(); it++){
        *it = reverseByte(*it);
    }
}
uint8_t Parser::reverseByte(uint8_t b){
        uint8_t tmp = 0;
        uint8_t mask = 1;
        for (int i=0; i<8; i++){            
            tmp = ((((b)&(mask<<i)) >> i) << (7-i)) | tmp;
        }
        return tmp;
}

int Parser::searchByte(const std::vector<uint8_t> &bytes, uint8_t search_byte, uint8_t byte_mask){
    boost::dynamic_bitset<uint8_t> bits;
    vectorToBitset(bytes, bits);
    return searchByte(bits, search_byte, byte_mask);
}

int Parser::searchByte(const boost::dynamic_bitset<uint8_t> &bits, uint8_t search_byte, uint8_t byte_mask){
    return searchByte(bits, search_byte, byte_mask, 0);
}
int Parser::searchByte(const boost::dynamic_bitset<uint8_t> &bits, uint8_t search_byte, uint8_t byte_mask, size_t start_pos){
    search_byte = reverseByte(search_byte);
    boost::dynamic_bitset<uint8_t> search_bits(byte_mask, search_byte);
    return searchByte(bits, search_bits, start_pos);
}

int Parser::searchByte(const boost::dynamic_bitset<uint8_t> &bits, const boost::dynamic_bitset<uint8_t> &search_bits){
    searchByte(bits, search_bits, 0);
}
int Parser::searchByte(const boost::dynamic_bitset<uint8_t> &bits, const boost::dynamic_bitset<uint8_t> &search_bits, size_t start_pos){
    for (int bit_nr=start_pos; bit_nr < bits.size()-search_bits.size()+1; bit_nr++){
        bool found = true;
        for (int bit=0; bit < search_bits.size(); bit++){
            if (bits[bit_nr+bit] != search_bits[bit]){
                found = false;
            }
        }
        if (found){
            return bit_nr;
        }
    }
    return -1;
}
int Parser::reverseSearchByte(const boost::dynamic_bitset<uint8_t> &bits, const boost::dynamic_bitset<uint8_t> &search_bits, size_t start_pos){
    for (int bit_nr=start_pos; bit_nr >= 0; bit_nr--){
        bool found = true;
        for (int bit=0; bit < search_bits.size(); bit++){
            if (bits[bit_nr+bit] != search_bits[bit]){
                found = false;
            }
        }
        if (found){
            return bit_nr;
        }
    }
    return -1;
}

int Parser::stuffBits(std::vector<uint8_t> &bytes, uint8_t forbidden_byte, uint8_t bit_count){
    boost::dynamic_bitset<uint8_t> bits;
    vectorToBitset(bytes, bits);
    int ret = stuffBits(bits, forbidden_byte, bit_count);
    bitsetToVector(bits, bytes);
    return ret;
}

int Parser::stuffBits(boost::dynamic_bitset<uint8_t> &bits, uint8_t forbidden_byte, uint8_t bit_count){
    forbidden_byte = reverseByte(forbidden_byte);
    boost::dynamic_bitset<uint8_t> forbidden_bits(bit_count, forbidden_byte);
    return stuffBits(bits, forbidden_bits);
}

int Parser::stuffBits(boost::dynamic_bitset<uint8_t> &bits, boost::dynamic_bitset<uint8_t> forbidden_bits){
    bool stuff = !forbidden_bits[forbidden_bits.size()-1];
    forbidden_bits.resize(forbidden_bits.size()-1);
    int found = searchByte(bits, forbidden_bits);
    int stuff_count = 0;
    while (found >= 0){
        stuffBit(bits, (size_t) found+forbidden_bits.size(), stuff);
        stuff_count++;
        found = searchByte(bits, forbidden_bits, found+forbidden_bits.size());
    }
    return stuff_count;
}
int Parser::stuffBit(boost::dynamic_bitset<uint8_t> &bits, size_t pos, bool stuff){
    boost::dynamic_bitset<uint8_t> bits_back(bits);
    boost::dynamic_bitset<uint8_t> mask(bits.size()+1);
    mask.set();
    mask <<= (pos+1); 
    bits.resize(pos);
    bits.push_back(stuff);
    bits.resize(mask.size());
    bits_back.push_back(stuff);
    bits_back <<= 1;
    bits_back &= mask;
    bits |= bits_back;
}
int Parser::deStuffBits(std::vector<uint8_t> &bytes, uint8_t forbidden_byte, uint8_t bit_count){
    boost::dynamic_bitset<uint8_t> bits;
    vectorToBitset(bytes, bits);
    int ret = deStuffBits(bits, forbidden_byte, bit_count);
    bitsetToVector(bits, bytes);
    return ret;
}
int Parser::deStuffBits(boost::dynamic_bitset<uint8_t> &bits, uint8_t forbidden_byte, uint8_t bit_count){
    forbidden_byte = reverseByte(forbidden_byte);
    boost::dynamic_bitset<uint8_t> forbidden_bits(bit_count, forbidden_byte);
    return deStuffBits(bits, forbidden_bits);
}
int Parser::deStuffBits(boost::dynamic_bitset<uint8_t> &bits, boost::dynamic_bitset<uint8_t> forbidden_bits){
    forbidden_bits.resize(forbidden_bits.size()-1);
    int found = reverseSearchByte(bits, forbidden_bits, bits.size()-forbidden_bits.size());
    int de_stuff_count = 0;
    while (found >= 0){
        deStuffBit(bits, (size_t) found+forbidden_bits.size());
        de_stuff_count++;
        found = reverseSearchByte(bits, forbidden_bits, found-1);
    }
    return de_stuff_count;
}
int Parser::deStuffBit(boost::dynamic_bitset<uint8_t> &bits, size_t pos){
    boost::dynamic_bitset<uint8_t> bits_back(bits);
    boost::dynamic_bitset<uint8_t> mask(bits.size()-1);
    mask.set();
    mask <<= pos; 
    bits.resize(pos);
    bits.resize(mask.size());
    bits_back >>= 1;
    bits_back.resize(mask.size());
    bits_back &= mask;
    bits |= bits_back;
}

int Parser::packData(std::vector<uint8_t> &data){
    boost::dynamic_bitset<uint8_t> bits;
    vectorToBitset(data, bits);
    packData(bits);
    bitsetToVector(bits, data);
}
int Parser::packData(boost::dynamic_bitset<uint8_t> &bits){
    if (bits.count()%2 != 0){
        bits.push_back(true);
    } else {
        bits.push_back(false);
    }
    stuffBits(bits, END_FLAG, END_BIT_COUNT);
    stuffBits(bits, SYN_FLAG, SYN_BIT_COUNT);
    //stuffBits(bits, 0xAA, 8);
    //stuffBits(bits, 0x55, 8);
    bits.resize(bits.size()+SYN_BIT_COUNT+END_BIT_COUNT);
    bits <<= SYN_BIT_COUNT;
    uint8_t syn_byte = reverseByte(SYN_FLAG);
    boost::dynamic_bitset<uint8_t> syn_bits(SYN_BIT_COUNT, syn_byte);
    syn_bits.resize(bits.size());
    uint8_t end_byte = reverseByte(END_FLAG);
    boost::dynamic_bitset<uint8_t> end_bits(SYN_BIT_COUNT, end_byte);
    end_bits.resize(bits.size());
    end_bits <<= end_bits.size()-END_BIT_COUNT;
    bits |= syn_bits | end_bits;
}

bool Parser::parseDistance(const uint8_t *buffer, const size_t buffer_size, double &distance)
{
    char buff[buffer_size+1];
    memcpy(buff,buffer,buffer_size);
    buff[buffer_size] = 0;
    if(sscanf(buff,"\r\n%fm\r\n",distance)){
        return true;
    }
    return false;
}
