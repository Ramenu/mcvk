#ifndef MCVULKAN_QUEUE_HPP
#define MCVULKAN_QUEUE_HPP

#include <array>
#include "mcvulkan/types.hpp"
#include "mcvulkan/physicaldeviceinfo.hpp"
#include "mcvulkan/global.hpp"


namespace Queue
{
    enum FamilyIndex
    {
        GraphicsQueueIndex,
        PresentationQueueIndex
    };

    class QueueFamilyIndices
    {
        private:
            static constexpr usize __QUEUE_FAMILY_INDICES_CURRENT_LINE_ =  __LINE__;
            enum IndexFlags {
                None = 0x0,
                GraphicsQueueCompatible = 0x1,
                PresentationQueueCompatible = 0x2
            } flags {};
            static constexpr usize __QUEUE_TOTAL_INDICES_ = __LINE__ - __QUEUE_FAMILY_INDICES_CURRENT_LINE_ - 4;
            static constexpr auto __QUEUE_FLAG_INDICES_SUM_ {Global::FLAG_SUM(__QUEUE_TOTAL_INDICES_)};
            std::array<u32, 2> indices {};
            void constexpr set(FamilyIndex family_index, u32 i) noexcept
            {
                indices.at(static_cast<usize>(family_index)) = i;
            }
        public:

            QueueFamilyIndices(Device::PhysicalDeviceInfo physical_device, VkSurfaceKHR surface) noexcept;
            QueueFamilyIndices() noexcept = default;
            bool constexpr is_complete() const noexcept { return (flags&__QUEUE_FLAG_INDICES_SUM_) == __QUEUE_FLAG_INDICES_SUM_; }
            auto constexpr get(FamilyIndex family_index) const noexcept
            {
                return indices.at(static_cast<usize>(family_index));
            }
            auto constexpr belongs_to_one_family() const noexcept
            {
                for (usize i {}; i < indices.size() - 1; ++i)
                    if (indices.at(i) != indices.at(i+1))
                        return false;
                return true;
            }
            auto const constexpr &array() const noexcept { return indices; }
    };
}

#endif // MCVULKAN_QUEUE_HPP