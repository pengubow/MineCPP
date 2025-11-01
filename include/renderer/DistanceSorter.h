#include "player/Player.h"
#include "renderer/Chunk.h"
#include "renderer/Frustum.h"
#include "Timer.h"

class DistanceSorter {
    shared_ptr<Player> player;
public:
    DistanceSorter(shared_ptr<Player>& player);

    bool operator()(Chunk* c0, Chunk* c1);
};