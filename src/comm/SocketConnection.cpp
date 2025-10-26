#include "comm/SocketConnection.h"
#include "net/ConnectionManager.h"
#include "net/Packet.h"
#include "net/NetworkPlayer.h"
#include "Minecraft.h"
#include "level/Level.h"
#include "level/LevelIO.h"
#include "gui/ErrorScreen.h"

// chatgpt'd

template<typename T, typename type>
T catch_get(const type& v, const char* fieldName = "unknown") {
    try {
        return get<T>(v);
    } catch (const bad_variant_access& e) {
        cerr << "ERROR: Failed to extract field '" << fieldName << "'" << endl;
        cerr << "  Expected type: " << typeid(T).name() << endl;
        cerr << "  Exception: " << e.what() << endl;
        throw;
    }
}

SocketConnection::SocketConnection(const string& var1, int var2) 
    : connected(false), socketHandle(INVALID_SOCKET_HANDLE), 
      readPos(0), writePos(0), manager(nullptr), initialized(false) {
    
    readBuffer.resize(1048576);
    writeBuffer.resize(1048576);
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw runtime_error("WSAStartup failed");
    }
#endif
    socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketHandle == INVALID_SOCKET_HANDLE) {
        throw runtime_error("Socket creation failed");
    }

    struct hostent* host = gethostbyname(var1.c_str());
    if (host == nullptr) {
        close(socketHandle);
        throw runtime_error("Host resolution failed");
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(var2);
    memcpy(&serverAddr.sin_addr, host->h_addr_list[0], host->h_length);

    if (connect(socketHandle, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(socketHandle);
        throw runtime_error("Failed to connect");
    }

    this->connected = true;
    readPos = 0;
    writePos = 0;
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(socketHandle, FIONBIO, &mode);
#else
    int flags = fcntl(socketHandle, F_GETFL, 0);
    fcntl(socketHandle, F_SETFL, flags | O_NONBLOCK);
#endif
    int flag = 1;
    setsockopt(socketHandle, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));
#ifndef _WIN32
    int tos = 24;
    setsockopt(socketHandle, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
#endif
    flag = 0;
    setsockopt(socketHandle, SOL_SOCKET, SO_KEEPALIVE, (char*)&flag, sizeof(int));
    flag = 0;
    setsockopt(socketHandle, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(int));
}

void SocketConnection::disconnect() {
    try {
        if (writePos > 0) {
            int sent = send(socketHandle, (char*)writeBuffer.data(), writePos, 0);
            if (sent > 0) {
                memmove(writeBuffer.data(), writeBuffer.data() + sent, writePos - sent);
                writePos -= sent;
            }
        }
    } catch (const exception& e) {}

    connected = false;

    try {
        if (socketHandle != INVALID_SOCKET_HANDLE) {
            close(socketHandle);
            socketHandle = INVALID_SOCKET_HANDLE;
        }
    } catch (const exception& e) {}

#ifdef _WIN32
    WSACleanup();
#endif
}

void SocketConnection::processDataFunc() {
    if (this->writePos > 0) {
        int sent = send(socketHandle, (char*)writeBuffer.data(), writePos, 0);
        if (sent > 0) {
            memmove(writeBuffer.data(), writeBuffer.data() + sent, writePos - sent);
            writePos -= sent;
        }
    }

    int received = recv(socketHandle, (char*)(readBuffer.data() + readPos), 
                        readBuffer.size() - readPos, 0);
    if (received > 0) {
        readPos += received;
    } else if (received == 0) {
        this->connected = false;
        throw runtime_error("Server closed connection");
#ifdef _WIN32
    } else if (WSAGetLastError() != WSAEWOULDBLOCK && WSAGetLastError() != WSAEINTR) {
        this->connected = false;
        throw runtime_error("recv error: " + to_string(WSAGetLastError()));
    }
#else
    } else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
        this->connected = false;
        throw runtime_error("recv error: " + string(strerror(errno)));
    }
