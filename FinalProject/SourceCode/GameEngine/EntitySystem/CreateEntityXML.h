#pragma once

#include <unordered_map>
#include "flecs.h"
#include <string>

std::vector<float> vectorFromString(std::string string);
flecs::entity createEntityFromXML(flecs::world& ecs, const char* filename);