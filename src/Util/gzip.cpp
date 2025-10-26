#include <iostream>
#include "Util/gzip.h"

void gzip::gzreadExact(gzFile file, void* buf, unsigned int len) {
    if (len == 0) {
        return;
    }

    int r = gzread(file, buf, len);
    if (r != (int)len) {
        throw runtime_error("gzread error");
    }
}

void gzip::gzwriteExact(gzFile file, const void* buf, unsigned int len) {
    if (len == 0) {
        return;
    }

    int w = gzwrite(file, buf, len);
    if (w != (int)len) {
        throw runtime_error("gzwrite failed");
    }
}

string gzip::gzreadString(gzFile file) {
    int32_t len;
    gzreadExact(file, &len, sizeof(len));
    if (len < 0 || len > 1000000) {
        throw runtime_error("Invalid string length: " + to_string(len));
    }
    if (len == 0) return "";
    string str;
    str.resize(len);
    gzreadExact(file, str.data(), len);
    return str;
}

void gzip::gzwriteString(gzFile file, const string& str) {
    int32_t len = str.size();
    gzwriteExact(file, &len, sizeof(len));
    if (len > 0) {
        gzwriteExact(file, str.data(), len);
    }
}

FileFieldHeader gzip::gzreadFieldHeader(gzFile file) {
    FileFieldHeader header;
    uint8_t firstByte;
    int r = gzread(file, &firstByte, 1);
    
    if (r == 0) {
        header.type = FileFieldType::END;
        header.name = "";
        return header;
    }
    
    if (r != 1) {
        throw runtime_error("Failed to read field header");
    }
    
    if (firstByte == (uint8_t)FileFieldType::END) {
        header.type = FileFieldType::END;
        header.name = "";
        return header;
    }

    uint8_t secondByte;
    gzreadExact(file, &secondByte, 1);

    int16_t nameLen = firstByte | (secondByte << 8);
    
    if (nameLen <= 0 || nameLen > 1000) {
        throw runtime_error("Invalid field name length: " + to_string(nameLen));
    }

    header.name.resize(nameLen);
    gzreadExact(file, header.name.data(), nameLen);

    uint8_t typeTag;
    gzreadExact(file, &typeTag, 1);
    header.type = (FileFieldType)typeTag;
    
    return header;
}

int32_t gzip::gzreadInt32Value(gzFile file) {
    int32_t value;
    gzreadExact(file, &value, sizeof(value));
    return value;
}

int64_t gzip::gzreadInt64Value(gzFile file) {
    int64_t value;
    gzreadExact(file, &value, sizeof(value));
    return value;
}

float gzip::gzreadFloatValue(gzFile file) {
    float value;
    gzreadExact(file, &value, sizeof(value));
    return value;
}

string gzip::gzreadStringValue(gzFile file) {
    int32_t len;
    gzreadExact(file, &len, sizeof(len));
    string value;
    value.resize(len);
    gzreadExact(file, value.data(), len);
    return value;
}

bool gzip::gzreadBoolValue(gzFile file) {
    bool value;
    gzreadExact(file, &value, sizeof(value));
    return value;
}

vector<uint8_t> gzip::gzreadByteArrayValue(gzFile file) {
    int32_t len;
    gzreadExact(file, &len, sizeof(len));
    vector<uint8_t> value;
    value.resize(len);
    gzreadExact(file, value.data(), len);
    return value;
}

void gzip::gzwriteFieldInt32(gzFile file, const string& name, int32_t value) {
    int16_t nameLen = name.size();
    gzwriteExact(file, &nameLen, sizeof(nameLen));
    gzwriteExact(file, name.data(), nameLen);
    uint8_t typeTag = (uint8_t)FileFieldType::INT32;
    gzwriteExact(file, &typeTag, sizeof(typeTag));
    gzwriteExact(file, &value, sizeof(value));
}

