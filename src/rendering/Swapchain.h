#ifndef CRUCIBLE_SWAPCHAIN_H
#define CRUCIBLE_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include "../CrucibleLib.h"
#include <vector>

namespace crucible
{
    enum RenderMode
    {
        SINGLEBUFFER = VK_PRESENT_MODE_IMMEDIATE_KHR,
        VSYNC = VK_PRESENT_MODE_MAILBOX_KHR,
        TRIPLE_BUFFER = VK_PRESENT_MODE_FIFO_KHR
    };

    class Swapchain
    {
    private:
        VkSwapchainKHR _swapchain = nullptr;
        VkFormat _swapchainImageFormat;
        std::vector<VkImage> _swapchainImages;
        std::vector<VkImageView> _swapchainImageViews;
        VkSurfaceKHR _surface;
        RenderMode _renderMode = VSYNC;
        unsigned int _width, _height;
        VkSemaphore _presentSemaphore = nullptr, _renderSemaphore = nullptr;
        VkFence _renderFence = nullptr;
        uint32_t _swapchainImageIndex = 0;
        VkClearValue _clearColor = {0,0,1.0,1};
        Swapchain(VkSurfaceKHR surface, int width, int height);
        void recreateSwapchain();
        void cleanUp();
    public:
        Swapchain(const Swapchain&) = delete;
        Swapchain& operator=(const Swapchain&) = delete;
        ~Swapchain();
        void resize(unsigned int width, unsigned int height);
        void setRenderMode(RenderMode renderMode);
        void nextImage();
        VkImage currentColorImage();
        VkImageView currentColorAttachment();
        VkRenderingAttachmentInfoKHR currentColorAttachmentInfo();
        friend class Window;
    };

} // crucible
#endif //CRUCIBLE_SWAPCHAIN_H