#endif

    int32_t var1 = 0;

    while (readPos > 0 && var1++ != 100) {
        if (readPos < 1) {
            break;
        }

        uint8_t var2 = readBuffer[0];
        Packet* var3 = Packet::PACKETS[var2];

        if (var3 == nullptr) {
            throw runtime_error("Bad command: " + to_string(var2));
        }

        if (readPos < var3->size + 1) {
            break;
        }

        size_t packetSize = var3->size + 1;

        size_t savedReadPos = readPos;
        readPos = 1;

        vector<PacketValue> var11(var3->fields.size());
        for (size_t var4 = 0; var4 < var11.size(); ++var4) {
            var11[var4] = read(var3, var4);
        }

        ConnectionManager* var12 = manager;
        shared_ptr<Minecraft> minecraft = var12->minecraft.lock();
        if (!minecraft) {
            break;
        }

        if (manager->processData) {
            if (var3 == Packet::LOGIN) {
                minecraft->beginLevelLoading(catch_get<string>(var11[1]));
                minecraft->levelLoadUpdate(catch_get<string>(var11[2]));
                minecraft->player->userType = catch_get<int8_t>(var11[3]);
            } else if (var3 == Packet::LEVEL_INITIALIZE) {
                minecraft->setLevel(nullptr);
                var12->levelBuffer.clear();
            } else if (var3 == Packet::LEVEL_DATA_CHUNK) {
                int16_t var13 = catch_get<int16_t>(var11[0]);
                vector<uint8_t> var5 = catch_get<vector<uint8_t>>(var11[1]);
                int8_t var6 = catch_get<int8_t>(var11[2]);
                minecraft->setLoadingProgress(var6);
                var12->levelBuffer.insert(var12->levelBuffer.end(), var5.begin(), 
                                        var5.begin() + var13);
            } else if (var3 == Packet::LEVEL_FINALIZE) {
                int16_t width = catch_get<int16_t>(var11[0]);
                int16_t depth = catch_get<int16_t>(var11[1]);
                int16_t height = catch_get<int16_t>(var11[2]);
                vector<uint8_t> blocks = LevelIO::loadBlocks(var12->levelBuffer);
                var12->levelBuffer.clear();
                shared_ptr<Level> level = make_shared<Level>();
                level->setNetworkMode(true);
                level->setData(width, depth, height, blocks);
                minecraft->setLevel(level);
                minecraft->hideGui = false;
                manager->connected = true;
            } else if (var3 == Packet::SET_TILE) {
                if (minecraft->level != nullptr) {
                    minecraft->level->netSetTile(
                        catch_get<int16_t>(var11[0]),
                        catch_get<int16_t>(var11[1]),
                        catch_get<int16_t>(var11[2]),
                        catch_get<int8_t>(var11[3])
                    );
                }
            } else if (var3 == Packet::PLAYER_JOIN) {
                int8_t var15 = catch_get<int8_t>(var11[0]);
                string var19 = catch_get<string>(var11[1]);
                int16_t var18_2 = catch_get<int16_t>(var11[2]);
                int16_t var21_2 = catch_get<int16_t>(var11[3]);
                int16_t var24 = catch_get<int16_t>(var11[4]);
                int8_t var8 = catch_get<int8_t>(var11[5]);
                int8_t var9 = catch_get<int8_t>(var11[6]);

                if (var15 >= 0) {
                    shared_ptr<NetworkPlayer> var20 = make_shared<NetworkPlayer>(
                        minecraft, var15, var19, var18_2, var21_2, var24,
                        (float)(-var8 * 360) / 256.0f, (float)(var9 * 360) / 256.0f
                    );
                    var12->players[var15] = var20;
                    minecraft->level->entities.push_back(var20);
                } else {
                    minecraft->level->setSpawnPos(var18_2 / 32, var21_2 / 32, var24 / 32, 
                              (float)(var8 * 320 / 256));
                    minecraft->player->moveTo(
                        (float)var18_2 / 32.0f, (float)var21_2 / 32.0f, (float)var24 / 32.0f,
                        (float)(var8 * 360) / 256.0f, (float)(var9 * 360) / 256.0f
                    );
                }
            } else if (var3 == Packet::PLAYER_TELEPORT) {
                int8_t var15 = catch_get<int8_t>(var11[0]);
                int16_t x = catch_get<int16_t>(var11[1]);
                int16_t y = catch_get<int16_t>(var11[2]);
                int16_t z = catch_get<int16_t>(var11[3]);
                int8_t yRot = catch_get<int8_t>(var11[4]);
                int8_t xRot = catch_get<int8_t>(var11[5]);

                if (var15 < 0) {
                    minecraft->player->moveTo((float)x / 32.0f, 
                        (float)y / 32.0f, (float)z / 32.0f, 
                        (float)(yRot * 360) / 256.0f, (float)(xRot * 360));
                }
                else {
                    shared_ptr<NetworkPlayer>& var28 = var12->players[var15];
                    if (var28 != nullptr) {
                        var28->teleport(x, y, z,
                            (float)(-yRot * 360) / 256.0f, 
                            (float)(xRot * 360) / 256.0f);
                    }
                }
            } else if (var3 == Packet::PLAYER_MOVE_AND_ROTATE) {
                int8_t var15 = catch_get<int8_t>(var11[0]);
                int8_t var30 = catch_get<int8_t>(var11[1]);
                int8_t var31 = catch_get<int8_t>(var11[2]);
                int8_t var6_2 = catch_get<int8_t>(var11[3]);
                int8_t var33 = catch_get<int8_t>(var11[4]);
                int8_t var8_3 = catch_get<int8_t>(var11[5]);

                if (var15 >= 0) {
                    shared_ptr<NetworkPlayer>& var28 = var12->players[var15];
                    if (var28 != nullptr) {
                        var28->queue(var30, var31, var6_2,
                            (float)(-var33 * 360) / 256.0f, (float)(var8_3 * 360) / 256.0f);
                    }
                }
            } else if (var3 == Packet::PLAYER_ROTATE) {
                int8_t var15 = catch_get<int8_t>(var11[0]);
                int8_t var30 = catch_get<int8_t>(var11[1]);
                int8_t var23 = catch_get<int8_t>(var11[2]);

                if (var15 >= 0) {
                    shared_ptr<NetworkPlayer>& var26 = var12->players[var15];
                    if (var26 != nullptr) {
                        var26->queue((float)(-var30 * 360) / 256.0f, 
                                (float)(var23 * 360) / 256.0f);
                    }
                }
            } else if (var3 == Packet::PLAYER_MOVE) {
                int8_t var15 = catch_get<int8_t>(var11[0]);
                int8_t var30 = catch_get<int8_t>(var11[1]);
                int8_t var31 = catch_get<int8_t>(var11[2]);
                int8_t var6_3 = catch_get<int8_t>(var11[3]);

                if (var15 >= 0) {
                    shared_ptr<NetworkPlayer>& var27 = var12->players[var15];
                    if (var27 != nullptr) {
                        var27->queue(var30, var31, var6_3);
                    }
                }
            } else if (var3 == Packet::PLAYER_DISCONNECT) {
                int8_t var15 = catch_get<int8_t>(var11[0]);
                if (var15 >= 0) {
                    shared_ptr<NetworkPlayer>& var20 = var12->players[var15];
                    var12->players.erase(var15);
                    if (var20 != nullptr) {
                        var20->clear();
                        auto& entities = minecraft->level->entities;
                        entities.erase(remove(entities.begin(), entities.end(), var20), entities.end());
                    }
                }
            } else if (var3 == Packet::CHAT_MESSAGE) {
                int8_t var15 = catch_get<int8_t>(var11[0]);
                string var19 = catch_get<string>(var11[1]);

                if (var15 < 0) {
                    minecraft->addChatMessage("&e" + var19);
                } else {
                    minecraft->addChatMessage(var19);
                }
            } else if (var3 == Packet::KICK_PLAYER) {
                string reason = catch_get<string>(var11[0]);
                minecraft->setScreen(make_shared<ErrorScreen>("Connection lost", reason));
            }
        }

        memmove(readBuffer.data(), readBuffer.data() + packetSize, savedReadPos - packetSize);
        readPos = savedReadPos - packetSize;

        if (!connected) {
            break;
        }
    }

    if (writePos > 0) {
        int sent = send(socketHandle, (char*)writeBuffer.data(), writePos, 0);
        if (sent > 0) {
            memmove(writeBuffer.data(), writeBuffer.data() + sent, writePos - sent);
            writePos -= sent;
        }
    }
}

