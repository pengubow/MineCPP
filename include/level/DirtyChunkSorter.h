#include "Player.h"
#include "level/Chunk.h"
#include "level/Frustum.h"
#include "Timer.h"

class DirtyChunkSorter {
private:
    std::shared_ptr<Player> player;
    Frustum& frustum;
    int64_t now = Timer::nanoTime() / 1000000;
public:

    DirtyChunkSorter(std::shared_ptr<Player> player, Frustum& frustum);

    bool operator()(Chunk* c0, Chunk* c1);
};