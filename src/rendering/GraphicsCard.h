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
        VkCommandPool _commandPool = nullptr;
        VkCommandBuffer  _mainCommandBuffer = nullptr;
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
        void resetCommandBuffer();
        //TODO delete this, submit sub buffers instead
        VkCommandBuffer mainCommandBuffer();
        VkQueue graphicsQueue();
    };
} // rendering
#endif //CRUCIBLE_GRAPHICSCARD_H