#include <zlib.h>
#include <cstring>
#include <functional>
#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <netinet/in.h>
#endif
#include "Util/httplib.h"
#include "level/LevelIO.h"
#include "Minecraft.h"
#include "Util/gzip.h"

using namespace std;

LevelIO::LevelIO(shared_ptr<Minecraft>& minecraft)
    : minecraft(minecraft) {}

// quite a bit of LevelIO is chatgpt'd btw
shared_ptr<Level> LevelIO::load(const string& minecraftUri, const string& username, int levelId) {
    shared_ptr<Minecraft> mc = minecraft.lock();
    
    if (mc != nullptr) {
        mc->beginLevelLoading("Loading level");
    }
    
    try {
        if (mc != nullptr) {
            mc->levelLoadUpdate("Connecting..");
        }
        
        // Parse host and port
        string host = minecraftUri;
        int port = 80;
        size_t colonPos = host.find(':');
        if (colonPos != string::npos) {
            port = stoi(host.substr(colonPos + 1));
            host = host.substr(0, colonPos);
        }
        
        httplib::Client cli(host, port);
        cli.set_connection_timeout(10, 0);
        cli.set_follow_location(true);
        
        string path = "/level/load.html?id=" + to_string(levelId) + "&user=" + username;

        if (mc != nullptr) {
            mc->levelLoadUpdate("Loading..");
        }
        
        auto res = cli.Get(path);
        
        if (!res || res->status != 200) {
            if (mc != nullptr) {
                mc->levelLoadUpdate("Failed to connect!");
            }
            this_thread::sleep_for(chrono::seconds(1));
            return nullptr;
        }

        if (res->body.size() < 10) {
            cerr << "Response too short to be valid level data" << endl;
            if (mc != nullptr) {
                mc->levelLoadUpdate("No level data received");
            }
            this_thread::sleep_for(chrono::seconds(1));
            return nullptr;
        }

        const vector<uint8_t> data(res->body.begin(), res->body.end());
        size_t pos = 0;
        
        auto readUTF = [&]() -> string {
            if (pos + 2 > data.size()) throw runtime_error("Unexpected end of data");
            uint16_t len = (data[pos] << 8) | data[pos + 1];
            pos += 2;
            if (pos + len > data.size()) throw runtime_error("Unexpected end of data");
            string str(data.begin() + pos, data.begin() + pos + len);
            pos += len;
            return str;
        };
        
        string status = readUTF();
        
        if (status != "ok") {
            string errorMsg = "";
            try {
                errorMsg = readUTF();
            } catch (...) {
                errorMsg = "Unknown error";
            }
            cerr << "Server returned error: " << errorMsg << endl;
            if (mc != nullptr) {
                mc->levelLoadUpdate("Failed: " + errorMsg);
            }
            this_thread::sleep_for(chrono::seconds(1));
            return nullptr;
        }
        
        vector<uint8_t> gzData(data.begin() + pos, data.end());
        
        if (gzData.size() < 10) {
            cerr << "Gzip data too small" << endl;
            if (mc != nullptr) {
                mc->levelLoadUpdate("Invalid level data");
            }
            return nullptr;
        }

        if (gzData[0] != 0x1f || gzData[1] != 0x8b) {
            cerr << "Not valid gzip data (magic: " << hex << (int)gzData[0] << " " << (int)gzData[1] << ")" << endl;
        }

        const char* tempFile = "temp_load.gz";
        FILE* f = fopen(tempFile, "wb");
        if (!f) {
            throw runtime_error("Failed to create temp file");
        }
        fwrite(gzData.data(), 1, gzData.size(), f);
        fclose(f);
        
        gzFile gzf = gzopen(tempFile, "rb");
        if (!gzf) {
            remove(tempFile);
            throw runtime_error("Failed to open gzip file");
        }

        shared_ptr<Level> level = loadDat(gzf);
        
        remove(tempFile);
        
        if (!level && mc != nullptr) {
            mc->levelLoadUpdate("Failed to parse level data");
        }
        
        return level;
        
    } catch (const exception& e) {
        cerr << "Load level exception: " << e.what() << endl;
        if (mc != nullptr) {
            mc->levelLoadUpdate("Failed!");
        }
        this_thread::sleep_for(chrono::seconds(3));
        return nullptr;
    }
}

