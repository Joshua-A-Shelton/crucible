#ifndef CRUCIBLE_TEXTURE_H
#define CRUCIBLE_TEXTURE_H

#include <vulkan/vulkan.h>
#include <memory>
#include <glm/glm.hpp>
namespace crucible
{

    class Texture
    {
    private:
        VkImage _handle;
        VkImageView _imageView;
        unsigned int _width;
        unsigned int _height;
        VkFormat _format;
        Texture(int w, int h, VkFormat format, void* data);
        Texture(VkImage image, VkImageView view, unsigned int w, unsigned int h);
    public:
        ~Texture();
        static std::shared_ptr<Texture> Create(unsigned int w, unsigned int h, glm::vec3 color);
        static std::shared_ptr<Texture> Create(unsigned int w, unsigned int h, glm::vec4 color);
        static std::shared_ptr<Texture> Create(int w, int h, VkFormat format, void* data);
        static std::shared_ptr<Texture> FromRaw(VkImage image, VkImageView view, unsigned int w, unsigned int h);
        VkImage handle();
        VkImageView overview();
        unsigned int width();
        unsigned int height();
    };

} // crucible
#endif //CRUCIBLE_TEXTURE_H