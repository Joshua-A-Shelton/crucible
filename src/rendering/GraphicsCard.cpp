#include "GraphicsCard.h"

namespace crucible
{

    void GraphicsCard::init(vkb::Device& device)
    {
        _physicalDevice = device.physical_device;
        _device = device.device;
        _graphicsQueue = device.get_queue(vkb::QueueType::graphics).value();
        _graphicsQueueFamily = device.get_queue_index(vkb::QueueType::graphics).value();
    }

    void GraphicsCard::cleanup()
    {
        if(_device)
        {
            vkDeviceWaitIdle(_device);
            vkDestroyDevice(_device, nullptr);
        }
    }

    VkPhysicalDevice GraphicsCard::physicalDevice()
    {
        return _physicalDevice;
    }

    VkDevice GraphicsCard::device()
    {
        return _device;
    }

    VkQueue GraphicsCard::graphicsQueue()
    {
        return _graphicsQueue;
    }

    uint32_t GraphicsCard::graphicsQueueFamily()
    {
        return _graphicsQueueFamily;
    }


} // rendering