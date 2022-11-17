#include "Texture.h"
#include "../CrucibleLib.h"
namespace crucible
{
    VkImage Texture::handle()
    {
        return _handle;
    }

    VkImageView Texture::overview()
    {
        return _imageView;
    }

    std::shared_ptr<Texture> Texture::FromRaw(VkImage image, VkImageView view, unsigned int w, unsigned int h)
    {
        return std::shared_ptr<Texture>(new Texture(image,view,w,h));
    }

    Texture::Texture(VkImage image, VkImageView view, unsigned int w, unsigned int h)
    {
        _handle = image;
        _imageView = view;
        _width = w;
        _height = h;
    }

    Texture::~Texture()
    {
        vkDestroyImage(CrucibleLib::graphicsCard().device(),_handle, nullptr);
        vkDestroyImageView(CrucibleLib::graphicsCard().device(),_imageView, nullptr);
    }

    unsigned int Texture::width()
    {
        return _width;
    }

    unsigned int Texture::height()
    {
        return _height;
    }
} // crucible