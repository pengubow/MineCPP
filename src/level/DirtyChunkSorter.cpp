#include "level/DirtyChunkSorter.h"

DirtyChunkSorter::DirtyChunkSorter(std::shared_ptr<Player> player, Frustum& frustum) 
    : player(player), frustum(frustum) {}

bool DirtyChunkSorter::operator()(Chunk* c0, Chunk* c1) {
    bool i0 = this->frustum.isVisible(c0->aabb);
    bool i1 = this->frustum.isVisible(c1->aabb);
    if (i0 && !i1) {
        return true;
    }
    if (i1 && !i0) {
        return false;
    }
    int32_t t0 = (int32_t)((this->now - c0->dirtiedTime) / 2000);
    int32_t t1 = (int32_t)((this->now - c1->dirtiedTime) / 2000);
    if (t0 < t1) {
        return true;
    }
    if (t0 > t1) {
        return true;
    }
    return c0->distanceToSqr(this->player) < c1->distanceToSqr(this->player) ? true : false;
}