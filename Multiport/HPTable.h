#ifndef __HPTABLE_H_INCLUDED__
#define __HPTABLE_H_INCLUDED__
#include <cstdint>

struct HPDest {
    uint16_t zone;
    float x, y, z;
    uint8_t rot;
};

// Homepoint destination table built from homepoint.lua
// Format: { zone, x, y, z, rot }
// zone encoding in 0x00A is not yet confirmed -- TODO
static const HPDest g_HPTable[] = {
    /* [  0] */ { 230,   -85.554f,    1.0f,   -64.554f,  45 }, // Southern San d'Oria #1
    /* [  1] */ { 230,     44.1f,    2.0f,    -34.5f,   170 }, // Southern San d'Oria #2
    /* [  2] */ { 230,    140.5f,   -2.0f,    121.0f,     0 }, // Southern San d'Oria #3
    /* [  3] */ { 231,   -178.0f,    4.0f,     71.0f,     0 }, // Northern San d'Oria #1
    /* [  4] */ { 231,     10.0f,   -0.2f,     95.0f,     0 }, // Northern San d'Oria #2
    /* [  5] */ { 231,     70.0f,   -0.2f,     10.0f,     0 }, // Northern San d'Oria #3
    /* [  6] */ { 232,    -38.0f,   -4.0f,    -63.0f,     0 }, // Port San d'Oria #1
    /* [  7] */ { 232,     48.0f,  -12.0f,   -105.0f,     0 }, // Port San d'Oria #2
    /* [  8] */ { 232,     -6.0f,  -13.0f,   -150.0f,     0 }, // Port San d'Oria #3
    /* [  9] */ { 234,     39.0f,    0.0f,    -43.0f,     0 }, // Bastok Mines #1
    /* [ 10] */ { 234,    118.0f,    1.0f,    -58.0f,     0 }, // Bastok Mines #2
    /* [ 11] */ { 235,   -342.0f,  -10.0f,   -154.0f,     0 }, // Bastok Markets #1
    /* [ 12] */ { 235,   -328.0f,  -12.0f,    -33.0f,     0 }, // Bastok Markets #2
    /* [ 13] */ { 235,   -189.0f,   -8.0f,     26.0f,     0 }, // Bastok Markets #3
    /* [ 14] */ { 236,    124.0f,    8.0f,      7.0f,     0 }, // Port Bastok #1
    /* [ 15] */ { 236,     42.0f,    8.5f,   -244.0f,     0 }, // Port Bastok #2
    /* [ 16] */ { 237,     46.0f,  -14.0f,    -19.0f,     0 }, // Metalworks #1
    /* [ 17] */ { 238,    -32.0f,   -5.0f,    132.0f,     0 }, // Windurst Waters #1
    /* [ 18] */ { 238,    138.5f,    0.0f,    -14.0f,     0 }, // Windurst Waters #2
    /* [ 19] */ { 239,    -72.0f,   -5.0f,    125.0f,     0 }, // Windurst Walls #1
    /* [ 20] */ { 239,   -212.0f,    0.0f,    -99.0f,     0 }, // Windurst Walls #2
    /* [ 21] */ { 239,     31.0f,   -6.5f,    -40.0f,     0 }, // Windurst Walls #3
    /* [ 22] */ { 240,   -188.0f,   -4.0f,    101.0f,     0 }, // Port Windurst #1
    /* [ 23] */ { 240,   -207.0f,   -8.0f,    210.0f,     0 }, // Port Windurst #2
    /* [ 24] */ { 240,    180.0f,  -12.0f,    226.0f,     0 }, // Port Windurst #3
    /* [ 25] */ { 241,      9.0f,   -2.0f,      0.0f,     0 }, // Windurst Woods #1
    /* [ 26] */ { 241,    107.0f,   -5.0f,    -56.0f,     0 }, // Windurst Woods #2
    /* [ 27] */ { 241,    -92.0f,   -5.0f,     62.0f,     0 }, // Windurst Woods #3
    /* [ 28] */ { 241,     74.0f,   -7.0f,   -139.0f,     0 }, // Windurst Woods #4
    /* [ 29] */ { 243,     -6.0f,    3.0f,      0.0f,     0 }, // Ru'Lude Gardens #1
    /* [ 30] */ { 243,     53.0f,    9.0f,    -57.0f,     0 }, // Ru'Lude Gardens #2
    /* [ 31] */ { 243,    -67.0f,    6.0f,    -25.0f,     1 }, // Ru'Lude Gardens #3
    /* [ 32] */ { 244,    -99.0f,    0.0f,    167.0f,     0 }, // Upper Jeuno #1
    /* [ 33] */ { 244,     32.0f,   -1.0f,    -44.0f,     0 }, // Upper Jeuno #2
    /* [ 34] */ { 244,    -52.0f,    1.0f,     16.0f,     0 }, // Upper Jeuno #3
    /* [ 35] */ { 245,    -99.0f,    0.0f,   -183.0f,     0 }, // Lower Jeuno #1
    /* [ 36] */ { 245,     18.0f,   -1.0f,     54.0f,     0 }, // Lower Jeuno #2
    /* [ 37] */ { 246,     37.0f,    0.0f,      9.0f,     0 }, // Port Jeuno #1
    /* [ 38] */ { 246,   -155.0f,   -1.0f,     -4.0f,     0 }, // Port Jeuno #2
    /* [ 39] */ { 250,     78.0f,  -13.0f,    -94.0f,     0 }, // Kazham #1
    /* [ 40] */ { 249,    -13.0f,  -15.0f,     87.0f,     0 }, // Mhaura #1
    /* [ 41] */ { 252,    -27.0f,    0.0f,    -47.0f,     0 }, // Norg #1
    /* [ 42] */ { 247,    -29.0f,    0.0f,    -76.0f,     0 }, // Rabao #1
    /* [ 43] */ { 248,     36.0f,  -11.0f,     35.0f,     0 }, // Selbina #1
    /* [ 44] */ { 256,    -84.0f,    4.0f,    -32.0f,   128 }, // Western Adoulin #1
    /* [ 45] */ { 257,    -51.0f,    0.0f,     59.0f,   128 }, // Eastern Adoulin #1
    /* [ 46] */ { 261,   -107.0f,    3.0f,    295.0f,   128 }, // Ceizak Battlegrounds #1
    /* [ 47] */ { 262,   -193.0f,   -0.5f,   -252.0f,   128 }, // Foret de Hennetiel #1
    /* [ 48] */ { 265,   -415.0f,  -63.2f,    409.0f,   106 }, // Morimar Basalt Fields #1
    /* [ 49] */ { 263,   -420.0f,    0.0f,    -62.0f,    64 }, // Yorcia Weald #1
    /* [ 50] */ { 266,    -23.0f,    0.0f,    174.0f,     0 }, // Marjami Ravine #1
    /* [ 51] */ { 267,    210.0f,   20.299f,  315.0f,   192 }, // Kamihr Drifts #1
    /* [ 52] */ { 142,    434.0f,  -40.0f,    171.0f,     0 }, // Yughott Grotto #1
    /* [ 53] */ { 143,    109.0f,  -38.0f,   -147.0f,     0 }, // Palborough Mines #1
    /* [ 54] */ { 145,   -132.0f,   -3.0f,   -303.0f,     0 }, // Giddeus #1
    /* [ 55] */ { 204,    243.0f,  -24.0f,     62.0f,     0 }, // Fei'Yin #1
    /* [ 56] */ { 208,   -984.0f,   17.0f,   -289.0f,     0 }, // Quicksand Caves #1
    /* [ 57] */ { 160,    -80.0f,   46.0f,     62.0f,     0 }, // Den of Rancor #1
    /* [ 58] */ { 162,   -554.0f,  -70.0f,     66.0f,     0 }, // Castle Zvahl Keep #1
    /* [ 59] */ { 130,      5.0f,  -42.0f,    526.0f,     0 }, // Ru'Aun Gardens #1
    /* [ 60] */ { 130,   -499.0f,  -42.0f,    167.0f,     0 }, // Ru'Aun Gardens #2
    /* [ 61] */ { 130,   -312.0f,  -42.0f,   -422.0f,     0 }, // Ru'Aun Gardens #3
    /* [ 62] */ { 130,    500.0f,  -42.0f,    158.0f,     0 }, // Ru'Aun Gardens #4
    /* [ 63] */ { 130,    305.0f,  -42.0f,   -427.0f,     0 }, // Ru'Aun Gardens #5
    /* [ 64] */ {  26,     -1.0f,  -28.0f,    107.0f,     0 }, // Tavnazian Safehold #1
    /* [ 65] */ {  50,    -21.0f,    0.0f,    -21.0f,     0 }, // Aht Urhgan Whitegate #1
    /* [ 66] */ {  53,    -20.0f,    0.0f,    -25.0f,     0 }, // Nashmau #1
    /* [ 67] */ {  50,      0.0f,    0.0f,      0.0f,     0 }, // Al Zahbi #1 (coords unknown)
    /* [ 68] */ {  80,    -85.0f,    1.0f,    -66.0f,     0 }, // Southern San d'Oria [S] #1
    /* [ 69] */ {  87,   -293.0f,  -10.0f,   -102.0f,     0 }, // Bastok Markets [S] #1
    /* [ 70] */ {  94,    -32.0f,   -5.0f,    131.0f,     0 }, // Windurst Waters [S] #1
    /* [ 71] */ { 158,   -365.0f, -176.5f,    -36.0f,     0 }, // Upper Delkfutt's Tower #1
    /* [ 72] */ { 178,    -13.0f,   48.0f,     61.0f,     0 }, // The Shrine of Ru'Avitau #1
    /* [ 73] */ {  29,   -520.0f,  -18.0f,    505.0f,   127 }, // Riverne - Site #B01 #1
    /* [ 74] */ {  52,    -98.0f,  -10.0f,   -493.0f,   192 }, // Bhaflau Thickets #1
    /* [ 75] */ {  79,   -449.0f,   13.0f,   -497.0f,     0 }, // Caedarva Mire #1
    /* [ 76] */ {   5,     64.0f, -196.0f,    181.0f,     0 }, // Uleguerand Range #1
    /* [ 77] */ {   5,    380.0f,   23.0f,    -62.0f,     0 }, // Uleguerand Range #2
    /* [ 78] */ {   5,    424.0f,  -32.0f,    221.0f,     0 }, // Uleguerand Range #3
    /* [ 79] */ {   5,     64.0f,  -96.0f,    461.0f,     0 }, // Uleguerand Range #4
    /* [ 80] */ {   5,   -220.0f,   -1.0f,    -62.0f,     0 }, // Uleguerand Range #5
    /* [ 81] */ {   7,   -200.0f,  -10.0f,    342.0f,     0 }, // Attohwa Chasm #1
    /* [ 82] */ {   9,    -58.0f,   40.0f,     14.0f,    64 }, // Pso'Xja #1
    /* [ 83] */ {  12,    445.0f,   27.0f,    -22.0f,     0 }, // Newton Movalpolos #1
    /* [ 84] */ {  30,    189.0f,    0.0f,    362.0f,     0 }, // Riverne - Site #A01 #1
    /* [ 85] */ {  33,      7.0f,    0.0f,    709.0f,   192 }, // Al'Taieu #1
    /* [ 86] */ {  33,   -532.0f,    0.0f,    447.0f,   128 }, // Al'Taieu #2
    /* [ 87] */ {  33,    569.0f,    0.0f,    410.0f,   192 }, // Al'Taieu #3
    /* [ 88] */ {  34,    -12.0f,    0.0f,   -288.0f,   192 }, // Grand Palace of Hu'Xzoi #1
    /* [ 89] */ {  35,   -426.0f,    0.0f,    368.0f,   224 }, // The Garden of Ru'Hmet #1
    /* [ 90] */ {  61,  -540.844f,  -4.0f,   70.809f,    74 }, // Mount Zhayolm #1
    /* [ 91] */ { 113,   -303.0f,   -8.0f,    526.0f,     0 }, // Cape Teriggan #1
    /* [ 92] */ { 153,     88.0f,  -15.0f,   -217.0f,     0 }, // The Boyahda Tree #1
    /* [ 93] */ { 160,    182.0f,   34.0f,    -62.0f,   223 }, // Den of Rancor #2
    /* [ 94] */ { 204,    102.0f,    0.0f,    269.0f,   191 }, // Fei'Yin #2
    /* [ 95] */ { 205,    -63.0f,   50.0f,     81.0f,   192 }, // Ifrit's Cauldron #1
    /* [ 96] */ { 208,    573.0f,    9.0f,   -500.0f,     0 }, // Quicksand Caves #2
    /* [ 97] */ { 230,   -165.0f,   -1.0f,     12.0f,    65 }, // Southern San d'Oria #4
    /* [ 98] */ { 231,   -132.0f,   12.0f,    194.0f,   170 }, // Northern San d'Oria #4
    /* [ 99] */ { 234,     87.0f,    7.0f,      1.0f,     0 }, // Bastok Mines #3
    /* [100] */ { 235,   -192.0f,   -6.0f,    -69.0f,     0 }, // Bastok Markets #4
    /* [101] */ { 236,   -127.0f,   -6.0f,      8.0f,   206 }, // Port Bastok #3
    /* [102] */ { 237,    -76.0f,    2.0f,      3.0f,   124 }, // Metalworks #2
    /* [103] */ { 238,      5.0f,   -4.0f,   -175.0f,   130 }, // Windurst Waters #3
    /* [104] */ { 252,    -65.0f,   -5.0f,     54.0f,   127 }, // Norg #2
    /* [105] */ { 247,    -21.0f,    8.0f,    110.0f,    64 }, // Rabao #2
    /* [106] */ {  50,    130.0f,    0.0f,    -16.0f,     0 }, // Aht Urhgan Whitegate #2
    /* [107] */ {  50,   -108.0f,   -6.0f,    108.0f,   192 }, // Aht Urhgan Whitegate #3
    /* [108] */ {  50,    -99.0f,    0.0f,    -68.0f,     0 }, // Aht Urhgan Whitegate #4
    /* [109] */ { 256,     32.0f,    0.0f,   -164.0f,    32 }, // Western Adoulin #2
    /* [110] */ { 257,    -51.0f,    0.0f,    -96.0f,    96 }, // Eastern Adoulin #2
    /* [111] */ { 137,    223.0f,  -13.0f,   -254.0f,     0 }, // Xarcabard [S] #1
    /* [112] */ { 281,   5.539f,  -0.434f,   8.133f,     73 }, // Leafallia #1
    /* [113] */ { 280,     0.0f,    0.0f,      0.0f,     0 }, // Mog Garden #1 (coords unknown)
    /* [114] */ { 284,     0.0f,    0.0f,      0.0f,     0 }, // Celennia Memorial Library #1 (coords unknown)
    /* [115] */ { 285,     0.0f,    0.0f,      0.0f,     0 }, // Feretory #1 (coords unknown)
    /* [116] */ { 238,     0.0f,    0.0f,      0.0f,     0 }, // (coords unknown)
    /* [117] */ { 239,     0.0f,    0.0f,      0.0f,     0 }, // (coords unknown)
    /* [118] */ { 240,     0.0f,    0.0f,      0.0f,     0 }, // (coords unknown)
    /* [119] */ { 241,   -43.5f,    0.0f,   -145.0f,     0 }, // Windurst Woods #5
    /* [120] */ {  26,    14.0f,   -9.96f,    -5.0f,     0 }, // Tavnazian Safehold #2
    /* [121] */ {  26,   73.59f, -36.149f,   38.87f,     0 }, // Tavnazian Safehold #3
};

static const uint32_t g_HPTableSize = sizeof(g_HPTable) / sizeof(g_HPTable[0]);

#endif
