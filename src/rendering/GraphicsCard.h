#ifndef CRUCIBLE_GRAPHICSCARD_H
#define CRUCIBLE_GRAPHICSCARD_H

#include <vulkan/vulkan.h>
#include "VkBootstrap.h"

namespace crucible
{
    class GraphicsCard
    {
    private:
        VkPhysicalDevice _physicalDevice = nullptr;
        VkDevice _device = nullptr;
        VkQueue _graphicsQueue = nullptr;
        uint32_t _graphicsQueueFamily = 0;
        GraphicsCard()=default;
        ///must be called before use
        void init(vkb::Device& device);
        void cleanup();
    public:
        GraphicsCard(const GraphicsCard&) = delete;
        GraphicsCard& operator=(const GraphicsCard&) = delete;
        friend class CrucibleLib;
        VkPhysicalDevice physicalDevice();
        VkDevice device();
        VkQueue graphicsQueue();
        uint32_t graphicsQueueFamily();
    };
} // rendering
#endif //CRUCIBLE_GRAPHICSCARD_H