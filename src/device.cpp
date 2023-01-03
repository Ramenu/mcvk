 #include "mcvulkan/device.hpp"
 #include "mcvulkan/logger.hpp"
 #include "mcvulkan/global.hpp"
 #include <vector>
 #include <string>
 #include <set>
 
namespace Device
{

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
        // in the future can also check for sparse binding support, dynamic array indexing, and tesselation support,
        u32 score_one {}, score_two {};
        const auto device_type_one = device_type_rating(one.properties.deviceType);
        const auto device_type_two = device_type_rating(two.properties.deviceType);

        // Compare GPU type (dedicated VS integrated)
        if (device_type_one > device_type_two)
            score_one += 2;
        else if (device_type_one < device_type_two)
            score_two += 2;
        
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

    static inline bool can_use_physical_device(const DeviceInfo &info)
    {
        // Geometry shader support is required, as well as specific queue family support
        return info.features.geometryShader && info.queue_family_indices.is_complete();
    }

    [[nodiscard]] DeviceInfo select_physical_device(const VkComponents &components) noexcept
    {
        u32 count {};
        vkEnumeratePhysicalDevices(components.get_instance(), &count, nullptr);
        std::vector<VkPhysicalDevice> devices (count);

        if (count == 0)
            Logger::fatal_error("Could not find available GPUs with Vulkan support");
        
        vkEnumeratePhysicalDevices(components.get_instance(), &count, devices.data());
        
        DeviceInfo previous_device_info {};
        DeviceInfo info {}, selected_device_info {};
        bool appropriate_device_exists = false;

        // iterate through all the available devices in the
        // system and try to select the best one
        for (auto device : devices) {
            info.device = device;
            VkPhysicalDeviceMemoryProperties device_mem_properties {};

            vkGetPhysicalDeviceProperties(device, &info.properties);
            vkGetPhysicalDeviceMemoryProperties(device, &device_mem_properties);
            vkGetPhysicalDeviceFeatures(device, &info.features);

            #ifndef NDEBUG
                const auto check_dev_msg = std::string{"Checking device: "} + info.properties.deviceName;
                Logger::info(check_dev_msg.c_str());
            #endif

            const auto memory_heaps_ptr {device_mem_properties.memoryHeaps};
            const std::vector<VkMemoryHeap> memory_heaps {memory_heaps_ptr, memory_heaps_ptr + device_mem_properties.memoryHeapCount};

            // find VRAM size
            for (auto heap : memory_heaps) {
                if (heap.flags&VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    info.memory_heap = heap;
                    break;
                }
            }
            #ifndef NDEBUG
                info.queue_family_indices = Queue::find_queue_families(device, components.get_surface(), info.properties.deviceName);
            #else
                info.queue_family_indices = Queue::find_queue_families(device, components.get_surface());
            #endif

            const bool can_use_device = can_use_physical_device(info);

            // device must be compatible in order to use it
            if (can_use_device) {
                #ifndef NDEBUG
                    const auto msg = std::string{"Device "} + info.properties.deviceName + " supports required features.";
                    Logger::diagnostic(msg.c_str());
                #endif
                // if the previous device wasnt initialized yet, set the selected device to this device
                if (previous_device_info.device == VK_NULL_HANDLE) {
                    appropriate_device_exists = true;
                    selected_device_info = info;
                }
                else {
                    // now we can actually compare the devices
                    const auto cmp = compare_device_specs(info, previous_device_info);
                    if (cmp == Global::Compare::Greater)
                        selected_device_info = info;
                }
                previous_device_info = std::move(info);
            }
            #ifndef NDEBUG 
                else {
                    const auto msg = std::string{"Device "} + info.properties.deviceName + " does not support required features. Skipping...";
                    Logger::diagnostic(msg.c_str());
                }
            #endif

        }

        if (!appropriate_device_exists)
            Logger::fatal_error("Could not find a suitable GPU to run the game");

        #ifndef NDEBUG
            const auto msg = std::string{"Selected physical device: "} + selected_device_info.properties.deviceName;
            Logger::diagnostic(msg.c_str());
        #endif

        return selected_device_info;
    }

    LogicalDevice::LogicalDevice(const DeviceInfo &selected_device_info) noexcept
    {
        constexpr float QUEUE_PRIORITY {1.0f};

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos {};

        const std::set<u32> unique_queue_families {
            selected_device_info.queue_family_indices.graphics.value(), 
            selected_device_info.queue_family_indices.presentation.value()
        };

        for (auto queue_family : unique_queue_families) {
            VkDeviceQueueCreateInfo queue_create_info {};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_family;
            queue_create_info.queueCount = 1;

            // set priority of queue to influence scheduling of command buffer
            queue_create_info.pQueuePriorities = &QUEUE_PRIORITY;
            queue_create_infos.push_back(queue_create_info);
        }

        VkDeviceCreateInfo device_create_info {};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pQueueCreateInfos = queue_create_infos.data();
        device_create_info.queueCreateInfoCount = static_cast<u32>(queue_create_infos.size());
        device_create_info.pEnabledFeatures = &selected_device_info.features;

        if (vkCreateDevice(selected_device_info.device, &device_create_info, nullptr, &device) != VK_SUCCESS)
            Logger::fatal_error("Failed to create logical device");
        #ifndef NDEBUG
            else
                Logger::diagnostic("Logical device created successfully");
        #endif

        vkGetDeviceQueue(device, selected_device_info.queue_family_indices.graphics.value(), 0, &graphics_queue);
        vkGetDeviceQueue(device, selected_device_info.queue_family_indices.presentation.value(), 0, &presentation_queue);
    }

    LogicalDevice::~LogicalDevice() noexcept
    {
        if (device != VK_NULL_HANDLE) {
            if constexpr (Global::IS_DEBUG_BUILD)
                Logger::diagnostic("De-allocating logical device");
            vkDestroyDevice(device, nullptr); 
        }
    }


}
