#include "comm/SocketConnection.h"
#include "net/ConnectionManager.h"
#include "net/Packet.h"
#include "net/NetworkPlayer.h"
#include "Minecraft.h"
#include "level/Level.h"
#include "level/LevelIO.h"

// obviously chatgpt'd

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

    // Resolve hostname
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
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(socketHandle, FIONBIO, &mode);
#else
    int flags = fcntl(socketHandle, F_GETFL, 0);
    fcntl(socketHandle, F_SETFL, flags | O_NONBLOCK);
#endif

    chrono::system_clock::now();
    
    this->connected = true;
    readPos = 0;
    writePos = 0;
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
#ifdef _WIN32
    DWORD timeout = 100;
    setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
#else
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif
}

void SocketConnection::disconnect() {
    this->connected = false;

    try {
        if (socketHandle != INVALID_SOCKET_HANDLE) {
            close(socketHandle);
            socketHandle = INVALID_SOCKET_HANDLE;
        }
    } catch (const exception &e) {
    }

#ifdef _WIN32
    WSACleanup();
#endif
}

void SocketConnection::processData() {
    if(this->writePos > 0) {
        int sent = send(socketHandle, (char*)writeBuffer.data(), writePos, 0);
        if (sent > 0) {
            memmove(writeBuffer.data(), writeBuffer.data() + sent, writePos - sent);
            writePos -= sent;
        }
    }

    int received = recv(socketHandle, (char*)(readBuffer.data() + readPos), readBuffer.size() - readPos, 0);
    if (received > 0) {
        readPos += received;
    }

    for (int var1 = 0; this->readPos > 0 && var1++ != 100; ) {
        if (readPos < 1) {
            break;
        }
        
        uint8_t var2 = readBuffer[0];
        Packet* var3 = Packet::PACKETS[var2];
        
        if (var3 == nullptr) {
            throw runtime_error("Bad command: " + to_string(var2));
        }

        if (this->readPos < var3->size + 1) {
            break;
        }

        size_t bufferPos = 1;
        vector<void*> var11(var3->fields.size());
        vector<string> stringValues;
        vector<vector<uint8_t>> byteArrayValues;

        for (size_t var4 = 0; var4 < var11.size(); ++var4) {
            FieldType var6 = var3->fields[var4];
            void* var10002;
            
            if (var6 == FieldType::LONG) {
                int64_t* val = new int64_t;
                memcpy(val, &readBuffer[bufferPos], 8);
                *val = be64toh(*val);
                var10002 = val;
                bufferPos += 8;
            } else if (var6 == FieldType::INT) {
                int32_t* val = new int32_t;
                memcpy(val, &readBuffer[bufferPos], 4);
                *val = ntohl(*val);
                var10002 = val;
                bufferPos += 4;
            } else if (var6 == FieldType::SHORT) {
                int16_t* val = new int16_t;
                memcpy(val, &readBuffer[bufferPos], 2);
                *val = ntohs(*val);
                var10002 = val;
                bufferPos += 2;
            } else if (var6 == FieldType::BYTE) {
                int8_t* val = new int8_t;
                *val = readBuffer[bufferPos];
                var10002 = val;
                bufferPos += 1;
            } else if (var6 == FieldType::DOUBLE) {
                double* val = new double;
                uint64_t temp;
                memcpy(&temp, &readBuffer[bufferPos], 8);
                temp = be64toh(temp);
                memcpy(val, &temp, 8);
                var10002 = val;
                bufferPos += 8;
            } else if (var6 == FieldType::FLOAT) {
                float* val = new float;
                uint32_t temp;
                memcpy(&temp, &readBuffer[bufferPos], 4);
                temp = ntohl(temp);
                memcpy(val, &temp, 4);
                var10002 = val;
                bufferPos += 4;
            } else if (var6 == FieldType::STRING) {
                memcpy(stringPacket, &readBuffer[bufferPos], 64);
                stringValues.push_back(string(reinterpret_cast<char*>(stringPacket), 64));
                stringValues.back().erase(stringValues.back().find_last_not_of(' ') + 1);
                var10002 = &stringValues.back();
                bufferPos += 64;
            } else if (var6 == FieldType::BYTE_ARRAY) {
                byteArrayValues.push_back(vector<uint8_t>(1024));
                memcpy(byteArrayValues.back().data(), &readBuffer[bufferPos], 1024);
                var10002 = &byteArrayValues.back();
                bufferPos += 1024;
            } else {
                var10002 = nullptr;
            }

            var11[var4] = var10002;
        }

        ConnectionManager* var5 = this->manager;

        shared_ptr<Minecraft> minecraft = var5->minecraft.lock();
        if (!minecraft) {
            return;
        }
        
        if (var3 == Packet::LOGIN) {
            minecraft->beginLevelLoading("Connecting to the server..");
        } else if (var3 == Packet::LEVEL_INITIALIZE) {
            minecraft->levelLoadUpdate("Loading level..");
            minecraft->setLevel(nullptr);
            var5->levelBuffer.clear();
        } else if (var3 == Packet::LEVEL_DATA_CHUNK) {
            int16_t var12 = *(int16_t*)var11[0];
            vector<uint8_t>* var14 = (vector<uint8_t>*)var11[1];
            int8_t var18 = *(int8_t*)var11[2];
            minecraft->setLoadingProgress(var18);
            var5->levelBuffer.insert(var5->levelBuffer.end(), var14->begin(), var14->begin() + var12);
        } else {
            int16_t var19;
            int16_t var23;
            
            if (var3 == Packet::LEVEL_FINALIZE) {
                vector<uint8_t> var13 = LevelIO::loadBlocks(var5->levelBuffer);
                var5->levelBuffer.clear();
                int16_t var16 = *(int16_t*)var11[0];
                var19 = *(int16_t*)var11[1];
                var23 = *(int16_t*)var11[2];
                shared_ptr<Level> var25 = make_shared<Level>();
                var25->setData(var16, var19, var23, var13);
                minecraft->setLevel(var25);
            } else if (var3 == Packet::SET_TILE) {
                if (minecraft->level != nullptr) {
                    minecraft->level->setTile(
                        *(int16_t*)var11[0], 
                        *(int16_t*)var11[1], 
                        *(int16_t*)var11[2], 
                        *(int8_t*)var11[3]
                    );
                }
            } else {
                int8_t var8;
                int8_t var17;
                int16_t var20;
                shared_ptr<NetworkPlayer> var22;
                
                if (var3 == Packet::PLAYER_JOIN) {
                    int8_t var10001 = *(int8_t*)var11[0];
                    string var30 = *(string*)var11[1];
                    int16_t var10003 = *(int16_t*)var11[2];
                    int16_t var10004 = *(int16_t*)var11[3];
                    int16_t var10005 = *(int16_t*)var11[4];
                    int8_t var10006 = *(int8_t*)var11[5];
                    int8_t var9 = *(int8_t*)var11[6];
                    var8 = var10006;
                    int16_t var26 = var10005;
                    var23 = var10004;
                    var20 = var10003;
                    string var21 = var30;
                    var17 = var10001;
                    
                    if (var17 >= 0) {
                        var22 = make_shared<NetworkPlayer>(
                            minecraft->level, var17, var21,
                            (float)var20 / 32.0f, (float)var23 / 32.0f, (float)var26 / 32.0f,
                            (float)(var8 * 360) / 256.0f, (float)(var9 * 360) / 256.0f
                        );
                        var5->players[var17] = var22;
                        minecraft->level->entities.push_back(var22);
                    } else {
                        minecraft->player->moveTo(
                            (float)var20 / 32.0f, (float)var23 / 32.0f, (float)var26 / 32.0f,
                            (float)(var8 * 360) / 256.0f, (float)(var9 * 360) / 256.0f
                        );
                    }
                } else if (var3 == Packet::PLAYER_MOVE) {
                    int8_t var10001 = *(int8_t*)var11[0];
                    int16_t var32 = *(int16_t*)var11[1];
                    int16_t var10003 = *(int16_t*)var11[2];
                    int16_t var10004 = *(int16_t*)var11[3];
                    int8_t var31 = *(int8_t*)var11[4];
                    var8 = *(int8_t*)var11[5];
                    int8_t var27 = var31;
                    var23 = var10004;
                    var20 = var10003;
                    var19 = var32;
                    var17 = var10001;
                    
                    if (var17 >= 0) {
                        shared_ptr<NetworkPlayer> var28 = var5->players[var17];
                        if (var28 != nullptr) {
                            var28->moveTo(
                                (float)var19 / 32.0f, (float)var20 / 32.0f, (float)var23 / 32.0f,
                                (float)(var8 * 360) / 256.0f, (float)(var27 * 360) / 256.0f
                            );
                        }
                    }
                } else if (var3 == Packet::PLAYER_DISCONNECT) {
                    var17 = *(int8_t*)var11[0];
                    if (var17 >= 0) {
                        var22 = var5->players[var17];
                        if (var22 != nullptr) {
                            var5->players.erase(var17);
                            auto& entities = minecraft->level->entities;
                            entities.erase(remove(entities.begin(), entities.end(), var22), entities.end());
                        }
                    }
                }
            }
        }

        for (size_t i = 0; i < var11.size(); ++i) {
            FieldType ft = var3->fields[i];
            if (ft != FieldType::STRING && ft != FieldType::BYTE_ARRAY && var11[i] != nullptr) {
                if (ft == FieldType::LONG) delete (int64_t*)var11[i];
                else if (ft == FieldType::INT) delete (int32_t*)var11[i];
                else if (ft == FieldType::SHORT) delete (int16_t*)var11[i];
                else if (ft == FieldType::BYTE) delete (int8_t*)var11[i];
                else if (ft == FieldType::DOUBLE) delete (double*)var11[i];
                else if (ft == FieldType::FLOAT) delete (float*)var11[i];
            }
        }

        memmove(readBuffer.data(), readBuffer.data() + var3->size + 1, readPos - var3->size - 1);
        this->readPos -= (var3->size + 1);
    }
}

