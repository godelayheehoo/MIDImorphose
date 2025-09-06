//currently unused

/*
#include "rx_message_handling.h" //don't need this anymore

// -------------------
// Static state for non-blocking parser
// -------------------
static MessageType currentMsgType = MessageType::NONE;
static bool receivingMessage = false;
static byte expectedLen = 0;
static byte bytesRead = 0;
static char payloadBuffer[MAX_MSG_LEN];

// -------------------
// Helpers for message types
// -------------------
bool isStringMessage(MessageType type) {
    switch (type) {
        case MessageType::LOG_MSG:
            return true;
        default:
            return false;
    }
}

bool isSingleByteMessage(MessageType type) {
    switch (type) {
        case MessageType::OCTAVE_SHIFT:
            return true;
        default:
            return false;
    }
}

bool isMultiByteMessage(MessageType type) {
    switch (type) {
        // Add your multi-byte types here
        default:
            return false;
    }
}

// -------------------
// Non-blocking parser
// -------------------
bool readSerial2NonBlocking(ParsedMessage &outMsg) {
    while (Serial2.available()) {
        byte b = Serial2.read();

        if (!receivingMessage) {
            currentMsgType = static_cast<MessageType>(b);
            receivingMessage = true;

            if (isSingleByteMessage(currentMsgType)) {
                if (Serial2.available()) {
                    outMsg.type = currentMsgType;
                    outMsg.length = 1;
                    outMsg.payload[0] = Serial2.read();
                    receivingMessage = false;
                    return true;
                }
            } else if (isStringMessage(currentMsgType) || isMultiByteMessage(currentMsgType)) {
                expectedLen = 0;
                bytesRead = 0;
            } else {
                // Unknown type, reset
                receivingMessage = false;
            }

        } else if (isStringMessage(currentMsgType)) {
            if (expectedLen == 0) {
                expectedLen = b;
                if (expectedLen > MAX_MSG_LEN) expectedLen = MAX_MSG_LEN;
            } else {
                payloadBuffer[bytesRead++] = b;
                if (bytesRead >= expectedLen) {
                    payloadBuffer[bytesRead] = '\0';
                    outMsg.type = currentMsgType;
                    outMsg.length = bytesRead;
                    memcpy(outMsg.payload, payloadBuffer, bytesRead);
                    receivingMessage = false;
                    return true;
                }
            }

        } else if (isMultiByteMessage(currentMsgType)) {
            if (expectedLen == 0) {
                expectedLen = b; // first byte = length
                if (expectedLen > MAX_MSG_LEN) expectedLen = MAX_MSG_LEN;
                bytesRead = 0;
            } else {
                payloadBuffer[bytesRead++] = b;
                if (bytesRead >= expectedLen) {
                    outMsg.type = currentMsgType;
                    outMsg.length = bytesRead;
                    memcpy(outMsg.payload, payloadBuffer, bytesRead);
                    receivingMessage = false;
                    expectedLen = 0;
                    bytesRead = 0;
                    return true;
                }
            }
        }
    }

    return false; // no complete message yet
}

*/