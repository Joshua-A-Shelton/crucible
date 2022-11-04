#include "Window.h"
#include "SDL_vulkan.h"

namespace crucible
{
    Window::Window(int w, int h, const char* title)
    {
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
        _handle = SDL_CreateWindow(title,SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w,h, window_flags);
        _isAlive = true;
        SDL_Vulkan_CreateSurface(_handle,CrucibleLib::vulkanInstance(),&_surface);
        _swapChain = new Swapchain(_surface,w,h);
    }

    std::shared_ptr<Window> Window::Create(int w, int h, const char *title)
    {
        return std::shared_ptr<Window>(new Window(w,h,title));
    }

    Window::~Window()
    {
        SDL_DestroyWindow(_handle);
    }

    int Window::width()
    {
        int w,h;
        SDL_GetWindowSize(_handle,&w,&h);
        return w;
    }

    int Window::height()
    {
        int w,h;
        SDL_GetWindowSize(_handle,&w,&h);
        return h;
    }

    void Window::show()
    {

        while(_isAlive)
        {
            pollEvents();
            draw();
        }
        delete(_swapChain);
        vkDestroySurfaceKHR(CrucibleLib::vulkanInstance(),_surface, nullptr);
    }

    void Window::onResize()
    {

    }

    void Window::pollEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    _isAlive = false;
                    return;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        onResize();
                    }
                    break;
            }
        }
    }

    void Window::draw()
    {
        _swapChain->nextImage();

        auto& card = CrucibleLib::graphicsCard();

        //naming it cmd for shorter writing
        VkCommandBuffer cmd = card.mainCommandBuffer();
        vkResetCommandBuffer(cmd,0);

        //TODO: remove from here, submit sub command buffer instead
        //begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
        VkCommandBufferBeginInfo cmdBeginInfo = {};
        cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBeginInfo.pNext = nullptr;

        cmdBeginInfo.pInheritanceInfo = nullptr;
        cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        if(vkBeginCommandBuffer(cmd, &cmdBeginInfo)!= VK_SUCCESS)
        {
            throw std::runtime_error("Unable to begin command buffer");
        }
        //this must be just after begin buffer
        const VkImageMemoryBarrier image_memory_barrier_begin {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .image = _swapChain->currentColorImage(),
                .subresourceRange = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                }
        };
        vkCmdPipelineBarrier(
                cmd,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
                0,
                0,
                nullptr,
                0,
                nullptr,
                1, // imageMemoryBarrierCount
                &image_memory_barrier_begin // pImageMemoryBarriers
        );


        //TODO: set up correct render targets
        auto colorAttachmentInfo = _swapChain->currentColorAttachmentInfo();

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea = {0,0,_swapChain->_width,_swapChain->_height};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachmentInfo;
        //renderingInfo.pDepthAttachment = &depth;
        //renderingInfo.pStencilAttachment = &stencil;


        vkCmdBeginRendering(cmd,&renderingInfo);
        //Draw commands go here
        vkCmdEndRendering(cmd);

        //this must be before end buffer
        const VkImageMemoryBarrier image_memory_barrier_end {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .image = _swapChain->currentColorImage(),
                .subresourceRange = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                }
        };
        vkCmdPipelineBarrier(
                cmd,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // srcStageMask
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // dstStageMask
                0,
                0,
                nullptr,
                0,
                nullptr,
                1, // imageMemoryBarrierCount
                &image_memory_barrier_end // pImageMemoryBarriers
        );

        //finalize the command buffer (we can no longer add commands, but it can now be executed)
        if(vkEndCommandBuffer(cmd)!=VK_SUCCESS)
        {
            throw std::runtime_error("Unable to end command buffer");
        }

        //prepare the submission to the queue.
        //we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
        //we will signal the _renderSemaphore, to signal that rendering has finished

        VkSubmitInfo submit = {};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.pNext = nullptr;

        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        submit.pWaitDstStageMask = &waitStage;

        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &_swapChain->_presentSemaphore;

        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &_swapChain->_renderSemaphore;

        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &cmd;

        //submit command buffer to the queue and execute it.
        // _renderFence will now block until the graphic commands finish execution
        if(vkQueueSubmit(card.graphicsQueue(), 1, &submit, _swapChain->_renderFence)!=VK_SUCCESS)
        {
            throw std::runtime_error("Unable to submit render queue");
        }

        // this will put the image we just rendered into the visible window.
        // we want to wait on the _renderSemaphore for that,
        // as it's necessary that drawing commands have finished before the image is displayed to the user
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;

        presentInfo.pSwapchains = &_swapChain->_swapchain;
        presentInfo.swapchainCount = 1;

        presentInfo.pWaitSemaphores = &_swapChain->_renderSemaphore;
        presentInfo.waitSemaphoreCount = 1;
        auto imageIndex = _swapChain->_swapchainImageIndex;
        presentInfo.pImageIndices = &(imageIndex);


        if(vkQueuePresentKHR(card.graphicsQueue(), &presentInfo)!= VK_SUCCESS)
        {
            throw std::runtime_error("Unable to present image");
        }

    }


} // crucible