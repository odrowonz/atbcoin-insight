#ifndef CROWDSALE_H
#define CROWDSALE_H
#include "amount.h"
#include "uint256.h"
#include <cmath>

const float max_vout=7880;
const int crowdsale_size=7890;
extern const uint256 crowdsaleTxHashes[];
const int CROWDSALE_BLOCK_COUNT=std::ceil(crowdsale_size/max_vout);
extern const struct Crowdsale{
   char address[41];
   CAmount amount;
} crowdsale[crowdsale_size];

#endif // CROWDSALE_H
