#include "sound/BaseSoundPos.h"

class EntitySoundPos : public BaseSoundPos {
    shared_ptr<Entity> source;
public:
    EntitySoundPos(shared_ptr<Entity>& var1, shared_ptr<Entity> var2);

    float getRotationDiff();
    float getDistanceSq();
};