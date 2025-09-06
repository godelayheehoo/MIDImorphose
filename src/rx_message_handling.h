//currently unused
/*
#pragma once
#include <Arduino.h>
#include "message_defs.h" //don't need this anymore

#define MAX_MSG_LEN 64

// -------------------
// Parsed message struct
// -------------------
struct ParsedMessage {
    MessageType type;
    byte payload[MAX_MSG_LEN];
    byte length; // length of payload
};

// -------------------
// Public functions
// -------------------
bool readSerial2NonBlocking(ParsedMessage &outMsg);

bool isStringMessage(MessageType type);
bool isSingleByteMessage(MessageType type);
bool isMultiByteMessage(MessageType type);
*/