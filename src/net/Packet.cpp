#include "net/Packet.h"

Packet* Packet::PACKETS[256] = {nullptr};
int Packet::nextId = 0;
Packet* Packet::LOGIN = new Packet(vector<FieldType>{FieldType::BYTE, FieldType::STRING, FieldType::STRING, FieldType::BYTE});
Packet* Packet::DUMMY = new Packet(vector<FieldType>{});
Packet* Packet::LEVEL_INITIALIZE = new Packet(vector<FieldType>{});
Packet* Packet::LEVEL_DATA_CHUNK = new Packet(vector<FieldType>{FieldType::SHORT, FieldType::BYTE_ARRAY, FieldType::BYTE});
Packet* Packet::LEVEL_FINALIZE = new Packet(vector<FieldType>{FieldType::SHORT, FieldType::SHORT, FieldType::SHORT});
Packet* Packet::PLACE_OR_REMOVE_TILE = new Packet(vector<FieldType>{FieldType::SHORT, FieldType::SHORT, FieldType::SHORT, FieldType::BYTE, FieldType::BYTE});
Packet* Packet::SET_TILE = new Packet(vector<FieldType>{FieldType::SHORT, FieldType::SHORT, FieldType::SHORT, FieldType::BYTE});
Packet* Packet::PLAYER_JOIN = new Packet(vector<FieldType>{FieldType::BYTE, FieldType::STRING, FieldType::SHORT, FieldType::SHORT, FieldType::SHORT, FieldType::BYTE, FieldType::BYTE});
Packet* Packet::PLAYER_TELEPORT = new Packet(vector<FieldType>{FieldType::BYTE, FieldType::SHORT, FieldType::SHORT, FieldType::SHORT, FieldType::BYTE, FieldType::BYTE});
Packet* Packet::PLAYER_MOVE_AND_ROTATE = new Packet(vector<FieldType>{FieldType::BYTE, FieldType::BYTE, FieldType::BYTE, FieldType::BYTE, FieldType::BYTE, FieldType::BYTE});
Packet* Packet::PLAYER_MOVE = new Packet(vector<FieldType>{FieldType::BYTE, FieldType::BYTE, FieldType::BYTE, FieldType::BYTE});
Packet* Packet::PLAYER_ROTATE = new Packet(vector<FieldType>{FieldType::BYTE, FieldType::BYTE, FieldType::BYTE});
Packet* Packet::PLAYER_DISCONNECT = new Packet(vector<FieldType>{FieldType::BYTE});
Packet* Packet::CHAT_MESSAGE = new Packet(vector<FieldType>{FieldType::BYTE, FieldType::STRING});
Packet* Packet::KICK_PLAYER = new Packet(vector<FieldType>{FieldType::STRING});

Packet::Packet(const vector<FieldType>& fieldTypes) 
    : id(static_cast<uint8_t>(nextId++)), size([&fieldTypes]() {
        int var2 = 0;
        for (size_t var3 = 0; var3 < fieldTypes.size(); ++var3) {
            FieldType var4 = fieldTypes[var3];
            if (var4 == FieldType::LONG) {
                var2 += 8;
            } else if (var4 == FieldType::INT) {
                var2 += 4;
            } else if (var4 == FieldType::SHORT) {
                var2 += 2;
            } else if (var4 == FieldType::BYTE) {
                ++var2;
            } else if (var4 == FieldType::FLOAT) {
                var2 += 4;
            } else if (var4 == FieldType::DOUBLE) {
                var2 += 8;
            } else if (var4 == FieldType::BYTE_ARRAY) {
                var2 += 1024;
            } else if (var4 == FieldType::STRING) {
                var2 += 64;
            }
        }
        return var2;
    }()) {
    PACKETS[this->id] = this;
    this->fields = vector<FieldType>(fieldTypes.size());
    for (size_t var3 = 0; var3 < fieldTypes.size(); ++var3) {
        this->fields[var3] = fieldTypes[var3];
    }
}