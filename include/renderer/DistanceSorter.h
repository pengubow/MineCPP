#include "player/Player.h"
#include "renderer/Chunk.h"
#include "renderer/Frustum.h"
#include "Timer.h"

class DistanceSorter {
private:
    shared_ptr<Player> player;
public:
    DistanceSorter(shared_ptr<Player>& player);

    bool operator()(shared_ptr<Chunk>& c0, shared_ptr<Chunk>& c1);
};