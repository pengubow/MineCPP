#include "renderer/DistanceSorter.h"

DistanceSorter::DistanceSorter(shared_ptr<Player> player) 
    : player(player) {}

bool DistanceSorter::operator()(shared_ptr<Chunk>& c0, shared_ptr<Chunk>& c1) {
    return c0->distanceToSqr(this->player) < c1->distanceToSqr(this->player) ? true : false;
}