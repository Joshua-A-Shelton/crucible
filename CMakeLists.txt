cmake_minimum_required(VERSION 3.24)
project(crucible)

set(CMAKE_CXX_STANDARD 17)
find_package(Vulkan REQUIRED)
set(SDL_STATIC ON CACHE BOOL "Build static SDL" FORCE)
set( SDL_SHARED OFF CACHE BOOL "Build shared SDL" FORCE )
add_subdirectory(third-party/sdl)
add_subdirectory(third-party/vk-bootstrap)
add_subdirectory(third-party/vma)
add_subdirectory(third-party/glm)

add_library(crucible-lib src/CrucibleLib.cpp src/CrucibleLib.h src/Window.cpp src/Window.h src/rendering/Swapchain.cpp src/rendering/Swapchain.h src/rendering/GraphicsCard.cpp src/rendering/GraphicsCard.h src/rendering/CommandBuffer.cpp src/rendering/CommandBuffer.h src/rendering/Framebuffer.cpp src/rendering/Framebuffer.h src/rendering/Texture.cpp src/rendering/Texture.h)
add_executable(crucible editor/Main.cpp)

target_include_directories(crucible-lib PUBLIC ${SDL2_INCLUDE_DIRS} third-party/vk-bootstrap/src third-party/vma/include third-party/glm)
target_link_libraries(crucible-lib ${Vulkan_LIBRARIES} SDL2main SDL2-static vk-bootstrap::vk-bootstrap VulkanMemoryAllocator glm)

target_include_directories(crucible PRIVATE src)
target_link_libraries(crucible crucible-lib)
