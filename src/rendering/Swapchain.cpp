#include "Swapchain.h"
#include <VkBootstrap.h>

namespace crucible
{
    Swapchain::Swapchain(VkSurfaceKHR surface, int width, int height)
    {
        _surface = surface;
        _width = width;
        _height = height;

        recreateSwapchain();
    }

    void Swapchain::recreateSwapchain()
    {
        GraphicsCard& card = CrucibleLib::graphicsCard();
        vkDeviceWaitIdle(card.device());


        vkb::SwapchainBuilder swapchainBuilder{card.physicalDevice(),card.device(),_surface};
        auto vkbSwapchain =
                swapchainBuilder
                .use_default_format_selection()
                .set_desired_present_mode(static_cast<VkPresentModeKHR>(_renderMode))
                .set_desired_extent(_width,_height)
                .set_old_swapchain(_swapchain)
                .build().value();
        if(_swapchain)
        {
            cleanUp();
        }
        _swapchain = vkbSwapchain.swapchain;
        _swapchainImages = vkbSwapchain.get_images().value();
        _swapchainImageViews = vkbSwapchain.get_image_views().value();

        _swapchainImageFormat = vkbSwapchain.image_format;

        //init command queue
        VkCommandPoolCreateInfo commandPoolInfo{};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.pNext = nullptr;

        //the command pool will be one that can submit graphics commands
        commandPoolInfo.queueFamilyIndex = card.graphicsQueueFamily();
        //we also want the pool to allow for resetting of individual command buffers
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if(vkCreateCommandPool(card.device(), &commandPoolInfo, nullptr, &_commandPool)!=VK_SUCCESS)
        {
            throw std::runtime_error("Unable to initialize Graphics Card Command Pool");
        }

        _maxFramesInFlight = _swapchainImages.size();

        _commandBuffers.resize(_maxFramesInFlight,VK_NULL_HANDLE);
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;

        allocInfo.commandPool = _commandPool;
        allocInfo.commandBufferCount = _commandBuffers.size();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) _commandBuffers.size();
        if (vkAllocateCommandBuffers(card.device(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        _imageAvailableSemaphores.resize(_maxFramesInFlight,VK_NULL_HANDLE);
        _renderFinishedSemaphores.resize(_maxFramesInFlight,VK_NULL_HANDLE);
        _inFlightFences.resize(_maxFramesInFlight, VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for(int i=0; i<_maxFramesInFlight; i++)
        {
            vkCreateSemaphore(card.device(),&semaphoreInfo, nullptr,&_imageAvailableSemaphores[i]);
            vkCreateSemaphore(card.device(),&semaphoreInfo, nullptr,&_renderFinishedSemaphores[i]);
            vkCreateFence(card.device(), &fenceInfo, nullptr,&_inFlightFences[i]);
        }

    }

    void Swapchain::cleanUp()
    {
        GraphicsCard& card = CrucibleLib::graphicsCard();
        _swapchainImages.clear();
        for(int i=0; i< _swapchainImageViews.size(); i++)
        {
            vkDestroyImageView(card.device(),_swapchainImageViews[i], nullptr);
        }
        vkDestroySwapchainKHR(card.device(),_swapchain, nullptr);


        for(int i=0; i< _maxFramesInFlight; i++)
        {
            vkDestroySemaphore(card.device(), _imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(card.device(), _renderFinishedSemaphores[i], nullptr);
            vkDestroyFence(card.device(), _inFlightFences[i], nullptr);
        }
        vkDestroyCommandPool(card.device(),_commandPool, nullptr);
    }

    Swapchain::~Swapchain()
    {
        GraphicsCard& card = CrucibleLib::graphicsCard();
        vkDeviceWaitIdle(card.device());
        cleanUp();
    }

    void Swapchain::resize(unsigned int width, unsigned int height)
    {
        _width = width;
        _height = height;
        recreateSwapchain();
    }

    void Swapchain::setRenderMode(RenderMode renderMode)
    {
        _renderMode = renderMode;
        recreateSwapchain();
    }

    void Swapchain::nextImage()
    {
        GraphicsCard& card = CrucibleLib::graphicsCard();
        if(vkWaitForFences(card.device(),1,&_inFlightFences[_currentFrameIndex], true, 1000000000)!= VK_SUCCESS)
        {
            throw std::runtime_error("Render Fence wait failed");
        }
        if(vkResetFences(card.device(),1,&_inFlightFences[_currentFrameIndex]))
        {
            throw std::runtime_error("Render Fence reset failed");
        }
        if(vkAcquireNextImageKHR(card.device(), _swapchain, 1000000000, _imageAvailableSemaphores[_currentFrameIndex], nullptr, &_swapchainImageIndex)!= VK_SUCCESS)
        {
            throw std::runtime_error("Failed to aquire next image in swap chain");
        }
    }

    VkImageView Swapchain::currentColorAttachment()
    {
        return _swapchainImageViews[_swapchainImageIndex];
    }

    VkRenderingAttachmentInfoKHR Swapchain::currentColorAttachmentInfo()
    {

        VkRenderingAttachmentInfoKHR color_attachment_info {
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                .imageView = _swapchainImageViews[_swapchainImageIndex],
                .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .clearValue = _clearColor
        };

        return color_attachment_info;
    }

    VkImage Swapchain::currentColorImage()
    {
        return _swapchainImages[_swapchainImageIndex];
    }

    VkCommandBuffer Swapchain::currentCommandBuffer()
    {
        return _commandBuffers[_currentFrameIndex];
    }

    void Swapchain::submitCommands()
    {

        VkSubmitInfo submit = {};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.pNext = nullptr;

        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        submit.pWaitDstStageMask = &waitStage;

        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &_imageAvailableSemaphores[_currentFrameIndex];

        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &_renderFinishedSemaphores[_currentFrameIndex];

        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &_commandBuffers[_currentFrameIndex];

        if(vkQueueSubmit(CrucibleLib::graphicsCard().graphicsQueue(), 1, &submit, _inFlightFences[_currentFrameIndex])!=VK_SUCCESS)
        {
            throw std::runtime_error("Unable to submit render queue");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &_renderFinishedSemaphores[_currentFrameIndex];

        VkSwapchainKHR swapChains[] = {_swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &_swapchainImageIndex;

        GraphicsCard& card = CrucibleLib::graphicsCard();

        vkQueuePresentKHR(card.graphicsQueue(), &presentInfo);

        //increment frame index
        _currentFrameIndex = (_currentFrameIndex + 1) % _maxFramesInFlight;
    }
} // crucible