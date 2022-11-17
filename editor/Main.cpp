#include <CrucibleLib.h>
#include <Window.h>
#ifdef NDEBUG
bool debug = false;
#else
//TODO: must be false for now, the validation layers (VK_LAYER_KHRONOS_validation) segfaults on vkCmdBeginRendering. See https://bugs.archlinux.org/task/74091
bool debug = true;
#endif
int main(int argc, char* arv[])
{
    crucible::CrucibleLib::init(debug);
    auto& card = crucible::CrucibleLib::graphicsCard();
    auto window = crucible::Window::Create(500,500,"Crucible");
    window->show();
    crucible::CrucibleLib::cleanup();
}