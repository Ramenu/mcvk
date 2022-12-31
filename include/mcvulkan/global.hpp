#ifndef MCVULKAN_GLOBAL_HPP
#define MCVULKAN_GLOBAL_HPP

namespace Global
{
    #ifndef NDEBUG
        static constexpr bool IS_DEBUG_BUILD = true;
    #else
        static constexpr bool IS_DEBUG_BUILD = false;
    #endif

}

#endif // MCVULKAN_GLOBAL_HPP
