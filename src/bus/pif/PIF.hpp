#pragma once

#include <cstdint>
#include "PIFChannel.hpp"

class Bus;

const uint32_t CART_CHANNEL = 4;

// boot rom code gotten from https://github.com/gopher64/gopher64
std::array<uint8_t, 1984> PIF_BOOT_ROM = {
    60, 9, 52, 0, 64, 137, 96, 0, 60, 9, 0, 6, 53, 41, 228, 99, 64, 137, 128, 0, 60, 8, 164, 4,
    141, 8, 0, 16, 49, 8, 0, 1, 81, 0, 255, 253, 60, 8, 164, 4, 36, 8, 0, 10, 60, 1, 164, 4, 172,
    40, 0, 16, 60, 8, 164, 4, 141, 8, 0, 24, 49, 8, 0, 1, 85, 0, 255, 253, 60, 8, 164, 4, 36, 8, 0,
    3, 60, 1, 164, 96, 172, 40, 0, 16, 36, 8, 3, 255, 60, 1, 164, 64, 172, 40, 0, 12, 60, 1, 164,
    64, 172, 32, 0, 36, 60, 1, 164, 64, 172, 32, 0, 16, 60, 1, 164, 80, 172, 32, 0, 0, 60, 1, 164,
    80, 172, 32, 0, 4, 60, 8, 164, 4, 141, 8, 0, 16, 49, 8, 0, 4, 85, 0, 255, 253, 60, 8, 164, 4,
    60, 11, 164, 0, 60, 12, 191, 192, 60, 13, 191, 192, 37, 107, 16, 0, 37, 140, 0, 212, 37, 173,
    7, 28, 141, 137, 0, 0, 37, 140, 0, 4, 37, 107, 0, 4, 21, 141, 255, 252, 173, 105, 255, 252, 60,
    11, 164, 0, 60, 29, 164, 0, 37, 107, 16, 0, 1, 96, 0, 8, 55, 189, 31, 240, 60, 13, 191, 192,
    141, 168, 7, 252, 37, 173, 7, 192, 49, 8, 0, 128, 85, 0, 255, 252, 60, 13, 191, 192, 141, 168,
    0, 36, 60, 11, 176, 0, 0, 8, 156, 194, 50, 115, 0, 1, 0, 8, 188, 130, 18, 96, 0, 2, 50, 247, 0,
    1, 60, 11, 166, 0, 0, 8, 178, 2, 49, 10, 0, 255, 0, 8, 172, 66, 141, 168, 0, 60, 36, 9, 0, 16,
    50, 214, 0, 255, 50, 181, 0, 1, 36, 20, 0, 1, 1, 9, 64, 37, 60, 9, 164, 128, 141, 41, 0, 24,
    49, 41, 0, 2, 85, 32, 255, 253, 60, 9, 164, 128, 173, 168, 0, 60, 60, 12, 164, 96, 36, 8, 0,
    255, 173, 136, 0, 20, 173, 136, 0, 24, 36, 8, 0, 15, 173, 136, 0, 28, 36, 8, 0, 3, 173, 136, 0,
    32, 141, 105, 0, 0, 60, 13, 164, 16, 37, 140, 0, 0, 49, 40, 0, 255, 173, 136, 0, 20, 0, 9, 66,
    2, 173, 136, 0, 24, 0, 9, 68, 2, 173, 136, 0, 28, 0, 9, 69, 2, 173, 136, 0, 32, 141, 175, 0,
    12, 32, 8, 15, 192, 33, 107, 0, 64, 49, 239, 0, 1, 17, 224, 0, 7, 37, 173, 0, 12, 60, 13, 164,
    16, 141, 175, 0, 12, 37, 173, 0, 12, 49, 239, 0, 32, 85, 224, 255, 252, 60, 13, 164, 16, 60,
    13, 164, 0, 37, 173, 0, 0, 0, 8, 48, 37, 33, 173, 0, 64, 141, 105, 0, 0, 33, 8, 255, 252, 33,
    107, 0, 4, 33, 173, 0, 4, 21, 0, 255, 251, 173, 169, 255, 252, 60, 8, 108, 7, 53, 8, 137, 101,
    1, 72, 0, 25, 0, 0, 32, 18, 36, 132, 0, 1, 60, 5, 164, 0, 36, 165, 0, 64, 4, 17, 0, 19, 0, 0,
    0, 0, 39, 189, 255, 208, 20, 160, 0, 2, 175, 191, 0, 28, 0, 192, 40, 37, 39, 166, 0, 44, 4, 17,
    0, 255, 39, 167, 0, 40, 143, 164, 0, 40, 143, 174, 0, 44, 1, 196, 16, 35, 20, 64, 0, 2, 0, 64,
    24, 37, 0, 128, 24, 37, 143, 191, 0, 28, 39, 189, 0, 48, 0, 96, 16, 37, 3, 224, 0, 8, 0, 0, 0,
    0, 39, 189, 255, 32, 175, 191, 0, 60, 175, 183, 0, 52, 175, 182, 0, 48, 175, 181, 0, 44, 175,
    180, 0, 40, 175, 179, 0, 36, 175, 178, 0, 32, 175, 177, 0, 28, 175, 176, 0, 24, 140, 174, 0, 0,
    0, 0, 24, 37, 39, 163, 0, 180, 39, 162, 0, 116, 1, 196, 128, 38, 36, 66, 0, 16, 172, 80, 255,
    244, 172, 80, 255, 248, 172, 80, 255, 252, 20, 67, 255, 251, 172, 80, 255, 240, 140, 176, 0, 0,
    0, 0, 136, 37, 0, 160, 176, 37, 36, 23, 0, 32, 2, 0, 160, 37, 142, 208, 0, 0, 38, 49, 0, 1, 36,
    15, 3, 239, 142, 211, 0, 4, 38, 214, 0, 4, 1, 241, 32, 35, 2, 32, 48, 37, 4, 17, 255, 204, 2,
    0, 40, 37, 143, 163, 0, 116, 143, 164, 0, 120, 2, 0, 40, 37, 0, 67, 24, 33, 175, 163, 0, 116,
    4, 17, 255, 197, 2, 32, 48, 37, 143, 184, 0, 124, 60, 5, 108, 7, 175, 162, 0, 120, 3, 16, 200,
    38, 175, 185, 0, 124, 52, 165, 137, 101, 38, 4, 0, 5, 4, 17, 255, 188, 2, 32, 48, 37, 143, 168,
    0, 128, 2, 144, 8, 43, 0, 72, 72, 33, 16, 32, 0, 7, 175, 169, 0, 128, 143, 164, 0, 152, 2, 0,
    40, 37, 4, 17, 255, 179, 2, 32, 48, 37, 16, 0, 0, 4, 175, 162, 0, 152, 143, 170, 0, 152, 1, 80,
    88, 33, 175, 171, 0, 152, 50, 130, 0, 31, 143, 174, 0, 132, 2, 226, 24, 35, 0, 112, 104, 4, 0,
    80, 96, 6, 1, 141, 168, 37, 0, 112, 200, 6, 0, 80, 192, 4, 1, 213, 120, 33, 175, 175, 0, 132,
    3, 25, 40, 37, 143, 164, 0, 144, 4, 17, 255, 160, 2, 32, 48, 37, 143, 163, 0, 140, 175, 162, 0,
    144, 2, 3, 8, 43, 80, 32, 0, 8, 143, 171, 0, 132, 143, 168, 0, 128, 2, 17, 80, 33, 1, 3, 72,
    33, 1, 42, 24, 38, 16, 0, 0, 5, 175, 163, 0, 140, 143, 171, 0, 132, 1, 112, 96, 33, 1, 131, 24,
    38, 175, 163, 0, 140, 0, 20, 22, 194, 143, 175, 0, 136, 2, 226, 24, 35, 0, 112, 112, 6, 0, 80,
    104, 4, 1, 174, 144, 37, 0, 112, 64, 4, 0, 80, 200, 6, 1, 242, 192, 33, 175, 184, 0, 136, 3,
    40, 40, 37, 143, 164, 0, 148, 4, 17, 255, 131, 2, 32, 48, 37, 36, 1, 3, 240, 18, 33, 0, 57,
    175, 162, 0, 148, 143, 164, 0, 176, 2, 64, 40, 37, 4, 17, 255, 124, 2, 32, 48, 37, 0, 16, 30,
    194, 2, 227, 80, 35, 1, 83, 88, 6, 0, 115, 72, 4, 1, 43, 40, 37, 0, 64, 32, 37, 4, 17, 255,
    116, 2, 32, 48, 37, 175, 162, 0, 176, 143, 164, 0, 172, 2, 160, 40, 37, 4, 17, 255, 111, 2, 32,
    48, 37, 50, 18, 0, 31, 2, 242, 160, 35, 2, 147, 104, 4, 2, 83, 96, 6, 1, 141, 40, 37, 0, 64,
    32, 37, 4, 17, 255, 103, 2, 32, 48, 37, 143, 169, 0, 168, 50, 99, 0, 31, 2, 80, 112, 6, 2, 144,
    120, 4, 2, 227, 200, 35, 1, 207, 56, 37, 3, 51, 64, 4, 0, 115, 192, 6, 143, 173, 0, 156, 3, 8,
    80, 37, 1, 39, 88, 33, 1, 106, 96, 33, 175, 162, 0, 172, 175, 172, 0, 168, 2, 96, 40, 37, 2,
    32, 48, 37, 4, 17, 255, 85, 1, 176, 32, 33, 143, 174, 0, 160, 175, 162, 0, 156, 2, 96, 40, 37,
    2, 32, 48, 37, 4, 17, 255, 79, 1, 208, 32, 38, 143, 175, 0, 148, 143, 184, 0, 164, 175, 162, 0,
    160, 1, 240, 200, 38, 3, 56, 64, 33, 16, 0, 255, 115, 175, 168, 0, 164, 143, 163, 0, 116, 0, 0,
    136, 37, 39, 179, 0, 116, 36, 21, 0, 16, 36, 20, 0, 1, 175, 163, 0, 100, 175, 163, 0, 104, 175,
    163, 0, 108, 175, 163, 0, 112, 142, 112, 0, 0, 143, 173, 0, 100, 50, 2, 0, 31, 2, 226, 88, 35,
    1, 112, 80, 4, 0, 80, 72, 6, 1, 42, 96, 37, 1, 172, 112, 33, 2, 14, 8, 43, 16, 32, 0, 5, 175,
    174, 0, 100, 143, 175, 0, 104, 1, 240, 200, 33, 16, 0, 0, 6, 175, 185, 0, 104, 143, 164, 0,
    104, 2, 0, 40, 37, 4, 17, 255, 44, 2, 32, 48, 37, 175, 162, 0, 104, 50, 24, 0, 2, 0, 24, 64,
    66, 50, 18, 0, 1, 85, 18, 0, 6, 143, 164, 0, 108, 143, 171, 0, 108, 1, 112, 72, 33, 16, 0, 0,
    6, 175, 169, 0, 108, 143, 164, 0, 108, 2, 0, 40, 37, 4, 17, 255, 30, 2, 32, 48, 37, 175, 162,
    0, 108, 86, 146, 0, 6, 143, 164, 0, 112, 143, 170, 0, 112, 1, 80, 104, 38, 16, 0, 0, 6, 175,
    173, 0, 112, 143, 164, 0, 112, 2, 0, 40, 37, 4, 17, 255, 19, 2, 32, 48, 37, 175, 162, 0, 112,
    38, 49, 0, 1, 22, 53, 255, 209, 38, 115, 0, 4, 143, 164, 0, 100, 143, 165, 0, 104, 4, 17, 255,
    11, 2, 32, 48, 37, 143, 172, 0, 112, 143, 174, 0, 108, 143, 176, 0, 24, 143, 177, 0, 28, 143,
    178, 0, 32, 143, 179, 0, 36, 143, 180, 0, 40, 143, 181, 0, 44, 143, 182, 0, 48, 143, 183, 0,
    52, 143, 191, 0, 60, 0, 64, 32, 37, 39, 189, 0, 224, 4, 17, 0, 8, 1, 142, 40, 38, 0, 133, 0,
    25, 0, 0, 112, 16, 172, 206, 0, 0, 0, 0, 120, 18, 172, 239, 0, 0, 3, 224, 0, 8, 0, 0, 0, 0, 60,
    11, 191, 192, 141, 104, 7, 240, 60, 10, 255, 255, 48, 132, 255, 255, 1, 10, 64, 36, 0, 136, 32,
    37, 37, 107, 7, 192, 60, 9, 164, 128, 141, 41, 0, 24, 49, 41, 0, 2, 85, 32, 255, 253, 60, 9,
    164, 128, 173, 100, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 9,
    164, 128, 141, 41, 0, 24, 49, 41, 0, 2, 85, 32, 255, 253, 60, 9, 164, 128, 141, 104, 0, 60, 36,
    9, 0, 32, 173, 101, 0, 52, 1, 9, 64, 37, 60, 9, 164, 128, 141, 41, 0, 24, 49, 41, 0, 2, 85, 32,
    255, 253, 60, 9, 164, 128, 173, 104, 0, 60, 32, 9, 0, 16, 33, 41, 255, 255, 85, 32, 255, 255,
    33, 41, 255, 255, 141, 104, 0, 60, 49, 10, 0, 128, 80, 10, 255, 250, 32, 9, 0, 16, 36, 10, 0,
    64, 1, 10, 64, 37, 60, 9, 164, 128, 141, 41, 0, 24, 49, 41, 0, 2, 85, 32, 255, 253, 60, 9, 164,
    128, 173, 104, 0, 60, 60, 11, 164, 0, 37, 107, 0, 0, 33, 107, 0, 64, 1, 96, 0, 8, 0, 0, 0, 0,
    0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

class PIF {
public:

    std::array<uint8_t, 64> ram = {};
    std::array<PIFChannel, 5> channels;

    std::array<uint8_t, 1984> rom = PIF_BOOT_ROM;

    Bus& bus;

    uint8_t getCrc(int channel, uint16_t address, int data);

    void executeCommand();
    void setupChannels();

    int setupPIFChannel(int channel, int buf);
    void disablePIFChannel(int channel);

    void processController(int channelId);
    void processCartridge();

    void readEeprom();
    void writeEeprom();

    void writePakBlock(int addrAcrc, int data, int dcrc, int channel);
    void readPakBlock(int addrAcrc, int data, int dcrc, int channel);


    PIF(Bus& bus): bus(bus) {
        ram = {};
    };
};