/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#ifndef TOOLS_H
#define TOOLS_H

#define PACKING __attribute__((packed)) // Pack the struct

struct PACKING XYZ_SHORT
{
    short x = 0;
    short y = 0;
    short z = 0;
};

struct PACKING XYZ_INT
{
    int x = 0;
    int y = 0;
    int z = 0;
};

struct PACKING XYZ_FLOAT
{
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
};

#endif