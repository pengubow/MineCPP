#include <zlib.h>
#include <cstring>
#include <functional>
#include "level/LevelIO.h"
#include "Minecraft.h"

using namespace std;

LevelIO::LevelIO(shared_ptr<Minecraft>& minecraft)
    : minecraft(minecraft) {}

shared_ptr<Level> LevelIO::load(gzFile file) {
    try {
        int32_t magicLength;
        Util::gzreadExact(file, &magicLength, sizeof(magicLength));
        string magic;
        magic.resize(magicLength);
        Util::gzreadExact(file, magic.data(), magicLength);
        if (magic != "MINE") {
            throw runtime_error("Invalid file format");
        }

        int32_t ver;
        Util::gzreadExact(file, &ver, sizeof(ver));
        if (ver != 2) {
            throw runtime_error("Invalid file format or invalid version");
        }

        int32_t width;
        int32_t height;
        int32_t depth;
        
        Util::gzreadExact(file, &width, sizeof(width));
        Util::gzreadExact(file, &height, sizeof(height));
        Util::gzreadExact(file, &depth, sizeof(depth));
        
        int32_t blocksLength;
        Util::gzreadExact(file, &blocksLength, sizeof(blocksLength));
        vector<uint8_t> blocks(blocksLength);
        Util::gzreadExact(file, blocks.data(), blocksLength);

        int32_t nameLength;
        Util::gzreadExact(file, &nameLength, sizeof(nameLength));
        string name;
        name.resize(nameLength);
        Util::gzreadExact(file, name.data(), nameLength);

        int32_t creatorLength;
        Util::gzreadExact(file, &creatorLength, sizeof(creatorLength));
        string creator;
        creator.resize(creatorLength);
        Util::gzreadExact(file, creator.data(), creatorLength);

        int64_t createTime;
        int32_t xSpawn;
        int32_t ySpawn;
        int32_t zSpawn;
        float rotSpawn;
        Util::gzreadExact(file, &createTime, sizeof(createTime));
        Util::gzreadExact(file, &xSpawn, sizeof(xSpawn));
        Util::gzreadExact(file, &ySpawn, sizeof(ySpawn));
        Util::gzreadExact(file, &zSpawn, sizeof(zSpawn));
        Util::gzreadExact(file, &rotSpawn, sizeof(rotSpawn));

        shared_ptr<Level> level = make_shared<Level>();

        // entities
        int32_t entitiesLength;
        vector<shared_ptr<Entity>> entities;
        Util::gzreadExact(file, &entitiesLength, sizeof(entitiesLength));
        for (int32_t i = 0; i < entitiesLength; ++i) {
            // zombie
            // we acc dont know if the entity is a zombie but in this version no other entities exist so we fine
            float x;
            float y;
            float z;
            Util::gzreadExact(file, &x, sizeof(x));
            Util::gzreadExact(file, &y, sizeof(y));
            Util::gzreadExact(file, &z, sizeof(z));
            auto e = make_shared<Zombie>(level, x, y, z);

            Util::gzreadExact(file, &e->rot, sizeof(e->rot));
            Util::gzreadExact(file, &e->timeOffs, sizeof(e->timeOffs));
            Util::gzreadExact(file, &e->speed, sizeof(e->speed));
            Util::gzreadExact(file, &e->rotA, sizeof(e->rotA));

            // entity
            Util::gzreadExact(file, &e->xo, sizeof(e->xo));
            Util::gzreadExact(file, &e->yo, sizeof(e->yo));
            Util::gzreadExact(file, &e->zo, sizeof(e->zo));
            Util::gzreadExact(file, &e->xd, sizeof(e->xd));
            Util::gzreadExact(file, &e->yd, sizeof(e->yd));
            Util::gzreadExact(file, &e->zd, sizeof(e->zd));
            Util::gzreadExact(file, &e->yRot, sizeof(e->yRot));
            Util::gzreadExact(file, &e->xRot, sizeof(e->xRot));
            Util::gzreadExact(file, &e->yRotI, sizeof(e->yRotI));
            Util::gzreadExact(file, &e->xRotI, sizeof(e->xRotI));
            Util::gzreadExact(file, &e->onGround, sizeof(e->onGround));
            Util::gzreadExact(file, &e->horizontalCollision, sizeof(e->horizontalCollision));
            Util::gzreadExact(file, &e->removed, sizeof(e->removed));
            Util::gzreadExact(file, &e->heightOffset, sizeof(e->heightOffset));
            Util::gzreadExact(file, &e->bbWidth, sizeof(e->bbWidth));
            Util::gzreadExact(file, &e->bbHeight, sizeof(e->bbHeight));

            // AABB
            float minX, minY, minZ, maxX, maxY, maxZ;
            Util::gzreadExact(file, &minX, sizeof(minX));
            Util::gzreadExact(file, &minY, sizeof(minY));
            Util::gzreadExact(file, &minZ, sizeof(minZ));
            Util::gzreadExact(file, &maxX, sizeof(maxX));
            Util::gzreadExact(file, &maxY, sizeof(maxY));
            Util::gzreadExact(file, &maxZ, sizeof(maxZ));
            e->bb = make_shared<AABB>(minX, minY, minZ, maxX, maxY, maxZ);

            entities.push_back(e);
        }

        int32_t unprocessed;
        int32_t tickCount;
        Util::gzreadExact(file, &unprocessed, sizeof(unprocessed));
        Util::gzreadExact(file, &tickCount, sizeof(tickCount));

        gzclose(file);

        
        level->setData(width, depth, height, blocks);
        level->name = name;
        level->creator = creator;
        level->createTime = createTime;
        level->xSpawn = xSpawn;
        level->ySpawn = ySpawn;
        level->zSpawn = zSpawn;
        level->rotSpawn = rotSpawn;
        level->entities = entities;
        level->unprocessed = unprocessed;
        level->tickCount = tickCount;
        return level;
    } catch (const exception& e) {
        cerr << "Failed to load level.mcpp: " << e.what() << endl;
    }
    gzclose(file);
    return nullptr;
}