void gzip::gzwriteFieldInt64(gzFile file, const string& name, int64_t value) {
    int16_t nameLen = name.size();
    gzwriteExact(file, &nameLen, sizeof(nameLen));
    gzwriteExact(file, name.data(), nameLen);
    uint8_t typeTag = (uint8_t)FileFieldType::INT64;
    gzwriteExact(file, &typeTag, sizeof(typeTag));
    gzwriteExact(file, &value, sizeof(value));
}

void gzip::gzwriteFieldFloat(gzFile file, const string& name, float value) {
    int16_t nameLen = name.size();
    gzwriteExact(file, &nameLen, sizeof(nameLen));
    gzwriteExact(file, name.data(), nameLen);
    uint8_t typeTag = (uint8_t)FileFieldType::FLOAT;
    gzwriteExact(file, &typeTag, sizeof(typeTag));
    gzwriteExact(file, &value, sizeof(value));
}

void gzip::gzwriteFieldString(gzFile file, const string& name, const string& value) {
    int16_t nameLen = name.size();
    gzwriteExact(file, &nameLen, sizeof(nameLen));
    gzwriteExact(file, name.data(), nameLen);
    uint8_t typeTag = (uint8_t)FileFieldType::STRING;
    gzwriteExact(file, &typeTag, sizeof(typeTag));
    int32_t valueLen = value.size();
    gzwriteExact(file, &valueLen, sizeof(valueLen));
    gzwriteExact(file, value.data(), valueLen);
}

void gzip::gzwriteFieldBool(gzFile file, const string& name, bool value) {
    int16_t nameLen = name.size();
    gzwriteExact(file, &nameLen, sizeof(nameLen));
    gzwriteExact(file, name.data(), nameLen);
    uint8_t typeTag = (uint8_t)FileFieldType::BOOL;
    gzwriteExact(file, &typeTag, sizeof(typeTag));
    gzwriteExact(file, &value, sizeof(value));
}

void gzip::gzwriteFieldByteArray(gzFile file, const string& name, const vector<uint8_t>& value) {
    int16_t nameLen = name.size();
    gzwriteExact(file, &nameLen, sizeof(nameLen));
    gzwriteExact(file, name.data(), nameLen);
    uint8_t typeTag = (uint8_t)FileFieldType::BYTE_ARRAY;
    gzwriteExact(file, &typeTag, sizeof(typeTag));
    int32_t arrayLen = value.size();
    gzwriteExact(file, &arrayLen, sizeof(arrayLen));
    gzwriteExact(file, value.data(), arrayLen);
}

void gzip::gzwriteFieldEnd(gzFile file) {
    uint8_t endMarker = (uint8_t)FileFieldType::END;
    gzwriteExact(file, &endMarker, sizeof(endMarker));
}

void gzip::gzskipFieldValue(gzFile file, FileFieldType type) {
    switch (type) {
        case FileFieldType::INT32: {
            int32_t dummy;
            gzreadExact(file, &dummy, sizeof(dummy));
            break;
        }
        case FileFieldType::INT64: {
            int64_t dummy;
            gzreadExact(file, &dummy, sizeof(dummy));
            break;
        }
        case FileFieldType::FLOAT: {
            float dummy;
            gzreadExact(file, &dummy, sizeof(dummy));
            break;
        }
        case FileFieldType::BOOL: {
            bool dummy;
            gzreadExact(file, &dummy, sizeof(dummy));
            break;
        }
        case FileFieldType::STRING: {
            int32_t len;
            gzreadExact(file, &len, sizeof(len));
            if (len > 0) {
                vector<char> dummy(len);
                gzreadExact(file, dummy.data(), len);
            }
            break;
        }
        case FileFieldType::BYTE_ARRAY: {
            int32_t len;
            gzreadExact(file, &len, sizeof(len));
            if (len > 0) {
                vector<uint8_t> dummy(len);
                gzreadExact(file, dummy.data(), len);
            }
            break;
        }
        case FileFieldType::END: {
            break;
        }
        default:
            cerr << "Warning: Unknown field type: " << (int)type << " - skipping may fail" << endl;
            throw runtime_error("Cannot skip unknown field type: " + to_string((int)type));
    }
}