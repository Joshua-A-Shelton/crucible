#ifndef CRUCIBLE_COMMANDBUFFER_H
#define CRUCIBLE_COMMANDBUFFER_H

#include <vulkan/vulkan.h>
namespace crucible
{

    class CommandBuffer
    {
    private:
        VkCommandBuffer _buffer;
    public:
        CommandBuffer(VkCommandBuffer buffer);
        void reset();
        void beginFrame();
        void endFrame();

        void temp_ImageToRender(VkImage image);
        void temp_ImageToPresent(VkImage image);
    };

} // crucible
#endif //CRUCIBLE_COMMANDBUFFER_H