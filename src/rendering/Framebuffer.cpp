#include "Framebuffer.h"

namespace crucible
{

    std::shared_ptr<Framebuffer> Framebuffer::Create(std::vector<std::shared_ptr<Texture>> &colorBuffers, bool includeDepth, bool includeStencil, bool clearOnLoad)
    {
        return std::shared_ptr<Framebuffer>(new Framebuffer(colorBuffers,includeDepth,includeStencil,clearOnLoad));
    }

    Framebuffer::Framebuffer(std::vector<std::shared_ptr<Texture>> &colorBuffers, bool includeDepth, bool includeStencil, bool clearOnLoad)
    {
        _colorBuffers = colorBuffers;
        _clearOnLoad = clearOnLoad;
    }


} // crucible