void SocketConnection::sendPacket(Packet* var1, const vector<PacketValue>& var2) {
    if (connected) {
        this->writeBuffer[writePos++] = var1->id;

        for (int var3 = 0; var3 < var2.size(); ++var3) {
            FieldType var10001 = var1->fields[var3];
            const PacketValue& var6 = var2[var3];
            FieldType var5 = var10001;
            SocketConnection* var4 = this;
            vector<uint8_t> var7;
            if (connected) {
                try {
                    if (var5 == FieldType::LONG) {
                        int64_t val = catch_get<int64_t>(var6);
                        uint64_t netVal = htobe64(val);
                        memcpy(&writeBuffer[writePos], &netVal, 8);
                        writePos += 8;
                    } else if (var5 == FieldType::INT) {
                        int32_t val = catch_get<int32_t>(var6);
                        uint32_t netVal = htonl(val);
                        memcpy(&writeBuffer[writePos], &netVal, 4);
                        writePos += 4;
                    } else if (var5 == FieldType::SHORT) {
                        int16_t val = catch_get<int16_t>(var6);
                        uint16_t netVal = htons(val);
                        memcpy(&writeBuffer[writePos], &netVal, 2);
                        writePos += 2;
                    } else if (var5 == FieldType::BYTE) {
                        int8_t val = catch_get<int8_t>(var6);
                        writeBuffer[writePos++] = val;
                    } else if (var5 == FieldType::DOUBLE) {
                        double val = catch_get<double>(var6);
                        uint64_t temp;
                        memcpy(&temp, &val, 8);
                        temp = htobe64(temp);
                        memcpy(&writeBuffer[writePos], &temp, 8);
                        writePos += 8;
                    } else if (var5 == FieldType::FLOAT) {
                        float val = catch_get<float>(var6);
                        uint32_t temp;
                        memcpy(&temp, &val, 4);
                        temp = htonl(temp);
                        memcpy(&writeBuffer[writePos], &temp, 4);
                        writePos += 4;
                    } else if (var5 == FieldType::BYTE_ARRAY) {
                        var7 = catch_get<vector<uint8_t>>(var6);
                        if (var7.size() < 1024) {
                            var7.resize(1024);
                        }
                        memcpy(&writeBuffer[writePos], var7.data(), 1024);
                        writePos += 1024;
                    } else if (var5 == FieldType::STRING) {
                        string str = catch_get<string>(var6);
                        var7 = vector<uint8_t>(str.begin(), str.end());
                        fill(var4->stringPacket, var4->stringPacket + 64, 32);
                        
                        int var8;
                        for (var8 = 0; var8 < 64 && var8 < var7.size(); ++var8) {
                            var4->stringPacket[var8] = var7[var8];
                        }
                        
                        for (var8 = var7.size(); var8 < 64; ++var8) {
                            var4->stringPacket[var8] = 32;
                        }
                        
                        memcpy(&writeBuffer[writePos], var4->stringPacket, 64);
                        writePos += 64;
                    }
                } catch (const exception& e) {
                    manager->disconnect(e);
                }
            }
        }
    }
}

