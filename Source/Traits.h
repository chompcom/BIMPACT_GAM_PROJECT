//This file has the definitions for traits and how to contain them


#pragma once

#include <set>
#include <string>

using Labels = std::set<std::string>;

bool HasCommonTrait(Labels const& a, Labels const& b);