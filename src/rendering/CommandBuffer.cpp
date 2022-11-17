#include "CommandBuffer.h"
#include "../CrucibleLib.h"
namespace crucible
{
    CommandBuffer::CommandBuffer(VkCommandBuffer buffer)
    {
        _buffer = buffer;
    }

    void CommandBuffer::beginFrame()
    {
        VkCommandBufferBeginInfo cmdBeginInfo = {};
        cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBeginInfo.pNext = nullptr;

        cmdBeginInfo.pInheritanceInfo = nullptr;
        cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        if(vkBeginCommandBuffer(_buffer, &cmdBeginInfo)!= VK_SUCCESS)
        {
            throw std::runtime_error("Unable to begin command buffer");
        }
    }

    void CommandBuffer::endFrame()
    {
        if(vkEndCommandBuffer(_buffer)!=VK_SUCCESS)
        {
            throw std::runtime_error("Unable to end command buffer");
        }
    }

    void CommandBuffer::reset()
    {
        vkResetCommandBuffer(_buffer,0);
    }

    void CommandBuffer::temp_ImageToRender(VkImage image)
    {
        //this must be just after begin buffer
        const VkImageMemoryBarrier image_memory_barrier_begin {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .image = image,
                .subresourceRange = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                }
        };
        vkCmdPipelineBarrier(
                _buffer,
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
    }

    void CommandBuffer::temp_ImageToPresent(VkImage image)
    {
        //this must be before end buffer
        const VkImageMemoryBarrier image_memory_barrier_end {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .image = image,
                .subresourceRange = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                }
        };
        vkCmdPipelineBarrier(
                _buffer,
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
    }


} // crucible