void SocketConnection::sendPacket(Packet* var1, const vector<PacketValue>& var2) {
    this->writeBuffer[writePos++] = var1->id;

    for (int var3 = 0; var3 < var2.size(); ++var3) {
        FieldType var10001 = var1->fields[var3];
        const PacketValue& var6 = var2[var3];
        FieldType var5 = var10001;
        SocketConnection* var4 = this;
        
        if (var5 == FieldType::LONG) {
            int64_t val = get<int64_t>(var6);
            uint64_t netVal = htobe64(val);
            memcpy(&writeBuffer[writePos], &netVal, 8);
            writePos += 8;
        } else if (var5 == FieldType::INT) {
            int32_t val = get<int32_t>(var6);
            uint32_t netVal = htonl(val);
            memcpy(&writeBuffer[writePos], &netVal, 4);
            writePos += 4;
        } else if (var5 == FieldType::SHORT) {
            int16_t val = get<int16_t>(var6);
            uint16_t netVal = htons(val);
            memcpy(&writeBuffer[writePos], &netVal, 2);
            writePos += 2;
        } else if (var5 == FieldType::BYTE) {
            int8_t val = get<int8_t>(var6);
            writeBuffer[writePos++] = val;
        } else if (var5 == FieldType::DOUBLE) {
            double val = get<double>(var6);
            uint64_t temp;
            memcpy(&temp, &val, 8);
            temp = htobe64(temp);
            memcpy(&writeBuffer[writePos], &temp, 8);
            writePos += 8;
        } else if (var5 == FieldType::FLOAT) {
            float val = get<float>(var6);
            uint32_t temp;
            memcpy(&temp, &val, 4);
            temp = htonl(temp);
            memcpy(&writeBuffer[writePos], &temp, 4);
            writePos += 4;
        } else {
            vector<uint8_t> var7;
            if (var5 != FieldType::STRING) {
                if (var5 == FieldType::BYTE_ARRAY) {
                    var7 = get<vector<uint8_t>>(var6);
                    if (var7.size() < 1024) {
                        var7.resize(1024);
                    }
                    memcpy(&writeBuffer[writePos], var7.data(), 1024);
                    writePos += 1024;
                }
            } else {
                string str = get<string>(var6);
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
        }
    }
}