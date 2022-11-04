#include "GraphicsCard.h"

namespace crucible
{

    void GraphicsCard::init(vkb::Device& device)
    {
        _physicalDevice = device.physical_device;
        _device = device.device;
        _graphicsQueue = device.get_queue(vkb::QueueType::graphics).value();
        _graphicsQueueFamily = device.get_queue_index(vkb::QueueType::graphics).value();

        //init command queue
        VkCommandPoolCreateInfo commandPoolInfo{};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.pNext = nullptr;

        //the command pool will be one that can submit graphics commands
        commandPoolInfo.queueFamilyIndex = _graphicsQueueFamily;
        //we also want the pool to allow for resetting of individual command buffers
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool)!=VK_SUCCESS)
        {
            throw std::runtime_error("Unable to initialize Graphics Card Command Pool");
        }

        //init command buffer
        //allocate the default command buffer that we will use for rendering
        VkCommandBufferAllocateInfo cmdAllocInfo = {};
        cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.pNext = nullptr;

        //commands will be made from our _commandPool
        cmdAllocInfo.commandPool = _commandPool;
        //we will allocate 1 command buffer
        cmdAllocInfo.commandBufferCount = 1;
        // command level is Primary
        cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;


        if(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_mainCommandBuffer)!= VK_SUCCESS)
        {
            throw std::runtime_error("Unable to initialize Graphics Card Command Buffer");
        }
    }

    void GraphicsCard::cleanup()
    {
        if(_device)
        {
            vkDeviceWaitIdle(_device);
            //automatically destroys associated command buffer as well
            vkDestroyCommandPool(_device,_commandPool, nullptr);
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

    void GraphicsCard::resetCommandBuffer()
    {
        vkResetCommandBuffer(_mainCommandBuffer,0);
    }

    VkCommandBuffer GraphicsCard::mainCommandBuffer()
    {
        return _mainCommandBuffer;
    }

    VkQueue GraphicsCard::graphicsQueue()
    {
        return _graphicsQueue;
    }


} // rendering