// this was made by fucking AI
shared_ptr<Level> LevelIO::loadDat(gzFile file) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (minecraft != nullptr) {
        minecraft->beginLevelLoading("Loading level");
    }
    if (minecraft != nullptr) {
        minecraft->levelLoadUpdate("Reading..");
    }

    try {
        const size_t CHUNK = 8192;
        vector<uint8_t> buf;
        buf.resize(1 << 20);
        uint8_t tmp[CHUNK];
        int r;
        while ((r = gzread(file, tmp, CHUNK)) > 0) {
            buf.insert(buf.end(), tmp, tmp + r);
        }
        gzclose(file);

        size_t pos = 0;
        auto ensure = [&](size_t n) {
            if (pos + n > buf.size()) throw runtime_error("Unexpected end of buffer");
        };
        auto readByte = [&]() -> uint8_t { ensure(1); return buf[pos++]; };
        auto readBytes = [&](void* out, size_t n) {
            if (n == 0) return;
            ensure(n);
            memcpy(out, buf.data() + pos, n);
            pos += n;
        };
        auto readInt16BE = [&]() -> int16_t { 
            ensure(2); 
            int16_t v = (int16_t)((buf[pos] << 8) | buf[pos+1]); 
            pos += 2; 
            return v; 
        };
        auto readInt32BE = [&]() -> int32_t {
            ensure(4);
            uint32_t v = (uint32_t(buf[pos]) << 24) | (uint32_t(buf[pos+1]) << 16) | 
                         (uint32_t(buf[pos+2]) << 8) | uint32_t(buf[pos+3]);
            pos += 4;
            return (int32_t)v;
        };
        auto readInt64BE = [&]() -> int64_t {
            ensure(8);
            uint64_t v = (uint64_t(buf[pos]) << 56) | (uint64_t(buf[pos+1]) << 48) |
                         (uint64_t(buf[pos+2]) << 40) | (uint64_t(buf[pos+3]) << 32) |
                         (uint64_t(buf[pos+4]) << 24) | (uint64_t(buf[pos+5]) << 16) |
                         (uint64_t(buf[pos+6]) << 8)  | (uint64_t(buf[pos+7]));
            pos += 8;
            return (int64_t)v;
        };
        auto readFloatBE = [&]() -> float {
            int32_t bits = readInt32BE();
            float result;
            memcpy(&result, &bits, sizeof(float));
            return result;
        };
        auto readStringBE = [&]() -> string {
            int16_t len = readInt16BE();
            if (len < 0) throw runtime_error("Negative string length");
            if (len == 0) return string();
            ensure((size_t)len);
            string s;
            s.resize((size_t)len);
            memcpy(&s[0], buf.data() + pos, (size_t)len);
            pos += (size_t)len;
            return s;
        };

        auto skip = [&](size_t n) { ensure(n); pos += n; };

        const uint8_t TC_NULL = 0x70, TC_REFERENCE = 0x71;
        const uint8_t TC_CLASSDESC = 0x72, TC_OBJECT = 0x73;
        const uint8_t TC_STRING = 0x74, TC_ARRAY = 0x75;
        const uint8_t TC_ENDBLOCKDATA = 0x78;

        function<pair<string, vector<tuple<char,string,string>>>()> readClassDesc;
        readClassDesc = [&]() -> pair<string, vector<tuple<char,string,string>>> {
            vector<tuple<char,string,string>> fields;
            uint8_t tc = readByte();
            if (tc == TC_NULL) return {"", fields};
            if (tc != TC_CLASSDESC) {
                throw runtime_error("Expected TC_CLASSDESC");
            }

            string className = readStringBE();
            readInt64BE(); // serialVersionUID
            uint8_t flags = readByte();

            int16_t fcount = readInt16BE();
            if (fcount < 0) throw runtime_error("Negative field count");
            
            for (int i = 0; i < fcount; ++i) {
                char ftype = (char)readByte();
                string fname = readStringBE();
                string class1;
                if (ftype == '[' || ftype == 'L') {
                    uint8_t t = readByte();
                    if (t == TC_STRING) {
                        class1 = readStringBE();
                    } else if (t == TC_REFERENCE) {
                        int32_t ref = readInt32BE();
                        class1 = "(reference)";
                    } else {
                        throw runtime_error("Expected TC_STRING or TC_REFERENCE");
                    }
                }
                fields.emplace_back(ftype, fname, class1);
            }
            
            uint8_t end = readByte();
            if (end != TC_ENDBLOCKDATA) {
                throw runtime_error("Expected TC_ENDBLOCKDATA");
            }

            uint8_t superTC = readByte();
            if (superTC != TC_NULL) {
                pos--;
                auto [superClassName, superFields] = readClassDesc();
                superFields.insert(superFields.end(), fields.begin(), fields.end());
                return {className, superFields};
            }
            return {className, fields};
        };

        int32_t magic = readInt32BE();
        if (magic != 0x271BB788) {
            throw runtime_error("Invalid .dat magic number");
        }

        uint8_t versionByte = readByte();
        if (versionByte != 0x02) {
            throw runtime_error("Invalid .dat version");
        }

        int16_t streamMagic = readInt16BE();
        int16_t streamVersion = readInt16BE();
        if ((uint16_t)streamMagic != 0xACED || streamVersion != 0x0005) {
            throw runtime_error("Invalid Java serialization header");
        }

        uint8_t topcode = readByte();
        if (topcode != TC_OBJECT) {
            throw runtime_error("Expected TC_OBJECT");
        }

        auto [levelClassName, topFields] = readClassDesc();

        int width = 0, height = 0, depth = 0;
        int xSpawn = 0, ySpawn = 0, zSpawn = 0;
        float rotSpawn = 0.0f;
        vector<uint8_t> blocks;
        string name, creator;
        int64_t createTime = 0;
        vector<shared_ptr<Entity>> entities;
        int unprocessed = 0;
        int tickCount = 0;

        shared_ptr<Level> level = make_shared<Level>();

        for (size_t i = 0; i < topFields.size(); ++i) {
            char ftype = get<0>(topFields[i]);
            string fname = get<1>(topFields[i]);


            if (fname == "width" || fname == "height" || fname == "depth" ||
                fname == "xSpawn" || fname == "ySpawn" || fname == "zSpawn" ||
                fname == "unprocessed" || fname == "tickCount") {
                int32_t val = readInt32BE();
                if (fname == "width") width = val;
                else if (fname == "height") height = val;
                else if (fname == "depth") depth = val;
                else if (fname == "xSpawn") xSpawn = val;
                else if (fname == "ySpawn") ySpawn = val;
                else if (fname == "zSpawn") zSpawn = val;
                else if (fname == "unprocessed") unprocessed = val;
                else if (fname == "tickCount") tickCount = val;
            }
            else if (fname == "blocks") {
                uint8_t tc = readByte();
                if (tc == TC_NULL) {
                    throw runtime_error("Blocks array is null");
                } else if (tc == TC_ARRAY) {
                    auto [arrayClassName, arrayFields] = readClassDesc();
                    int32_t len = readInt32BE();
                    if (len < 0) throw runtime_error("Negative array length");
                    if (len > 0) {
                        blocks.resize((size_t)len);
                        readBytes(blocks.data(), blocks.size());
                    }
                } else {
                    throw runtime_error("Expected TC_ARRAY for blocks");
                }
            }
            else if (fname == "name" || fname == "creator") {
                uint8_t tc = readByte();
                if (tc == TC_NULL) {
                    if (fname == "name") name = "";
                    else creator = "";
                } else if (tc == TC_STRING) {
                    string str = readStringBE();
                    if (fname == "name") name = str;
                    else creator = str;
                } else if (tc == TC_REFERENCE) {
                    readInt32BE();
                    if (fname == "name") name = "";
                    else creator = "";
                }
            }
            else if (fname == "createTime") {
                createTime = readInt64BE();
            }
            else if (fname == "rotSpawn") {
                rotSpawn = readFloatBE();
            }
            else if (fname == "entities") {
                uint8_t tc = readByte();
                if (tc == TC_NULL) {
                    entities.clear();
                }
                else if (tc == TC_OBJECT) {
                    auto [arrayListClassName, arrayListFields] = readClassDesc();
                    
                    int32_t listSize = 0;
                    
                    for (size_t i = 0; i < arrayListFields.size(); ++i) {
                        char ftype = get<0>(arrayListFields[i]);
                        string fname = get<1>(arrayListFields[i]);
                        
                        if (ftype == 'I') {
                            int32_t val = readInt32BE();
                            if (fname == "size") {
                                listSize = val;
                            }
                        }
                        else if (ftype == '[') {
                            uint8_t arrayTC = readByte();
                            if (arrayTC == TC_ARRAY) {
                                auto [elemArrayClassName, elemArrayFields] = readClassDesc();
                                readInt32BE();
                            } else if (arrayTC == TC_NULL) {}
                        }
                    }
                    
                    uint8_t blockTC = readByte();
                    if (blockTC == 0x77) {
                        uint8_t blockSize = readByte();
                        if (blockSize == 4) {
                            int32_t sizeAgain = readInt32BE();
                        } else {
                            skip(blockSize);
                        }
                    } else if (blockTC == 0x7A) {
                        int32_t blockSize = readInt32BE();
                        skip(blockSize);
                    } else {
                        pos--;
                    }
                    
                    for (int32_t j = 0; j < listSize; j++) {                        
                        uint8_t objTC = readByte();
                        if (objTC == TC_NULL) {
                            continue;
                        }
                        
                        if (objTC != TC_OBJECT) {
                            break;
                        }
                        
                        auto [entityClassName, allFields] = readClassDesc();
                        
                        cout << "    Entity class: " << entityClassName << endl;

                        float x = 0, y = 0, z = 0;
                        float xd = 0, yd = 0, zd = 0;
                        float xo = 0, yo = 0, zo = 0;
                        float yRot = 0, xRot = 0, yRotO = 0, xRotO = 0;
                        float heightOffset = 0.0f;
                        float bbWidth = 0.6f, bbHeight = 1.8f;
                        bool onGround = false, horizontalCollision = false, removed = false;
                        optional<AABB> bb;

                        float rot = 0, timeOffs = 0, speed = 0, rotA = 0;

                        for (const auto& field : allFields) {
                            char ftype = get<0>(field);
                            string fname = get<1>(field);
                            
                            switch (ftype) {
                                case 'F': {
                                    float val = readFloatBE();
                                    if (fname == "rot") rot = val;
                                    else if (fname == "rotA") rotA = val;
                                    else if (fname == "speed") speed = val;
                                    else if (fname == "timeOffs") timeOffs = val;
                                    else if (fname == "bbHeight") bbHeight = val;
                                    else if (fname == "bbWidth") bbWidth = val;
                                    else if (fname == "heightOffset") heightOffset = val;
                                    else if (fname == "x") x = val;
                                    else if (fname == "xRot") xRot = val;
                                    else if (fname == "xRotO") xRotO = val;
                                    else if (fname == "xd") xd = val;
                                    else if (fname == "xo") xo = val;
                                    else if (fname == "y") y = val;
                                    else if (fname == "yRot") yRot = val;
                                    else if (fname == "yRotO") yRotO = val;
                                    else if (fname == "yd") yd = val;
                                    else if (fname == "yo") yo = val;
                                    else if (fname == "z") z = val;
                                    else if (fname == "zd") zd = val;
                                    else if (fname == "zo") zo = val;
                                    break;
                                }
                                case 'Z': {
                                    bool val = readByte() != 0;
                                    if (fname == "horizontalCollision") horizontalCollision = val;
                                    else if (fname == "onGround") onGround = val;
                                    else if (fname == "removed") removed = val;
                                    break;
                                }
                                case 'L': {
                                    uint8_t objRefTC = readByte();
                                    if (fname == "bb") {
                                        if (objRefTC == TC_OBJECT) {
                                            auto [aabbClassName, aabbFields] = readClassDesc();
                                            float x0=0, y0=0, z0=0, x1=0, y1=0, z1=0;
                                            for (const auto& af : aabbFields) {
                                                if (get<0>(af) == 'F') {
                                                    float v = readFloatBE();
                                                    string n = get<1>(af);
                                                    if (n == "epsilon") {}
                                                    else if (n == "x0") x0 = v;
                                                    else if (n == "y0") y0 = v;
                                                    else if (n == "z0") z0 = v;
                                                    else if (n == "x1") x1 = v;
                                                    else if (n == "y1") y1 = v;
                                                    else if (n == "z1") z1 = v;
                                                }
                                            }
                                            bb = AABB(x0, y0, z0, x1, y1, z1);
                                        } else if (objRefTC == TC_REFERENCE) {
                                            readInt32BE();
                                            bb = AABB(0, 0, 0, 0, 0, 0);
                                        } else if (objRefTC == TC_NULL) {
                                            bb = AABB(0, 0, 0, 0, 0, 0);
                                        }
                                    } else if (fname == "level") {
                                        if (objRefTC == TC_REFERENCE) readInt32BE();
                                        else if (objRefTC == TC_OBJECT) {
                                            auto [objClassName, objFields] = readClassDesc();
                                        }
                                        else if (objRefTC == TC_NULL) {}
                                    } else {
                                        if (objRefTC == TC_STRING) readStringBE();
                                        else if (objRefTC == TC_REFERENCE) readInt32BE();
                                        else if (objRefTC == TC_NULL) {}
                                    }
                                    break;
                                }
                                case 'I': readInt32BE(); break;
                                case 'J': readInt64BE(); break;
                                case 'D': readInt64BE(); break;
                                case 'B': readByte(); break;
                                case 'S': readInt16BE(); break;
                                case 'C': readInt16BE(); break;
                                case '[': {
                                    uint8_t arrTC = readByte();
                                    if (arrTC == TC_ARRAY) {
                                        auto [arrClassName, arrFields] = readClassDesc();
                                        readInt32BE();
                                    } else if (arrTC == TC_NULL) {}
                                    break;
                                }
                            }
                        }
                        
                        shared_ptr<Entity> entity;
                        
                        if (entityClassName == "com.mojang.minecraft.character.Zombie") {
                            auto zombie = make_shared<Zombie>(level, x, y, z);
                            zombie->rot = rot;
                            zombie->timeOffs = timeOffs;
                            zombie->speed = speed;
                            zombie->rotA = rotA;
                            entity = zombie;
                        }
                        else {
                            cerr << "    Unknown entity type: " << entityClassName << endl;
                            entity = make_shared<Entity>(level);
                        }
                        
                        entity->x = x; entity->y = y; entity->z = z;
                        entity->xd = xd; entity->yd = yd; entity->zd = zd;
                        entity->xo = xo; entity->yo = yo; entity->zo = zo;
                        entity->yRot = yRot; entity->xRot = xRot;
                        entity->yRotO = yRotO; entity->xRotO = xRotO;
                        entity->heightOffset = heightOffset;
                        entity->bbWidth = bbWidth; entity->bbHeight = bbHeight;
                        entity->onGround = onGround;
                        entity->horizontalCollision = horizontalCollision;
                        entity->removed = removed;
                        entity->bb = bb.has_value() ? bb.value() : AABB(0, 0, 0, 0, 0, 0);
                        
                        entities.push_back(entity);
                    }
                    
                    uint8_t endBlock = readByte();
                    if (endBlock != TC_ENDBLOCKDATA) {
                        cerr << "  Expected TC_ENDBLOCKDATA after objects, got 0x" 
                                 << hex << (int)endBlock << dec << endl;
                    }
                    
                    cout << "  Loaded " << entities.size() << " entities" << endl;
                }
            }
            else {
                switch (ftype) {
                    case 'B': readByte(); break;
                    case 'I': readInt32BE(); break;
                    case 'J': readInt64BE(); break;
                    case 'F': readFloatBE(); break;
                    case 'Z': readByte(); break;
                    case 'L': {
                        uint8_t tc = readByte();
                        if (tc == TC_NULL) {}
                        else if (tc == TC_STRING) { readStringBE(); }
                        else if (tc == TC_REFERENCE) { readInt32BE(); }
                        break;
                    }
                    case '[': {
                        uint8_t tc = readByte();
                        if (tc == TC_ARRAY) {
                            auto [arrClassName, arrFields] = readClassDesc();
                            int32_t len = readInt32BE();
                            skip(len);
                        }
                        break;
                    }
                }
            }
        }

        const size_t TILE_ARRAY_SIZE = 256;
        for (size_t i = 0; i < blocks.size(); ++i) {
            if (blocks[i] >= TILE_ARRAY_SIZE || Tile::tiles[blocks[i]] == nullptr) {
                blocks[i] = 0;
            }
        }

        level->setData(width, depth, height, blocks);
        level->name = name.empty() ? "--" : name;
        level->creator = creator.empty() ? "unknown" : creator;
        level->createTime = createTime;
        level->xSpawn = xSpawn;
        level->ySpawn = ySpawn;
        level->zSpawn = zSpawn;
        level->rotSpawn = rotSpawn;
        level->entities = entities;
        level->unprocessed = unprocessed;
        level->tickCount = tickCount;

        return level;
    } catch (const exception &ex) {
        cerr << "Failed to load level.dat: " << ex.what() << endl;
        return nullptr;
    }
}

