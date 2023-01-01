#ifndef MCVULKAN_GLOBAL_HPP
#define MCVULKAN_GLOBAL_HPP

namespace Global
{
    // Should be used for comparsions between two values/objects.
    // Order should be:
    // $1 > $2: Greater
    // $1 = $2: Equal
    // $1 < $2: Less
    // If a comparsion fails for whatever reason, use Fail
    enum class Compare
    {
        Greater,
        Equal,
        Less,
        Fail
    };

    #ifndef NDEBUG
        static constexpr bool IS_DEBUG_BUILD = true;
    #else
        static constexpr bool IS_DEBUG_BUILD = false;
    #endif

}

#endif // MCVULKAN_GLOBAL_HPP
