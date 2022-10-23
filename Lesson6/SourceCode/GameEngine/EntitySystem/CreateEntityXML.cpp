#include "CreateEntityXML.h"

#include <fstream>
#include <sstream>

#include "../tinyxml2.h"

#include "ecsMesh.h"
#include "ecsControl.h"
#include "ecsPhys.h"
#include "ecsScript.h"

enum attributes_enum {
  mesh_attribute = 0,
  position_attribute,
  velocity_attribute,
  bounce_plane_attribute,
  bounciness_attribute,
  gravity_attribute,
  friction_amount_attribute,
  reload_timer_attribute,
  script_name_attribute
};

std::unordered_map<std::string, size_t> indices = {
  {"mesh",            mesh_attribute},
  {"position",        position_attribute},
  {"velocity",        velocity_attribute},
  {"bounce_plane",    bounce_plane_attribute},
  {"bounciness",      bounciness_attribute},
  {"gravity",         gravity_attribute},
  {"friction_amount", friction_amount_attribute},
  {"reload_timer",    reload_timer_attribute},
  {"script_name",     script_name_attribute}
};


std::vector<float> vectorFromString(std::string string) {
  std::vector<float> vect;
  std::stringstream ss(string);
  float i;

  while (ss >> i) {
    vect.push_back(i);

    if (ss.peek() == ',')
      ss.ignore();
  }
  return vect;
}

flecs::entity createEntityFromXML(flecs::world& ecs, const char* filename) {

  tinyxml2::XMLDocument doc;
  doc.LoadFile(filename);

  std::unordered_map<size_t, std::string> dict;

  tinyxml2::XMLElement* parent = doc.FirstChildElement()->FirstChildElement();
  tinyxml2::XMLElement* row = parent->FirstChildElement();
  while (row != NULL) {
    for (const tinyxml2::XMLAttribute* attr = row->FirstAttribute(); attr != 0; attr = attr->Next())
      dict[indices[attr->Name()]] = attr->Value();
    row = row->NextSiblingElement();
  }

  auto entity = ecs.entity();

  for (std::unordered_map<size_t, std::string>::iterator it = dict.begin(); it != dict.end(); ++it) {
    switch (it->first) {
    case mesh_attribute:
      if (it->second == "Cube")
        entity.add<CubeMesh>();
      else if (it->second == "Icosahedron")
        entity.add<IcosahedronMesh>();
      break;

    case position_attribute:
    {
      auto data = vectorFromString(it->second);
      entity.set(Position{ data[0], data[1], data[2] });
    }
    break;

    case velocity_attribute:
    {
      auto data = vectorFromString(it->second);
      entity.set(Velocity{ data[0], data[1], data[2] });
    }
    break;

    case bounce_plane_attribute:
    {
      auto data = vectorFromString(it->second);
      entity.set(BouncePlane{ data[0], data[1], data[2], data[3] });
    }
    break;

    case bounciness_attribute:
      entity.set(Bounciness{ std::stof(it->second) });
      break;

    case gravity_attribute:
    {
      auto data = vectorFromString(it->second);
      entity.set(Gravity{ data[0], data[1], data[2] });
    }
    break;

    case friction_amount_attribute:
      entity.set(FrictionAmount{ std::stof(it->second) });
      break;

    case reload_timer_attribute:
    {
      int numberOfBulletsToAdd;
      float time;
      std::stringstream ss(it->second);
      ss >> numberOfBulletsToAdd;
      ss.ignore();
      ss >> time;
      entity.set(ReloadTimer{ numberOfBulletsToAdd, time });
    }
    break;

    case script_name_attribute:
      entity.set(Scripts("../../../Assets/Scripts/" + it->second));
      break;

    default:
      break;
    }
  }

  return entity;
}
