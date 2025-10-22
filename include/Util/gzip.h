#pragma once

#include <stdint.h>
#include <string>
#include <zlib.h>
#include <vector>

using namespace std;

enum class FileFieldType : uint8_t {
    INT32 = 0x01,
    INT64 = 0x02,
    FLOAT = 0x03,
    STRING = 0x04,
    BYTE_ARRAY = 0x05,
    BOOL = 0x06,
    ENTITY_ARRAY = 0x07,
    END = 0xFF
};

struct FileFieldHeader {
    string name;
    FileFieldType type;
};

namespace gzip {
    void gzreadExact(gzFile file, void* buf, unsigned int len);
    void gzwriteExact(gzFile file, const void* buf, unsigned int len);
    string gzreadString(gzFile file);
    void gzwriteString(gzFile file, const string& str);

    FileFieldHeader gzreadFieldHeader(gzFile file);
    int32_t gzreadInt32Value(gzFile file);
    int64_t gzreadInt64Value(gzFile file);
    float gzreadFloatValue(gzFile file);
    string gzreadStringValue(gzFile file);
    bool gzreadBoolValue(gzFile file);
    vector<uint8_t> gzreadByteArrayValue(gzFile file);
    

    void gzwriteFieldInt32(gzFile file, const string& name, int32_t value);
    void gzwriteFieldInt64(gzFile file, const string& name, int64_t value);
    void gzwriteFieldFloat(gzFile file, const string& name, float value);
    void gzwriteFieldString(gzFile file, const string& name, const string& value);
    void gzwriteFieldBool(gzFile file, const string& name, bool value);
    void gzwriteFieldByteArray(gzFile file, const string& name, const vector<uint8_t>& value);
    void gzwriteFieldEnd(gzFile file);

    void gzskipFieldValue(gzFile file, FileFieldType type);
}