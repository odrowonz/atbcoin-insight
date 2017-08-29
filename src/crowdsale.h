#ifndef CROWDSALE_H
#define CROWDSALE_H
#include "amount.h"
const int max_vout=5000;
const int crowdsale_size=40000;
extern const char crowdsale_address[][35];
extern const CAmount crowdsale_amount[];
extern const std::string blockHashMerkleRoot[];
constexpr int CROWDSALE_BLOCK_COUNT=std::ceil(crowdsale_size/max_vout);
#endif // CROWDSALE_H
