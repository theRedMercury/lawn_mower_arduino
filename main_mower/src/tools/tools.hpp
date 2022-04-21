/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef TOOLS_H
#define TOOLS_H

#define PACKING __attribute__((packed)) // Pack the struct

struct PACKING XYZ
{
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
};

#endif