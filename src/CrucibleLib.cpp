#include "CrucibleLib.h"
#include <SDL2/SDL.h>
#include <VkBootstrap.h>
#include <stdexcept>

namespace crucible
{
    bool _using_validation_layers;
    VkInstance _vulkan_instance = nullptr;
    VkDebugUtilsMessengerEXT _vulkan_debug_messenger;


    void CrucibleLib::init(bool includeValidationLayers)
    {
        if(SDL_Init(SDL_INIT_EVERYTHING))
        {
            throw std::runtime_error(SDL_GetError());
        }
        _using_validation_layers = includeValidationLayers;
        vkb::InstanceBuilder builder;
        auto instance_ret = builder.set_app_name("Crucible Application").request_validation_layers(_using_validation_layers).use_default_debug_messenger().build();
        if(!instance_ret.has_value())
        {
            throw std::runtime_error("Unable to initialize graphics");
        }
        _vulkan_instance = instance_ret->instance;
        _vulkan_debug_messenger = instance_ret->debug_messenger;

        vkb::PhysicalDeviceSelector selector{instance_ret.value()};
        vkb::PhysicalDevice physicalDevice = selector.set_minimum_version(1,3).defer_surface_initialization().require_dedicated_transfer_queue().select().value();
        vkb::DeviceBuilder deviceBuilder {physicalDevice};
        vkb::Device vkbDevice = deviceBuilder.build().value();
        _graphicsCard.init(vkbDevice);
    }

    void CrucibleLib::cleanup()
    {
        _graphicsCard.cleanup();
        if(_vulkan_debug_messenger)
        {
            vkb::destroy_debug_utils_messenger(_vulkan_instance, _vulkan_debug_messenger);
        }
        vkDestroyInstance(_vulkan_instance, nullptr);
    }

    VkInstance CrucibleLib::vulkanInstance()
    {
        return _vulkan_instance;
    }

    GraphicsCard &CrucibleLib::graphicsCard()
    {
        return _graphicsCard;
    }

} // crucible