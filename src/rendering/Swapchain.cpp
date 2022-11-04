#include "Swapchain.h"
#include <VkBootstrap.h>

namespace crucible
{
    Swapchain::Swapchain(VkSurfaceKHR surface, int width, int height)
    {
        _surface = surface;
        _width = width;
        _height = height;

        //create synchronization structures

        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.pNext = nullptr;

        //we want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        GraphicsCard& card = CrucibleLib::graphicsCard();
        if(vkCreateFence(card.device(), &fenceCreateInfo, nullptr, &_renderFence)!=VK_SUCCESS)
        {
            throw std::runtime_error("Unable to create Render Fence");
        }

        //for the semaphores we don't need any flags
        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.pNext = nullptr;
        semaphoreCreateInfo.flags = 0;

        if(vkCreateSemaphore(card.device(), &semaphoreCreateInfo, nullptr, &_presentSemaphore)!= VK_SUCCESS)
        {
            throw std::runtime_error("Unable to create Present Semaphore");
        }
        if(vkCreateSemaphore(card.device(), &semaphoreCreateInfo, nullptr, &_renderSemaphore)!=VK_SUCCESS)
        {
            throw std::runtime_error("Unable to create Render Semaphore");
        }

        recreateSwapchain();
    }

    void Swapchain::recreateSwapchain()
    {
        GraphicsCard& card = CrucibleLib::graphicsCard();
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

    }

    void Swapchain::cleanUp()
    {
        GraphicsCard& card = CrucibleLib::graphicsCard();

        //but we still need to clean up image views
        for(auto& swapChainImageView : _swapchainImageViews)
        {
            vkDestroyImageView(card.device(),swapChainImageView, nullptr);
        }

        //this call also destroys the associated images
        vkDestroySwapchainKHR(card.device(),_swapchain, nullptr);
    }

    Swapchain::~Swapchain()
    {
        cleanUp();
        GraphicsCard& card = CrucibleLib::graphicsCard();
        vkDeviceWaitIdle(card.device());
        vkDestroySemaphore(card.device(),_presentSemaphore, nullptr);
        vkDestroySemaphore(card.device(),_renderSemaphore, nullptr);
        vkDestroyFence(card.device(),_renderFence, nullptr);
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
        if(vkWaitForFences(card.device(),1,&_renderFence, true, 1000000000)!= VK_SUCCESS)
        {
            throw std::runtime_error("Render Fence wait failed");
        }
        if(vkResetFences(card.device(),1,&_renderFence))
        {
            throw std::runtime_error("Render Fence reset failed");
        }
        if(vkAcquireNextImageKHR(card.device(), _swapchain, 1000000000, _presentSemaphore, nullptr, &_swapchainImageIndex)!= VK_SUCCESS)
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
} // crucible