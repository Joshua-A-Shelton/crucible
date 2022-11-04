#ifndef CRUCIBLE_CRUCIBLELIB_H
#define CRUCIBLE_CRUCIBLELIB_H

#include <vulkan/vulkan.h>
#include "rendering/GraphicsCard.h"

namespace crucible
{
    class CrucibleLib
    {
    private:
        inline static GraphicsCard _graphicsCard;
    public:
        static void init(bool includeValidationLayers);
        static void cleanup();
        static VkInstance vulkanInstance();
        static GraphicsCard& graphicsCard();
    };

} // crucible
#endif //CRUCIBLE_CRUCIBLELIB_H