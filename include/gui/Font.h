#include <vector>
#include <memory>
#include "renderer/Textures.h"

class Font {
private:
    vector<int32_t> charWidths = vector<int32_t>(256);
    int32_t fontTexture = 0;
public:
    Font(string name, shared_ptr<Textures>& textures);

    void drawShadow(string str, int32_t x, int32_t y, int32_t color);
    void draw(string str, int32_t x, int32_t y, int32_t color);
    void draw(string str, int32_t x, int32_t y, int32_t color, bool darken);
    int32_t width(string str);
};