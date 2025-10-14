#include "renderer/DirtyChunkSorter.h"
#include <iostream>

DirtyChunkSorter::DirtyChunkSorter(shared_ptr<Player>& player) 
    : player(player) {}

bool DirtyChunkSorter::operator()(shared_ptr<Chunk>& c0, shared_ptr<Chunk>& c1) {
    bool var3 = c0->isInFrustum;
    bool var4 = c1->isInFrustum;
    return var3 && !var4 ? true : ((!var4 || var3) && c0->distanceToSqr(player) < c1->distanceToSqr(player) ? true : false);
}