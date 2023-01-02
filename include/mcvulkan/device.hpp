#ifndef MCVULKAN_DEVICE_HPP
#define MCVULKAN_DEVICE_HPP

#include <vulkan/vulkan.h>
#include "mcvulkan/queue.hpp"

namespace Device
{
    struct DeviceInfo
    {
        VkPhysicalDevice device {};
        VkPhysicalDeviceProperties properties {};
        VkPhysicalDeviceFeatures features {};
        VkMemoryHeap memory_heap {};
        Queue::QueueFamilyIndices queue_family_indices {};
    };

    class LogicalDevice
    {
        private:
            VkDevice device {VK_NULL_HANDLE};
            VkQueue graphics_queue {};
        public:
            //LogicalDevice(const DeviceInfo &selected_device_info) noexcept; 
            constexpr LogicalDevice() noexcept = default;
            explicit LogicalDevice(const DeviceInfo &selected_device_info) noexcept;
            constexpr LogicalDevice& operator=(LogicalDevice &&other) noexcept
            {
                this->device = other.device;
                this->graphics_queue = other.graphics_queue;

                other.device = VK_NULL_HANDLE;
                return *this;
            }
            constexpr explicit LogicalDevice(LogicalDevice &&other) noexcept
            {
                this->device = other.device;
                this->graphics_queue = other.graphics_queue;

                other.device = VK_NULL_HANDLE;
            }
            // delete copy operations
            LogicalDevice(const LogicalDevice &other) noexcept = delete;
            LogicalDevice& operator=(const LogicalDevice &other) noexcept = delete;

            ~LogicalDevice() noexcept; 
            constexpr auto get() const { return device; }
    };

    extern DeviceInfo select_physical_device(VkInstance instance) noexcept;
}

#endif // MCVULKAN_DEVICE_HPP
