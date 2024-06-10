// #pragma once
// #include "types.h"
//
// #include <string>
// #include <unordered_map>
//
// struct Texture {
//     int width, height;
//     Rect mapCoord;
// };
//
// class TextureMap {
//     private:
//         uint textureID = 0;
//         std::unordered_map<std::string, Texture> m_textureMap;
//         
//     public:
//         explicit TextureMap(const std::unordered_map<std::string, std::string> &paths);
//         virtual ~TextureMap();
//
//         Texture operator[](const std::string &name) const;
// };
//
//
//
