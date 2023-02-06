/**
 * This file is part of mcvk.
 * 
 * mcvk is free software: you can redistribute it and/or modify it under the terms of the 
 * GNU General Public License as published by the Free Software Foundation, either version 
 * 3 of the License, or (at your option) any later version.
 * 
 * mcvk is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with mcvk. If 
 * not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MCVK_VALIDATIONLAYERS_HPP
#define MCVK_VALIDATIONLAYERS_HPP

#ifndef NDEBUG
    #include <array>
    static constexpr std::array VALIDATION_LAYERS {
        "VK_LAYER_KHRONOS_validation"
    };
#endif

#endif // MCVK_VALIDATIONLAYERS_HPP
