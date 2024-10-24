#include "serialize/serialize.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static char _privateBuffer[PACKET_SIZE];

static result_t assemble(char *outputBuffer, const char *inputBuffer, int len) {
    // byes copied to output buffer so far
    static int counter = 0;

    // for leftover bytes from previous call
    static bool leftoverFlag = false;
    static int bytesLeftover = 0;
    static char leftoverBuffer[PACKET_SIZE];

    int bytesAvailableInOutputBuffer;

    // copy leftover bytes from previous call first
    if (leftoverFlag) {
        int bytesToCopyToOutput;
        if (bytesLeftover <= PACKET_SIZE) {  // leftover bytes cannot form a full packet
            leftoverFlag = false;
            bytesToCopyToOutput = bytesLeftover;
        } else {  // leftover bytes can form a full packet
            bytesToCopyToOutput = PACKET_SIZE;
        }

        bytesLeftover -= bytesToCopyToOutput;  // remove bytes that were copied from leftover buffer

        // copy leftover bytes to output
        for (int i = 0; i < bytesToCopyToOutput; i++) {
            outputBuffer[counter++] = leftoverBuffer[i];
        }
    }

    // fill up leftover buffer
    if (counter + len >= PACKET_SIZE) {  // copied bytes + current stream exceed packet size; there will be bytes leftover
        bytesAvailableInOutputBuffer = PACKET_SIZE - counter;
        leftoverFlag = true;
        int bytesToCopyToLeftover = len - bytesAvailableInOutputBuffer;

        // copy leftover bytes from output to leftover buffer after entries that were already inside
        for (int i = 0; i < bytesToCopyToLeftover; i++) {
            leftoverBuffer[bytesLeftover + i] = inputBuffer[bytesAvailableInOutputBuffer + i];
        }

        bytesLeftover += bytesToCopyToLeftover; // add bytes that were copied to leftover buffer
    } else {    // there will be no bytes leftover after copying input to output
        bytesAvailableInOutputBuffer = len;
    }

    // copy input bytes to output
    for (int i = 0; i < bytesAvailableInOutputBuffer; i++) {
        outputBuffer[counter++] = inputBuffer[i];
    }

    if (counter == PACKET_SIZE) {
        counter = 0;
        return PACKET_COMPLETE;
    }
    return PACKET_INCOMPLETE;
}

result_t deserialize(const char *buffer, int len, void *output) {
    result_t result = assemble(_privateBuffer, buffer, len);

    if (result == PACKET_COMPLETE) {
        memcpy(output, _privateBuffer, PACKET_SIZE);
        return PACKET_OK;
    }
    return result;
}

int serialize(char *buffer, void *dataStructure, size_t size) {
    memcpy(buffer, dataStructure, size);
    return size;
}
