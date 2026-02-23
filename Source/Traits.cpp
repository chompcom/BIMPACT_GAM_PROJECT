#include "Traits.h"

bool HasCommonTrait(Labels const& a, Labels const& b)
{
    for (auto const& thing : a) {
            if (b.find(thing) != b.end()) {
                return true;
            }
        }
        return false;

}
