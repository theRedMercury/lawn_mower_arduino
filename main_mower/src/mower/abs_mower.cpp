/**
 *  @brief  lawn mower home
 *  @author Nicolas Masson <https://github.com/theRedMercury>
 *  @date   2021 - 2022
 */

#include "abs_mower.hpp"

abs_mower_class::abs_mower_class(mower_manager &mower_m, String name)
{
    mower = &mower_m;
    class_name = name;
}