shared_ptr<Level> LevelIO::loadLegacy(gzFile file) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    if (minecraft != nullptr) {
        minecraft->beginLevelLoading("Loading level");
    }
    if (minecraft != nullptr) {
        minecraft->levelLoadUpdate("Reading...");
    }

    try {
        vector<uint8_t> buf(256 * 64 * 256);
        size_t offset = 0;
        const size_t chunkSize = 8192;
        uint8_t tmp[chunkSize];
        int bytes;

        while ((bytes = gzread(file, tmp, chunkSize)) > 0) {
            memcpy(buf.data() + offset, tmp, bytes);
            offset += bytes;
        }
        gzclose(file);

        shared_ptr<Level> level = make_shared<Level>();
        level->setData(256, 64, 256, buf);
        level->name = "--";
        level->creator = "unknown";
        level->createTime = 0;
        return level;
    } catch (const exception& e) {
        cerr << "Failed to load legacy level: " << e.what() << endl;
        return nullptr;
    }
}

bool LevelIO::save(shared_ptr<Level>& level, string minecraftUri, string username, string sessionId, string levelName, int32_t levelId) {
    shared_ptr<Minecraft> minecraft = this->minecraft.lock();
    
    if (minecraft != nullptr) {
        minecraft->beginLevelLoading("Saving level");
        minecraft->levelLoadUpdate("Not implemented");
    }
    this_thread::sleep_for(chrono::milliseconds(3000));
    return false;
}

