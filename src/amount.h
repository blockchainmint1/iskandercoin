// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_AMOUNT_H
#define BITCOIN_AMOUNT_H

#include <stdint.h>

/** Amount in satoshis (Can be negative) */
typedef int64_t CAmount;

static const CAmount COIN = 100000000;

/** No amount larger than this (in satoshi) is valid.
 *
 * Note that this constant is *not* the total money supply, which in Iskander
 * is designed to be 434,093,088 ISK based on 312 ISK block reward
 * halving every 695,662 blocks over approximately 139 years.
 * */
static const CAmount MAX_MONEY = 434093088 * COIN;
inline bool MoneyRange(const CAmount& nValue) { return (nValue >= 0 && nValue <= MAX_MONEY); }

#endif //  BITCOIN_AMOUNT_H
