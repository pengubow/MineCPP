#include "player/Player.h"
#include "renderer/Chunk.h"
#include "renderer/Frustum.h"
#include "Timer.h"

class DirtyChunkSorter {
    shared_ptr<Player> player;
public:
    DirtyChunkSorter(shared_ptr<Player>& player);

    bool operator()(shared_ptr<Chunk>& c0, shared_ptr<Chunk>& c1);
};