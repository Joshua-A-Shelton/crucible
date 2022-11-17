#ifndef CRUCIBLE_FRAMEBUFFER_H
#define CRUCIBLE_FRAMEBUFFER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "Texture.h"
#include <memory>
namespace crucible
{

    class Framebuffer
    {
    private:
        std::vector<std::shared_ptr<Texture>> _colorBuffers;
        VkFramebuffer _handle;
        bool _clearOnLoad = true;
        Framebuffer(std::vector<std::shared_ptr<Texture>>& colorBuffers, bool includeDepth, bool includeStencil, bool clearOnLoad);
    public:
        Framebuffer() = delete;
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        std::shared_ptr<Framebuffer> Create(std::vector<std::shared_ptr<Texture>>& colorBuffers, bool includeDepth, bool includeStencil, bool clearOnLoad);
    };

} // crucible
#endif //CRUCIBLE_FRAMEBUFFER_H