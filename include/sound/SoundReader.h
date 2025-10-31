#pragma once

#include <memory>
#include <string>
#include "sound/SoundData.h"

class SoundReader {
public:
    static shared_ptr<SoundData> read(string var0);
};