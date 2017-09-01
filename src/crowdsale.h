#ifndef CROWDSALE_H
#define CROWDSALE_H
#include "amount.h"
#include "uint256.h"

const int max_vout=5000;
const int crowdsale_size=40000;
extern const uint256 crowdsaleTxHashes[];
constexpr int CROWDSALE_BLOCK_COUNT=std::ceil(crowdsale_size/max_vout);
extern const struct Crowdsale{
   char address[41];
   CAmount amount;
} crowdsale[crowdsale_size];

#endif // CROWDSALE_H
