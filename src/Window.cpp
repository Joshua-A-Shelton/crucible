#include "Window.h"
#include <SDL_vulkan.h>

namespace crucible
{
    Window::Window(int w, int h, const char* title)
    {
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
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
        SDL_DestroyWindow(_handle);
    }

    void Window::onResize()
    {
        _swapChain->resize(width(),height());
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
        CommandBuffer cmd = CommandBuffer(_swapChain->currentCommandBuffer());
        cmd.reset();
        cmd.beginFrame();
        cmd.temp_ImageToRender(_swapChain->currentColorImage());
        cmd.temp_ImageToPresent(_swapChain->currentColorImage());
        cmd.endFrame();
        _swapChain->submitCommands();

    }


} // crucible