shared_ptr<Level> LevelIO::loadDat(gzFile file) {
    if (this->minecraft != nullptr) {
        this->minecraft->beginLevelLoading("Loading level");
    }
    if (this->minecraft != nullptr) {
        this->minecraft->levelLoadUpdate("Reading..");
    }

    // ts was chatgptd
    // ^^^^^^^^^^^^^^^
    try {
        const size_t CHUNK = 8192;
        std::vector<uint8_t> buf;
        buf.reserve(1 << 20);
        uint8_t tmp[CHUNK];
        int r;
        while ((r = gzread(file, tmp, CHUNK)) > 0) {
            buf.insert(buf.end(), tmp, tmp + r);
        }

        gzclose(file);

        size_t pos = 0;
        auto ensure = [&](size_t n) {
            if (pos + n > buf.size()) throw std::runtime_error("Unexpected end of buffer");
        };
        auto readByte = [&]() -> uint8_t { ensure(1); return buf[pos++]; };
        auto readBytes = [&](void* out, size_t n) {
            if (n == 0) return;
            ensure(n);
            memcpy(out, buf.data() + pos, n);
            pos += n;
        };
        auto readInt16BE = [&]() -> int16_t { ensure(2); int16_t v = (int16_t)((buf[pos] << 8) | buf[pos+1]); pos += 2; return v; };
        auto readInt32BE = [&]() -> int32_t {
            ensure(4);
            uint32_t v = (uint32_t(buf[pos]) << 24) | (uint32_t(buf[pos+1]) << 16) | (uint32_t(buf[pos+2]) << 8) | uint32_t(buf[pos+3]);
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
        auto readStringBE = [&]() -> std::string {
            int16_t len = readInt32BE() >> 16;
            pos -= 4;
            len = readInt16BE();
            if (len < 0) throw std::runtime_error("Negative string length");
            if (len == 0) return std::string();
            ensure((size_t)len);
            std::string s;
            s.resize((size_t)len);
            memcpy(&s[0], buf.data() + pos, (size_t)len);
            pos += (size_t)len;
            return s;
        };

        auto skip = [&](size_t n) { ensure(n); pos += n; };
        auto parseError = [&](int expected, int got) {
            char tmpbuf[128];
            snprintf(tmpbuf, sizeof(tmpbuf), "Expected %02X, got %02X", expected, got);
            throw std::runtime_error(tmpbuf);
        };

        const uint8_t TC_NULL = 0x70, TC_REFERENCE = 0x71;
        const uint8_t TC_CLASSDESC = 0x72, TC_OBJECT = 0x73;
        const uint8_t TC_STRING = 0x74, TC_ARRAY = 0x75;
        const uint8_t TC_ENDBLOCKDATA = 0x78;

        std::function<std::vector<std::tuple<char,std::string,std::string>>()> readClassDesc;
        readClassDesc = [&]() -> std::vector<std::tuple<char,std::string,std::string>> {
            std::vector<std::tuple<char,std::string,std::string>> fields;
            uint8_t tc = readByte();
            if (tc == TC_NULL) return fields;
            if (tc != TC_CLASSDESC) parseError(TC_CLASSDESC, tc);

            std::string className = readStringBE();
            readInt64BE();
            readByte();
            int16_t fcount = readInt16BE();
            if (fcount < 0) throw std::runtime_error("Negative field count");
            for (int i = 0; i < fcount; ++i) {
                char ftype = (char)readByte();
                std::string fname = readStringBE();
                std::string class1;
                if (ftype == '[' || ftype == 'L') {
                    uint8_t t = readByte();
                    if (t == TC_STRING) {
                        class1 = readStringBE();
                    } else if (t == TC_REFERENCE) {
                        readInt32BE();
                    } else parseError(TC_STRING, t);
                }
                fields.emplace_back(ftype, fname, class1);
            }
            uint8_t end = readByte();
            if (end != TC_ENDBLOCKDATA) parseError(TC_ENDBLOCKDATA, end);

            readClassDesc();
            return fields;
        };

        int32_t magic = readInt32BE();
        uint8_t versionByte = readByte();
        if (magic != 0x271BB788 || versionByte != 0x2) {
            throw std::runtime_error("Unexpected constant in .dat file");
        }
        int16_t ac = readInt16BE();
        int16_t ver = readInt16BE();
        if ((uint16_t)ac != 0xACED || ver != 0x0005) {
            throw std::runtime_error("Unexpected java serialisation constant(s).");
        }
        uint8_t topcode = readByte();
        if (topcode != TC_OBJECT) parseError(TC_OBJECT, topcode);

        auto topFields = readClassDesc();

        auto readArrayForField = [&](const std::string& fname) -> std::vector<uint8_t> {
            uint8_t t = readByte();
            if (t == TC_NULL) return std::vector<uint8_t>();
            if (t != TC_ARRAY) parseError(TC_ARRAY, t);
            readClassDesc();
            int32_t len = readInt32BE();
            if (len < 0) throw std::runtime_error("Negative array length");
            std::vector<uint8_t> out;
            if (len > 0) {
                out.resize((size_t)len);
                readBytes(out.data(), out.size());
            }
            return out;
        };

        int width = 0, depth = 0, height = 0;
        int xSpawn = 0, ySpawn = 0, zSpawn = 0;
        std::vector<uint8_t> blocks;

        for (size_t i = 0; i < topFields.size(); ++i) {
            char ftype = std::get<0>(topFields[i]);
            std::string fname = std::get<1>(topFields[i]);

            switch (ftype) {
                case 'B': {
                    (void)readByte();
                    break;
                }
                case 'F': {
                    int32_t v = readInt32BE();
                    if (fname == "width") width = v;
                    else if (fname == "height") height = v;
                    else if (fname == "depth") depth = v;
                    break;
                }
                case 'I': {
                    int32_t v = readInt32BE();
                    if (fname == "width") width = v;
                    else if (fname == "height") height = v;
                    else if (fname == "depth") depth = v;
                    else if (fname == "xSpawn") xSpawn = v;
                    else if (fname == "ySpawn") ySpawn = v;
                    else if (fname == "zSpawn") zSpawn = v;
                    break;
                }
                case 'J': {
                    (void)readInt64BE();
                    break;
                }
                case 'Z': {
                    (void)readByte();
                    break;
                }
                case 'L': {
                    if (fname == "blockMap") {
                        uint8_t tc = readByte();
                        if (tc == TC_OBJECT) {
                            skip(315);
                            int32_t count = readInt32BE();
                            if (count < 0) throw std::runtime_error("Negative blockMap count");
                            for (int j = 0; j < count; ++j) skip(17);
                            skip(152);
                        } else {
                            if (tc == TC_NULL) {}
                            else if (tc == TC_STRING) { (void)readStringBE(); }
                            else if (tc == TC_REFERENCE) { (void)readInt32BE(); }
                            else {
                                if (tc == TC_CLASSDESC) {
                                    pos--;
                                    readClassDesc();
                                }
                            }
                        }
                    } else {
                        uint8_t tc = readByte();
                        if (tc == TC_NULL) {}
                        else if (tc == TC_STRING) { (void)readStringBE(); }
                        else if (tc == TC_REFERENCE) { (void)readInt32BE(); }
                        else if (tc == TC_CLASSDESC) { pos--; readClassDesc(); }
                        else if (tc == TC_OBJECT) {}
                    }
                    break;
                }
                case '[': {
                    std::vector<uint8_t> arr = readArrayForField(fname);
                    if (fname == "blocks") {
                        blocks = std::move(arr);
                    }
                    break;
                }
                default:
                    break;
            }
        }

        auto level = std::make_shared<Level>();

        const size_t TILE_ARRAY_SIZE = 256;

        for (size_t i = 0; i < blocks.size(); ++i) {
            uint8_t id = blocks[i];
            bool invalid = false;

            if (id >= TILE_ARRAY_SIZE) invalid = true;
            else {
                if (Tile::tiles[id] == nullptr) invalid = true;
            }

            if (invalid) {
                blocks[i] = 0;
            }
        }

        level->setData(width, depth, height, blocks);
        level->name = "--";
        level->creator = "unknown";
        level->createTime = 0;

        level->xSpawn = xSpawn;
        level->ySpawn = ySpawn;
        level->zSpawn = zSpawn;

        return level;
    } catch (const std::exception &ex) {
        std::cerr << "Failed to load level.dat: " << ex.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<Level> LevelIO::loadLegacy(gzFile file) {
    try {
        vector<uint8_t> buf(256 * 64 * 256);
        size_t offset = 0;
        const size_t chunkSize = 8192;
        uint8_t tmp[chunkSize];
        int bytes;

        while ((bytes = gzwrite(file, tmp, chunkSize)) > 0) {
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

void LevelIO::save(shared_ptr<Level>& level, gzFile file) {
    try {
        string magic = "MINE";
        int32_t length = magic.size();
        Util::gzwriteExact(file, &length, sizeof(length));
        Util::gzwriteExact(file, magic.data(), length);

        int32_t ver = 2;
        Util::gzwriteExact(file, &ver, sizeof(ver));

        Util::gzwriteExact(file, &level->width, sizeof(level->width));
        Util::gzwriteExact(file, &level->height, sizeof(level->height));
        Util::gzwriteExact(file, &level->depth, sizeof(level->depth));

        length = level->blocks.size();
        Util::gzwriteExact(file, &length, sizeof(length));
        Util::gzwriteExact(file, level->blocks.data(), length);
        

        length = level->name.size();
        Util::gzwriteExact(file, &length, sizeof(length));
        Util::gzwriteExact(file, level->name.data(), length);

        length = level->creator.size();
        Util::gzwriteExact(file, &length, sizeof(length));
        Util::gzwriteExact(file, level->creator.data(), length);
        
        Util::gzwriteExact(file, &level->createTime, sizeof(level->createTime));
        Util::gzwriteExact(file, &level->xSpawn, sizeof(level->xSpawn));
        Util::gzwriteExact(file, &level->ySpawn, sizeof(level->ySpawn));
        Util::gzwriteExact(file, &level->zSpawn, sizeof(level->zSpawn));
        Util::gzwriteExact(file, &level->rotSpawn, sizeof(level->rotSpawn));

        // entities
        length = level->entities.size();
        Util::gzwriteExact(file, &length, sizeof(length));
        for (auto& e : level->entities) {
            // zombie
            shared_ptr<Zombie> zombie = dynamic_pointer_cast<Zombie>(e);
            if (zombie) {
                float x;
                float y;
                float z;
                Util::gzwriteExact(file, &zombie->x, sizeof(zombie->x));
                Util::gzwriteExact(file, &zombie->y, sizeof(zombie->y));
                Util::gzwriteExact(file, &zombie->z, sizeof(zombie->z));

                float rot;
                float timeOffs;
                float speed;
                float rotA;
                Util::gzwriteExact(file, &zombie->rot, sizeof(zombie->rot));
                Util::gzwriteExact(file, &zombie->timeOffs, sizeof(zombie->timeOffs));
                Util::gzwriteExact(file, &zombie->speed, sizeof(zombie->speed));
                Util::gzwriteExact(file, &zombie->rotA, sizeof(zombie->rotA));
            }
            else {
                Util::gzwriteExact(file, &e->x, sizeof(e->x));
                Util::gzwriteExact(file, &e->y, sizeof(e->y));
                Util::gzwriteExact(file, &e->z, sizeof(e->z));
            }
            // entity
            Util::gzwriteExact(file, &e->xo, sizeof(e->xo));
            Util::gzwriteExact(file, &e->yo, sizeof(e->yo));
            Util::gzwriteExact(file, &e->zo, sizeof(e->zo));
            Util::gzwriteExact(file, &e->xd, sizeof(e->xd));
            Util::gzwriteExact(file, &e->yd, sizeof(e->yd));
            Util::gzwriteExact(file, &e->zd, sizeof(e->zd));
            Util::gzwriteExact(file, &e->yRot, sizeof(e->yRot));
            Util::gzwriteExact(file, &e->xRot, sizeof(e->xRot));
            Util::gzwriteExact(file, &e->yRotI, sizeof(e->yRotI));
            Util::gzwriteExact(file, &e->xRotI, sizeof(e->xRotI));
            Util::gzwriteExact(file, &e->onGround, sizeof(e->onGround));
            Util::gzwriteExact(file, &e->horizontalCollision, sizeof(e->horizontalCollision));
            Util::gzwriteExact(file, &e->removed, sizeof(e->removed));
            Util::gzwriteExact(file, &e->heightOffset, sizeof(e->heightOffset));
            Util::gzwriteExact(file, &e->bbWidth, sizeof(e->bbWidth));
            Util::gzwriteExact(file, &e->bbHeight, sizeof(e->bbHeight));

            // AABB
            Util::gzwriteExact(file, &e->bb->minX, sizeof(float));
            Util::gzwriteExact(file, &e->bb->minY, sizeof(float));
            Util::gzwriteExact(file, &e->bb->minZ, sizeof(float));
            Util::gzwriteExact(file, &e->bb->maxX, sizeof(float));
            Util::gzwriteExact(file, &e->bb->maxY, sizeof(float));
            Util::gzwriteExact(file, &e->bb->maxZ, sizeof(float));
        }

        Util::gzwriteExact(file, &level->unprocessed, sizeof(level->unprocessed));
        Util::gzwriteExact(file, &level->tickCount, sizeof(level->tickCount));

    } catch (const exception& e) {
        cerr << "Failed to save level: " << e.what() << endl;
    }
    gzclose(file);
    
}