#include "player/Player.h"
#include "renderer/Chunk.h"
#include "renderer/Frustum.h"
#include "Timer.h"

class DirtyChunkSorter {
    shared_ptr<Player> player;
public:
    DirtyChunkSorter(shared_ptr<Player>& player);

    bool operator()(Chunk* c0, Chunk* c1);
};