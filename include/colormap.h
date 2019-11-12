#pragma once

#include <map>

const std::map <std::string, std::array<uint8_t, 3>> colormap = {
      {"red",       {255, 0,   0}},
      {"blue",      {0,   0,   255}},
      {"green",     {0,   255, 0}},
      {"white",     {255, 255, 255}},
      {"orange",    {255, 128, 0}},
      {"magicpink", {255, 0,   255}}
};

void setRGB(const std::string& name) {
   const auto i = colormap.find(name);
   if(i != colormap.end())
      RGB.color(std::get<0>(i->second), std::get<1>(i->second), std::get<2>(i->second));
   else
      RGB.color(32, 32, 32);
}