shared_ptr<Level> LevelIO::load(gzFile file) {
    try {
        string magic = gzip::gzreadString(file);
        if (magic != "MINE") {
            throw runtime_error("Invalid file format");
        }

        int32_t ver;
        gzip::gzreadExact(file, &ver, sizeof(ver));
        if (ver != 2) {
            throw runtime_error("Unsupported version: " + to_string(ver));
        }

        int32_t width = 0, height = 0, depth = 0;
        vector<uint8_t> blocks;
        string name, creator;
        int64_t createTime = 0;
        int32_t xSpawn = 0, ySpawn = 0, zSpawn = 0;
        float rotSpawn = 0.0f;
        int32_t unprocessed = 0, tickCount = 0;
        vector<shared_ptr<Entity>> entities = vector<shared_ptr<Entity>>();
        
        shared_ptr<Level> level = make_shared<Level>();

        while (true) {
            FileFieldHeader header = gzip::gzreadFieldHeader(file);
            
            if (header.type == FileFieldType::END) {
                break;
            }
            
            if (header.name == "levelwidth") {
                width = gzip::gzreadInt32Value(file);
            }
            else if (header.name == "levelheight") {
                height = gzip::gzreadInt32Value(file);
            }
            else if (header.name == "leveldepth") {
                depth = gzip::gzreadInt32Value(file);
            }
            else if (header.name == "levelblocks") {
                blocks = gzip::gzreadByteArrayValue(file);
            }
            else if (header.name == "levelname") {
                name = gzip::gzreadStringValue(file);
            }
            else if (header.name == "levelcreator") {
                creator = gzip::gzreadStringValue(file);
            }
            else if (header.name == "levelcreateTime") {
                createTime = gzip::gzreadInt64Value(file);
            }
            else if (header.name == "levelxSpawn") {
                xSpawn = gzip::gzreadInt32Value(file);
            }
            else if (header.name == "levelySpawn") {
                ySpawn = gzip::gzreadInt32Value(file);
            }
            else if (header.name == "levelzSpawn") {
                zSpawn = gzip::gzreadInt32Value(file);
            }
            else if (header.name == "levelrotSpawn") {
                rotSpawn = gzip::gzreadFloatValue(file);
            }
            else if (header.name == "levelunprocessed") {
                unprocessed = gzip::gzreadInt32Value(file);
            }
            else if (header.name == "leveltickCount") {
                tickCount = gzip::gzreadInt32Value(file);
            }
            else if (header.name == "levelentities") {
                int32_t entityCount = gzip::gzreadInt32Value(file);
                
                for (int32_t i = 0; i < entityCount; ++i) {
                    shared_ptr<Entity> entity = nullptr;

                    while (true) {
                        FileFieldHeader eHeader = gzip::gzreadFieldHeader(file);
                        
                        if (eHeader.type == FileFieldType::END) {
                            break;
                        }

                        if (eHeader.name == "entityType") {
                            string entityType = gzip::gzreadStringValue(file);
                            if (entityType == "Zombie") {
                                entity = make_shared<Zombie>(level, 0, 0, 0);
                            }
                        }
                        else if (!entity) {
                            gzip::gzskipFieldValue(file, eHeader.type);
                        }
                        else {
                            shared_ptr<Zombie> zombie = dynamic_pointer_cast<Zombie>(entity);

                            // zombie
                            if (zombie && eHeader.name == "rot") {
                                zombie->rot = gzip::gzreadFloatValue(file);
                            }
                            else if (zombie && eHeader.name == "speed") {
                                zombie->speed = gzip::gzreadFloatValue(file);
                            }
                            else if (zombie && eHeader.name == "timeOffs") {
                                zombie->timeOffs = gzip::gzreadFloatValue(file);
                            }
                            else if (zombie && eHeader.name == "rotA") {
                                zombie->rotA = gzip::gzreadFloatValue(file);
                            }

                            // entity
                            else if (eHeader.name == "x") {
                                entity->x = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "y") {
                                entity->y = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "z") {
                                entity->z = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "xo") {
                                entity->xo = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "yo") {
                                entity->yo = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "zo") {
                                entity->zo = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "xd") {
                                entity->xd = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "yd") {
                                entity->yd = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "zd") {
                                entity->zd = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "yRot") {
                                entity->yRot = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "xRot") {
                                entity->xRot = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "yRotO") {
                                entity->yRotO = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "xRotO") {
                                entity->xRotO = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "onGround") {
                                entity->onGround = gzip::gzreadBoolValue(file);
                            }
                            else if (eHeader.name == "horizontalCollision") {
                                entity->horizontalCollision = gzip::gzreadBoolValue(file);
                            }
                            else if (eHeader.name == "removed") {
                                entity->removed = gzip::gzreadBoolValue(file);
                            }
                            else if (eHeader.name == "heightOffset") {
                                entity->heightOffset = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "bbWidth") {
                                entity->bbWidth = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "bbHeight") {
                                entity->bbHeight = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "bbminX") {
                                entity->bb.minX = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "bbminY") {
                                entity->bb.minY = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "bbminZ") {
                                entity->bb.minZ = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "bbmaxX") {
                                entity->bb.maxX = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "bbmaxY") {
                                entity->bb.maxY = gzip::gzreadFloatValue(file);
                            }
                            else if (eHeader.name == "bbmaxZ") {
                                entity->bb.maxZ = gzip::gzreadFloatValue(file);
                            }
                            else {
                                gzip::gzskipFieldValue(file, eHeader.type);
                            }
                        }
                    }
                    
                    if (entity) {
                        entities.push_back(entity);
                    }
                }
            }
            else {
                cerr << "Unknown field: " << header.name << " (type: " << (int)header.type << ")" << endl;
                gzip::gzskipFieldValue(file, header.type);
            }
        }

        gzclose(file);

        level->setData(width, depth, height, blocks);
        level->name = name.empty() ? "--" : name;
        level->creator = creator.empty() ? "unknown" : creator;
        level->createTime = createTime;
        level->xSpawn = xSpawn;
        level->ySpawn = ySpawn;
        level->zSpawn = zSpawn;
        level->rotSpawn = rotSpawn;
        level->unprocessed = unprocessed;
        level->tickCount = tickCount;
        level->entities = entities;
        return level;
    } catch (const exception& e) {
        cerr << "Failed to load level: " << e.what() << endl;
        gzclose(file);
        return nullptr;
    }
}

