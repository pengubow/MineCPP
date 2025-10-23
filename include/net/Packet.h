#pragma once

#include <cstdint>
#include <vector>

using namespace std;

enum class FieldType {
    LONG,
    INT,
    SHORT,
    BYTE,
    DOUBLE,
    FLOAT,
    STRING,
    BYTE_ARRAY
};

class Packet {
public:
    static Packet* PACKETS[256];
    static Packet* LOGIN;
    static Packet* DUMMY;
    static Packet* LEVEL_INITIALIZE;
    static Packet* LEVEL_DATA_CHUNK;
    static Packet* LEVEL_FINALIZE;
    static Packet* PLACE_OR_REMOVE_TILE;
    static Packet* SET_TILE;
    static Packet* PLAYER_JOIN;
    static Packet* PLAYER_TELEPORT;
    static Packet* PLAYER_MOVE_AND_ROTATE;
    static Packet* PLAYER_MOVE;
    static Packet* PLAYER_ROTATE;
    static Packet* PLAYER_DISCONNECT;
    static Packet* CHAT_MESSAGE;
    static Packet* KICK_PLAYER;

    const int size;
    const uint8_t id;
    vector<FieldType> fields;
private:
    static int nextId;
    
    Packet(const vector<FieldType>& fieldTypes);
};