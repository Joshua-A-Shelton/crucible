#ifndef CRUCIBLE_WINDOW_H
#define CRUCIBLE_WINDOW_H

#include <memory>
#include <SDL2/SDL.h>
#include "rendering/Swapchain.h"
namespace crucible
{
    class Window
    {
    private:
        Window(int w, int h, const char* title);
        SDL_Window* _handle;
        bool _isAlive;
        VkSurfaceKHR _surface;
        Swapchain* _swapChain;
        void onResize();
        void pollEvents();
        void draw();
    public:
        static std::shared_ptr<Window> Create(int w, int h, const char* title);
        Window(const Window&)=delete;
        Window& operator=(const Window&)=delete;
        ~Window();
        int width();
        int height();
        void show();

    };

} // crucible
#endif //CRUCIBLE_WINDOW_H