void LevelIO::save(shared_ptr<Level>& level, gzFile file) {
    try {
        for (size_t i = 0; i < level->entities.size(); ++i) {
            auto& e = level->entities[i];
            shared_ptr<Zombie> zombie = dynamic_pointer_cast<Zombie>(e);
        }
        string magic = "MINE";
        
        gzip::gzwriteString(file, magic);

        int32_t ver = 2;
        gzip::gzwriteExact(file, &ver, sizeof(ver));

        gzip::gzwriteFieldInt32(file, "levelwidth", level->width);
        gzip::gzwriteFieldInt32(file, "levelheight", level->height);
        gzip::gzwriteFieldInt32(file, "leveldepth", level->depth);

        gzip::gzwriteFieldByteArray(file, "levelblocks", level->blocks);
        gzip::gzwriteFieldString(file, "levelname", level->name);
        gzip::gzwriteFieldString(file, "levelcreator", level->creator);
        gzip::gzwriteFieldInt64(file, "levelcreateTime", level->createTime);

        gzip::gzwriteFieldInt32(file, "levelxSpawn", level->xSpawn);
        gzip::gzwriteFieldInt32(file, "levelySpawn", level->ySpawn);
        gzip::gzwriteFieldInt32(file, "levelzSpawn", level->zSpawn);
        gzip::gzwriteFieldFloat(file, "levelrotSpawn", level->rotSpawn);

        gzip::gzwriteFieldInt32(file, "levelunprocessed", level->unprocessed);
        gzip::gzwriteFieldInt32(file, "leveltickCount", level->tickCount);

        // entities
        gzip::gzwriteFieldInt32(file, "levelentities", level->entities.size());

        for (auto& e : level->entities) {
            shared_ptr<Zombie> zombie = dynamic_pointer_cast<Zombie>(e);
            if (zombie) {
                gzip::gzwriteFieldString(file, "entityType", "Zombie");
                gzip::gzwriteFieldFloat(file, "rot", zombie->rot);
                gzip::gzwriteFieldFloat(file, "speed", zombie->speed);
                gzip::gzwriteFieldFloat(file, "timeOffs", zombie->timeOffs);
                gzip::gzwriteFieldFloat(file, "rotA", zombie->rotA);
            }
            else {
                gzip::gzwriteFieldEnd(file);
                continue;
            }

            gzip::gzwriteFieldFloat(file, "x", e->x);
            gzip::gzwriteFieldFloat(file, "y", e->y);
            gzip::gzwriteFieldFloat(file, "z", e->z);
            gzip::gzwriteFieldFloat(file, "xo", e->xo);
            gzip::gzwriteFieldFloat(file, "yo", e->yo);
            gzip::gzwriteFieldFloat(file, "zo", e->zo);
            gzip::gzwriteFieldFloat(file, "xd", e->xd);
            gzip::gzwriteFieldFloat(file, "yd", e->yd);
            gzip::gzwriteFieldFloat(file, "zd", e->zd);
            gzip::gzwriteFieldFloat(file, "yRot", e->yRot);
            gzip::gzwriteFieldFloat(file, "xRot", e->xRot);
            gzip::gzwriteFieldFloat(file, "yRotO", e->yRotO);
            gzip::gzwriteFieldFloat(file, "xRotO", e->xRotO);

            gzip::gzwriteFieldBool(file, "onGround", e->onGround);
            gzip::gzwriteFieldBool(file, "horizontalCollision", e->horizontalCollision);
            gzip::gzwriteFieldBool(file, "removed", e->removed);

            gzip::gzwriteFieldFloat(file, "heightOffset", e->heightOffset);
            gzip::gzwriteFieldFloat(file, "bbWidth", e->bbWidth);
            gzip::gzwriteFieldFloat(file, "bbHeight", e->bbHeight);

            // AABB
            gzip::gzwriteFieldFloat(file, "bbminX", e->bb.minX);
            gzip::gzwriteFieldFloat(file, "bbminY", e->bb.minY);
            gzip::gzwriteFieldFloat(file, "bbminZ", e->bb.minZ);
            gzip::gzwriteFieldFloat(file, "bbmaxX", e->bb.maxX);
            gzip::gzwriteFieldFloat(file, "bbmaxY", e->bb.maxY);
            gzip::gzwriteFieldFloat(file, "bbmaxZ", e->bb.maxZ);
            gzip::gzwriteFieldEnd(file);
        }
        gzip::gzwriteFieldEnd(file);
    } catch (const exception& e) {
        cerr << "Failed to save level: " << e.what() << endl;
    }
    gzclose(file);
}

