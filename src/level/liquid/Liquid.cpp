#include "level/liquid/Liquid.h"

vector<Liquid*> Liquid::liquids = vector<Liquid*>(4);

Liquid* Liquid::none = new Liquid(0);
Liquid* Liquid::water = new Liquid(1);
Liquid* Liquid::lava = new Liquid(2);

Liquid::Liquid(int32_t id) {
    liquids[id] = this;
}