PacketValue SocketConnection::read(Packet* packet, size_t fieldIndex) {
    if (!connected) {
        return PacketValue(int8_t(0));
    }

    try {
        FieldType type = packet->fields[fieldIndex];

        if (type == FieldType::LONG) {
            int64_t val;
            memcpy(&val, readBuffer.data() + readPos, 8);
            val = be64toh(val);
            readPos += 8;
            return PacketValue(val);
        } else if (type == FieldType::INT) {
            int32_t val;
            memcpy(&val, readBuffer.data() + readPos, 4);
            val = ntohl(val);
            readPos += 4;
            return PacketValue(val);
        } else if (type == FieldType::SHORT) {
            int16_t val;
            memcpy(&val, readBuffer.data() + readPos, 2);
            val = ntohs(val);
            readPos += 2;
            return PacketValue(val);
        } else if (type == FieldType::BYTE) {
            int8_t val = readBuffer[readPos++];
            return PacketValue(val);
        } else if (type == FieldType::DOUBLE) {
            double val;
            uint64_t temp;
            memcpy(&temp, readBuffer.data() + readPos, 8);
            temp = be64toh(temp);
            memcpy(&val, &temp, 8);
            readPos += 8;
            return PacketValue(val);
        } else if (type == FieldType::FLOAT) {
            float val;
            uint32_t temp;
            memcpy(&temp, readBuffer.data() + readPos, 4);
            temp = ntohl(temp);
            memcpy(&val, &temp, 4);
            readPos += 4;
            return PacketValue(val);
        } else if (type == FieldType::STRING) {
            memcpy(stringPacket, readBuffer.data() + readPos, 64);
            readPos += 64;
            size_t len = 0;
            for (size_t i = 0; i < 64; ++i) {
                if (stringPacket[i] == '\0') {
                    len = i;
                    break;
                }
                len = i + 1;
            }
            // Trim trailing spaces
            while (len > 0 && stringPacket[len - 1] == ' ') {
                len--;
            }
            string result(reinterpret_cast<char*>(stringPacket), len);
            return PacketValue(result);
        } else if (type == FieldType::BYTE_ARRAY) {
            vector<uint8_t> result(1024);
            memcpy(result.data(), readBuffer.data() + readPos, 1024);
            readPos += 1024;
            return PacketValue(result);
        }
    } catch (const exception& e) {
        manager->disconnect(e);
    }

    return PacketValue(int8_t(0));
}