vector<uint8_t> LevelIO::loadBlocks(gzFile file) {
    vector<uint8_t> blocks;

    unsigned char lenBuf[4];
    gzip::gzreadExact(file, lenBuf, sizeof(lenBuf));

    uint32_t length = ((uint32_t)lenBuf[0] << 24) |
                      ((uint32_t)lenBuf[1] << 16) |
                      ((uint32_t)lenBuf[2] <<  8) |
                      ((uint32_t)lenBuf[3] <<  0);

    blocks.resize(length);

    uint32_t offset = 0;
    while (offset < length) {
        int toRead = (int)min<uint32_t>(length - offset, 65536);
        int got = gzread(file, blocks.data() + offset, toRead);
        if (got <= 0) {
            int errnum = 0;
            const char* err = gzerror(file, &errnum);
            cerr << "loadBlocks: gzread failed at offset " << offset << " (got " << got << "): "
                 << (err ? err : "unknown") << endl;
            gzclose(file);
            blocks.clear();
            return blocks;
        }
        offset += (uint32_t)got;
    }

    gzclose(file);
    return blocks;
}

vector<uint8_t> LevelIO::loadBlocks(vector<uint8_t>& in) {
    const char* tempFilename = "temp_blocks.gz";
    FILE* tempFile = fopen(tempFilename, "wb");
    if (!tempFile) {
        throw runtime_error("Failed to create temporary file");
    }
    
    fwrite(in.data(), 1, in.size(), tempFile);
    fclose(tempFile);
    
    gzFile file = gzopen(tempFilename, "rb");
    if (!file) {
        remove(tempFilename);
        throw runtime_error("Failed to open temporary gzip file");
    }
    
    unsigned char lenBuf[4];
    if (gzread(file, lenBuf, sizeof(lenBuf)) != sizeof(lenBuf)) {
        gzclose(file);
        remove(tempFilename);
        throw runtime_error("Failed to read length header");
    }
    
    uint32_t length = ((uint32_t)lenBuf[0] << 24) |
                      ((uint32_t)lenBuf[1] << 16) |
                      ((uint32_t)lenBuf[2] << 8) |
                      ((uint32_t)lenBuf[3] << 0);
    
    vector<uint8_t> blocks;
    blocks.resize(length);
    
    uint32_t offset = 0;
    while (offset < length) {
        int toRead = (int)min<uint32_t>(length - offset, 65536);
        int got = gzread(file, blocks.data() + offset, toRead);
        if (got <= 0) {
            gzclose(file);
            remove(tempFilename);
            throw runtime_error("gzread failed during block reading");
        }
        offset += (uint32_t)got;
    }
    
    gzclose(file);
    remove(tempFilename);
    
    return blocks;
}