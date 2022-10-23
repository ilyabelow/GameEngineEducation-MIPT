#pragma once

#include <unordered_map>
#include "flecs.h"


std::vector<float> vectorFromString(std::string string);
flecs::entity createEntityFromXML(flecs::world& ecs, const char* filename);