 #include "mcvulkan/device.hpp"
 #include "mcvulkan/types.hpp"
 #include "mcvulkan/logger.hpp"
 #include "mcvulkan/global.hpp"
 #include <vector>
 #include <string>
 
namespace Device
{
    struct DeviceInfo
    {
        VkPhysicalDevice device {};
        VkPhysicalDeviceProperties properties {};
        VkPhysicalDeviceFeatures features {};
        VkMemoryHeap memory_heap {};
    };

    static unsigned device_type_rating(VkPhysicalDeviceType type)
    {
        switch (type)
        {
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return 2;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return 1;
            // other types also exist, but for simplicity sake just
            // return 0.
            default: return 0;
        }
    }

    static bool received_vram_retrieval_error(const DeviceInfo &info)
    {
        if (!(info.memory_heap.flags&VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)) {
            const auto msg = std::string{"Failed to retrieve "} + info.properties.deviceName + " VRAM size";
            Logger::error(msg.c_str());
            return true;
        }
        return false;
    }

    // keep in mind this comparsion is quite primitive, but it should at least be good enough
    static Global::Compare compare_device_specs(const DeviceInfo &one, const DeviceInfo &two)
    {
        bool vram_retrieval_error = false;
        // in the future can also check for sparse binding support, dynamic array indexing, and tesselation support,
        u32 score_one {}, score_two {};
        const auto device_type_one = device_type_rating(one.properties.deviceType);
        const auto device_type_two = device_type_rating(two.properties.deviceType);

        // Compare GPU type (dedicated VS integrated)
        if (device_type_one > device_type_two)
            ++score_one;
        else if (device_type_one < device_type_two)
            ++score_two;
        
        if (received_vram_retrieval_error(one) || received_vram_retrieval_error(two))
            return Global::Compare::Fail;
        
        // Compare VRAM size
        if (one.memory_heap.size > two.memory_heap.size)
            ++score_one;
        else if (one.memory_heap.size < two.memory_heap.size)
            ++score_two;
        
        // Compare maximum 2D texture size (higher values make for better quality)
        if (one.properties.limits.maxImageDimension2D > two.properties.limits.maxImageDimension2D)
            ++score_one;
        else if (one.properties.limits.maxImageDimension2D < two.properties.limits.maxImageDimension2D)
            ++score_two;
        
        if (score_one > score_two)
            return Global::Compare::Greater;
        else if (score_one < score_two)
            return Global::Compare::Less;
        return Global::Compare::Equal;
    }

    static inline bool can_use_device(const DeviceInfo &info)
    {
        // Geometry shader support is required
        return info.features.geometryShader;
    }

    void select_physical_device(VkInstance instance)
    {
        VkPhysicalDevice selected_device {VK_NULL_HANDLE};

        u32 count {};
        std::vector<VkPhysicalDevice> devices (count);
        vkEnumeratePhysicalDevices(instance, &count, devices.data());

        if (count == 0)
            Logger::fatal_error("Could not find available GPUs with Vulkan support");
        
        DeviceInfo previous_device_info {};

        // iterate through all the available devices and try to select the
        // best one
        for (auto device : devices) {
            DeviceInfo info {};
            VkPhysicalDeviceMemoryProperties device_mem_properties {};

            vkGetPhysicalDeviceProperties(device, &info.properties);
            vkGetPhysicalDeviceMemoryProperties(device, &device_mem_properties);

            const auto memory_heaps_ptr {device_mem_properties.memoryHeaps};
            const std::vector<VkMemoryHeap> memory_heaps {memory_heaps_ptr, memory_heaps_ptr + device_mem_properties.memoryHeapCount};

            for (auto heap : memory_heaps) {
                if (heap.flags&VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    info.memory_heap = heap;
                    break;
                }
            }

            // if the previous device wasnt initialized yet, set the selected device to this device
            if (previous_device_info.device == VK_NULL_HANDLE)
                selected_device = device;
            else {
                // now we can actually compare the devices
                const auto cmp = compare_device_specs(info, previous_device_info);
                if (cmp == Global::Compare::Greater)
                    selected_device = device;
            }
            previous_device_info = std::move(info);
        }
    }
}
