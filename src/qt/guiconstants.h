// Copyright (c) 2011-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_GUICONSTANTS_H
#define BITCOIN_QT_GUICONSTANTS_H
#include "../amount.h"
/* Milliseconds between model updates */
static const int MODEL_UPDATE_DELAY = 250;

/* AskPassphraseDialog -- Maximum passphrase length */
static const int MAX_PASSPHRASE_SIZE = 1024;

/* BitcoinGUI -- Size of icons in status bar */
static const int STATUSBAR_ICONSIZE = 16;

static const bool DEFAULT_SPLASHSCREEN = true;

/* Invalid field background style */
#define STYLE_INVALID "background:#e87764"

#define STATIC_COIN_MODE 0      //Mode in which all in one currency will be displayed with accuracy STATIC_DECEMALS.
#define DYNAMIC_COIN_MODE 1     //Mode in which several display options will be offered.
#define CUSTOM_COIN_MODE 2      //Mode in which the accuracy is determined by STATIC_DECEMALS, and the factor CUSTOM_FACTOR.


#define CUSTOM_FACTOR COIN      //Value of a unit of currency
#define STATIC_DECEMALS 8       //Precision after the decimal point point in static and custom mode

#define COIN_MODE STATIC_DECEMALS //Currency display mode.

/* Transaction list -- unconfirmed transaction */
#define COLOR_UNCONFIRMED QColor(128, 128, 128)
/* Transaction list -- negative amount */
#define COLOR_NEGATIVE QColor(0xe8,0x77,0x64)
#define COLOR_POSITIVE QColor(0x4b,0xa2,0x7e)

/* Transaction list -- bare address (without label) */
#define COLOR_BAREADDRESS QColor(140, 140, 140)
/* Transaction list -- TX status decoration - open until date */
#define COLOR_TX_STATUS_OPENUNTILDATE QColor(64, 64, 255)
/* Transaction list -- TX status decoration - offline */
#define COLOR_TX_STATUS_OFFLINE QColor(192, 192, 192)
/* Transaction list -- TX status decoration - danger, tx needs attention */
#define COLOR_TX_STATUS_DANGER QColor(200, 100, 100)
/* Transaction list -- TX status decoration - default color */
#define COLOR_BLACK QColor(0, 0, 0)

/* Tooltips longer than this (in characters) are converted into rich text,
   so that they can be word-wrapped.
 */
static const int TOOLTIP_WRAP_THRESHOLD = 80;

/* Maximum allowed URI length */
static const int MAX_URI_LENGTH = 255;

/* QRCodeDialog -- size of exported QR Code image */
#define QR_IMAGE_SIZE 300

/* Number of frames in spinner animation */
#define SPINNER_FRAMES 36

#define QAPP_ORG_NAME "Bitcoin"
#define QAPP_ORG_DOMAIN "bitcoin.org"
#define QAPP_APP_NAME_DEFAULT "Bitcoin-Qt"
#define QAPP_APP_NAME_TESTNET "Bitcoin-Qt-testnet"

#endif // BITCOIN_QT_GUICONSTANTS_H
