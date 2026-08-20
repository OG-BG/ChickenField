/* LEFA Engine : vk_manager.h
2025 SILD Team
*/

#ifndef VK_MANAGER_H
#define VK_MANAGER_H

#ifdef __WATCOMC__
	#include<stdbool.h>
#endif

#define LOAD_VKHEADER

#ifdef LOAD_VKHEADER
    #include<3rdparty/vulkan/vulkan/vulkan.h>
    #include<3rdparty/vulkan/vulkan/vk_sdk_platform.h>
#endif







#include<core/window_manager.h>


int  VK_Init(LE_Display *display);
void VK_Update(LE_Display *display);
void VK_SetVSync(int interval);
void VK_Cleanup(LE_Display *display);









#endif /* VK_MANAGER_H */
