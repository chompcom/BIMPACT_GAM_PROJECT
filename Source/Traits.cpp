#include "Traits.h"
#include <iostream>
bool HasCommonTrait(Labels const& a, Labels const& b)
{
    for (std::string const& thing : a) {
            if (b.find(thing) != b.end()) {
                return true;
            }
        }
        return false;

}
