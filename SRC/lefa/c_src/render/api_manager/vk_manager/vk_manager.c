/* vk_manager.c : OpenGL Values and Functions manager
2025 SILD Team */


#define _VULKAN
#include<render/api_manager/vk_manager.h>
#include"object_type_string_helper.h"

#include<stdio.h>

/* Allow a maximum of two outstanding presentation operations. */
#define FRAME_LAG 2

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#if defined(NDEBUG) && defined(__GNUC__)
    #define U_ASSERT_ONLY __attribute__((unused))
#else
    #define U_ASSERT_ONLY
#endif

typedef struct
{

    VkSurfaceKHR                                    surface;

    bool                                            separate_present_queue;

    int32_t                                         gpu_number;

    bool                                            VK_KHR_incremental_present_enabled;
    bool                                            VK_GOOGLE_display_timing_enabled;

    VkInstance                                      instance;
    VkPhysicalDevice                                gpu;
    VkDevice                                        device;
    VkQueue                                         graphics_queue;
    VkQueue                                         present_queue;
    uint32_t                                        graphics_queue_family_index;
    uint32_t                                        present_queue_family_index;
    VkSemaphore                                     image_acquired_semaphores[FRAME_LAG];
    VkSemaphore                                     draw_complete_semaphores[FRAME_LAG];
    VkSemaphore                                     image_ownership_semaphores[FRAME_LAG];
    VkPhysicalDeviceProperties                      gpu_props;
    VkQueueFamilyProperties                         *queue_props;
    VkPhysicalDeviceMemoryProperties                memory_properties;



    uint32_t                                        enabled_extension_count;
    uint32_t                                        enabled_layer_count;
    char                                            *extension_names[64];
    char                                            *enabled_layers[64];

    VkFormat                                        format;
    VkColorSpaceKHR                                 color_space;

    PFN_vkGetPhysicalDeviceSurfaceSupportKHR        fpGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR   fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR        fpGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR   fpGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkCreateSwapchainKHR                        fpCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR                       fpDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR                     fpGetSwapchainImagesKHR;
    PFN_vkAcquireNextImageKHR                       fpAcquireNextImageKHR;
    PFN_vkQueuePresentKHR                           fpQueuePresentKHR;
    PFN_vkGetRefreshCycleDurationGOOGLE             fpGetRefreshCycleDurationGOOGLE;
    PFN_vkGetPastPresentationTimingGOOGLE           fpGetPastPresentationTimingGOOGLE;
    uint32_t                                        swapchainImageCount;
    VkSwapchainKHR                                  swapchain;
    //SwapchainImageResources                         *swapchain_image_resources;
    VkPresentModeKHR                                presentMode;
    VkFence                                         fences[FRAME_LAG];
    int                                             frame_index;


    bool quit;
    int32_t curFrame;
    int32_t frameCount;
    bool validate;
    bool validate_checks_disabled;
    bool use_break;
    bool suppress_popups;

    PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT;
    PFN_vkSubmitDebugUtilsMessageEXT SubmitDebugUtilsMessageEXT;
    PFN_vkCmdBeginDebugUtilsLabelEXT CmdBeginDebugUtilsLabelEXT;
    PFN_vkCmdEndDebugUtilsLabelEXT CmdEndDebugUtilsLabelEXT;
    PFN_vkCmdInsertDebugUtilsLabelEXT CmdInsertDebugUtilsLabelEXT;
    PFN_vkSetDebugUtilsObjectNameEXT SetDebugUtilsObjectNameEXT;
    VkDebugUtilsMessengerEXT dbg_messenger;

    uint32_t current_buffer;
    uint32_t queue_family_count;


} LE_VKGPR;

LE_VKGPR *vkApp;

static int validation_error = 0;

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                                                              \
    {                                                                                                         \
        vkGPR->fp##entrypoint = (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint);            \
        if(vkGPR->fp##entrypoint == NULL)                                                                     \
        {                                                                                                     \
            ERR_EXIT("vkGetInstanceProcAddr failed to find vk" #entrypoint, "vkGetInstanceProcAddr Failure"); \
        }                                                                                                     \
    }

static PFN_vkGetDeviceProcAddr g_gdpa = NULL;

#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                                                                           \
    {                                                                                                                   \
        if(!g_gdpa) g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(vkGPR->instance, "vkGetDeviceProcAddr");    \
        vkGPR->fp##entrypoint = (PFN_vk##entrypoint)g_gdpa(dev, "vk" #entrypoint);                                      \
        if(vkGPR->fp##entrypoint == NULL)                                                                               \
        {                                                                                                               \
            ERR_EXIT("vkGetDeviceProcAddr failed to find vk" #entrypoint, "vkGetDeviceProcAddr Failure");               \
        }                                                                                                               \
    }

#if defined(_WIN32)
    #undef APIENTRY

    #include<windows.h>
    #include<assert.h>
    
    bool in_callback = false;
    #define ERR_EXIT(err_msg, err_class)                                              \
        do {                                                                          \
            if (!vkGPR->suppress_popups) MessageBox(NULL, err_msg, err_class, MB_OK); \
            exit(1);                                                                  \
        } while (0)


void DbgMsg(char *fmt, ...) 
{
    va_list va;
    va_start(va, fmt);
    vprintf(fmt, va);
    va_end(va);
    fflush(stdout);
}



VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT             messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT  *pCallbackData,
                                                        void                                        *pUserData) 
{
    char        prefix[64] = "";
    char        *message = (char *)malloc(strlen(pCallbackData->pMessage) + 5000);
    LE_VKGPR    *demo = (LE_VKGPR *)pUserData;
    uint32_t    object;
    uint32_t    cmd_buf_label;
    


    assert(message);

    if(demo->use_break) 
    {
        #define WIN32

        #ifndef WIN32
            raise(SIGTRAP);
        #else
            DebugBreak();
        #endif
    }

    if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) 
    {
        strcat(prefix, "VERBOSE : ");
    } 
    else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) 
    {
        strcat(prefix, "INFO : ");
    } 
    else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) 
    {
        strcat(prefix, "WARNING : ");
    } 
    else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) 
    {
        strcat(prefix, "ERROR : ");
    }

    if(messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) 
    {
        strcat(prefix, "GENERAL");
    } 
    else 
    {
        if(messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) 
        {
            strcat(prefix, "VALIDATION");
            validation_error = 1;
        }
        if(messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) 
        {
            if(messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) 
            {
                strcat(prefix, "|");
            }
            strcat(prefix, "PERFORMANCE");
        }
    }

    sprintf(message, 
            "%s - Message Id Number: %d | Message Id Name: %s\n\t%s\n", 
            prefix, 
            pCallbackData->messageIdNumber,
            pCallbackData->pMessageIdName, 
            pCallbackData->pMessage);

    if(pCallbackData->objectCount > 0) 
    {
        char tmp_message[500];
        sprintf(tmp_message, "\n\tObjects - %d\n", pCallbackData->objectCount);
        strcat(message, tmp_message);
        for(object = 0; object < pCallbackData->objectCount; ++object) 
        {
            if(NULL != pCallbackData->pObjects[object].pObjectName && 
                strlen(pCallbackData->pObjects[object].pObjectName) > 0) 
            {
                sprintf(tmp_message, 
                        "\t\tObject[%d] - %s, Handle %p, Name \"%s\"\n", 
                        object,
                        string_VkObjectType(pCallbackData->pObjects[object].objectType),
                        /*(void *)(pCallbackData->pObjects[object].objectHandle), 
                        */
                        (void *)(uintptr_t)(pCallbackData->pObjects[object].objectHandle),
                        pCallbackData->pObjects[object].pObjectName);
            } 
            else 
            {
                sprintf(tmp_message, 
                        "\t\tObject[%d] - %s, Handle %p\n", 
                        object,
                        string_VkObjectType(pCallbackData->pObjects[object].objectType),
                        /*(void *)(pCallbackData->pObjects[object].objectHandle));
                        */
                        (void *)(uintptr_t)(pCallbackData->pObjects[object].objectHandle));
            }
            strcat(message, tmp_message);
        }
    }
    if (pCallbackData->cmdBufLabelCount > 0) 
    {
        char tmp_message[500];
        sprintf(tmp_message, "\n\tCommand Buffer Labels - %d\n", pCallbackData->cmdBufLabelCount);
        strcat(message, tmp_message);
        for(cmd_buf_label = 0; cmd_buf_label < pCallbackData->cmdBufLabelCount; ++cmd_buf_label) 
        {
            sprintf(tmp_message, 
                    "\t\tLabel[%d] - %s { %f, %f, %f, %f}\n", 
                    cmd_buf_label,
                    pCallbackData->pCmdBufLabels[cmd_buf_label].pLabelName, 
                    pCallbackData->pCmdBufLabels[cmd_buf_label].color[0],
                    pCallbackData->pCmdBufLabels[cmd_buf_label].color[1], 
                    pCallbackData->pCmdBufLabels[cmd_buf_label].color[2],
                    pCallbackData->pCmdBufLabels[cmd_buf_label].color[3]);
            strcat(message, tmp_message);
        }
    }

#ifdef _WIN32

    in_callback = true;
    if (!demo->suppress_popups) MessageBox(NULL, message, "Alert", MB_OK);
    printf("\n ================== \n %s \n ================= \n", message);
    in_callback = false;


#else

    printf("%s\n", message);
    fflush(stdout);

#endif

    free(message);

    /* Don't bail out, but keep going. */
    return false;
}

    /*
    * Return 1 (true) if all layer names specified in check_names
    * can be found in given layer properties.
    */
    static VkBool32 gpr_check_layers(uint32_t          check_count, 
                                     char              **check_names, 
                                     uint32_t          layer_count, 
                                     VkLayerProperties *layers) 
    {
        uint32_t i;
        uint32_t j;

        for(i = 0; i < check_count; i++) 
        {
            VkBool32 found = 0;
            for(j = 0; j < layer_count; j++) 
            {
                if (!strcmp(check_names[i], layers[j].layerName)) 
                {
                    found = 1;
                    break;
                }
            }
            if (!found) 
            {
                fprintf(stderr, "Cannot find layer: %s\n", check_names[i]);
                return 0;
            }
        }
        return 1;
    }

    void vk_Init(LE_VKGPR *vkGPR) 
    {
        VkResult                            err;
        uint32_t                            instance_extension_count = 0;
        uint32_t                            instance_layer_count = 0;
        char                                *instance_validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
        VkBool32                            validation_found = 0;
        VkBool32                            surfaceExtFound = 0;
        VkBool32                            platformSurfaceExtFound = 0;
        uint32_t                            i;
        VkApplicationInfo                   appInfo = {0};
        VkInstanceCreateInfo                createInfo = {0};
        VkDebugUtilsMessengerCreateInfoEXT  dbg_messenger_create_info;
        uint32_t                            gpu_count = 0;
        VkPhysicalDevice                    *physical_devices;
        VkPhysicalDeviceProperties          physicalDeviceProperties;
        VkPhysicalDeviceType                search_for_device_type;
        uint32_t                            device_extension_count = 0;
        VkBool32                            swapchainExtFound = 0;
        VkPhysicalDeviceFeatures            physDevFeatures;

        vkGPR->enabled_extension_count = 0;

        if(vkGPR->validate)
        {
            err = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
            assert(!err);

            if(instance_layer_count > 0) 
            {
                VkLayerProperties *instance_layers = malloc(sizeof(VkLayerProperties) * instance_layer_count);
                err = vkEnumerateInstanceLayerProperties(&instance_layer_count, instance_layers);
                assert(!err);

                validation_found = gpr_check_layers(ARRAY_SIZE(instance_validation_layers), 
                                                    instance_validation_layers,
                                                    instance_layer_count, instance_layers);
                if(validation_found) 
                {
                    vkGPR->enabled_layer_count = ARRAY_SIZE(instance_validation_layers);
                    vkGPR->enabled_layers[0] = "VK_LAYER_KHRONOS_validation";
                }
                free(instance_layers);
            }

            if (!validation_found) 
            {
                ERR_EXIT("vkEnumerateInstanceLayerProperties failed to find required validation layer.\n\n"
                         "Please look at the Getting Started guide for additional information.\n",
                         "vkCreateInstance Failure");
            }

        }

        /* Look for instance extensions */
        memset(vkGPR->extension_names, 0, sizeof(vkGPR->extension_names));

        err = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, NULL);
        assert(!err);

        if(instance_extension_count > 0) 
        {
            VkExtensionProperties *instance_extensions = malloc(sizeof(VkExtensionProperties) * instance_extension_count);
            err = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, instance_extensions);
            assert(!err);

            for(i = 0; i < instance_extension_count; i++) 
            {
                if(!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) 
                {
                    surfaceExtFound = 1;
                    vkGPR->extension_names[vkGPR->enabled_extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
                }

                if(!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) 
                {
                    platformSurfaceExtFound = 1;
                    vkGPR->extension_names[vkGPR->enabled_extension_count++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
                }

                if(!strcmp(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, instance_extensions[i].extensionName)) 
                {
                    vkGPR->extension_names[vkGPR->enabled_extension_count++] = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
                }
                if(!strcmp(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, instance_extensions[i].extensionName)) 
                {
                    if(vkGPR->validate) 
                    {
                        vkGPR->extension_names[vkGPR->enabled_extension_count++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
                    }
                }
                assert(vkGPR->enabled_extension_count < 64);
            }

            free(instance_extensions);
        }

        if (!surfaceExtFound) 
        {
            ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_SURFACE_EXTENSION_NAME
                     " extension.\n\n"
                     "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
                     "Please look at the Getting Started guide for additional information.\n",
                     "vkCreateInstance Failure");
        }
    
        if (!platformSurfaceExtFound) 
        {
#if defined(VK_USE_PLATFORM_WIN32_KHR)
            ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find the " VK_KHR_WIN32_SURFACE_EXTENSION_NAME
                     " extension.\n\n"
                     "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
                     "Please look at the Getting Started guide for additional information.\n",
                     "vkCreateInstance Failure");
#endif
        }

        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = NULL;
        appInfo.pApplicationName = "LEFA_VKGPR";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "LITHE ENGINE";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        memset(&createInfo, 0, sizeof(VkInstanceCreateInfo));
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pNext = NULL;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = vkGPR->enabled_layer_count;
        createInfo.ppEnabledLayerNames = (const char *const *)instance_validation_layers;
        createInfo.enabledExtensionCount = vkGPR->enabled_extension_count;
        createInfo.ppEnabledExtensionNames = (const char *const *)vkGPR->extension_names;

    /*
     * This is info for a temp callback to use during CreateInstance.
     * After the instance is created, we use the instance-based
     * function to register the final callback.
     */
        if(vkGPR->validate) 
        {
            /* VK_EXT_debug_utils style */
            dbg_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            dbg_messenger_create_info.pNext = NULL;
            dbg_messenger_create_info.flags = 0;
            dbg_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        
            dbg_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
                                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
                                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        
            dbg_messenger_create_info.pfnUserCallback = debug_messenger_callback;
            dbg_messenger_create_info.pUserData = vkGPR;
            createInfo.pNext = &dbg_messenger_create_info;
        }

        err = vkCreateInstance(&createInfo, NULL, &vkGPR->instance);
        if (err == VK_ERROR_INCOMPATIBLE_DRIVER) 
        {
            ERR_EXIT("Cannot find a compatible Vulkan installable client driver (ICD).\n\n"
                     "Please look at the Getting Started guide for additional information.\n",
                     "vkCreateInstance Failure");
        } 
        else if (err == VK_ERROR_EXTENSION_NOT_PRESENT) 
        {
            ERR_EXIT("Cannot find a specified extension library.\n"
                     "Make sure your layers path is set appropriately.\n",
                     "vkCreateInstance Failure");
        } 
        else if (err) 
        {
            ERR_EXIT("vkCreateInstance failed.\n\n"
                     "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
                     "Please look at the Getting Started guide for additional information.\n",
                     "vkCreateInstance Failure");
        }

        /* Make initial call to query gpu_count, then second call for gpu info */
        err = vkEnumeratePhysicalDevices(vkGPR->instance, &gpu_count, NULL);
        assert(!err);

        if(gpu_count <= 0) 
        {
            ERR_EXIT("vkEnumeratePhysicalDevices reported zero accessible devices.\n\n"
                     "Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
                     "Please look at the Getting Started guide for additional information.\n",
                     "vkEnumeratePhysicalDevices Failure");
        }

        physical_devices = malloc(sizeof(VkPhysicalDevice) * gpu_count);
        err = vkEnumeratePhysicalDevices(vkGPR->instance, &gpu_count, physical_devices);
        assert(!err);

        if(vkGPR->gpu_number >= 0 && !((uint32_t)vkGPR->gpu_number < gpu_count)) 
        {
            fprintf(stderr, "GPU %d specified is not present, GPU count = %u\n", vkGPR->gpu_number, gpu_count);
            ERR_EXIT("Specified GPU number is not present", "User Error");
        }

        /* Try to auto select most suitable device */
        if(vkGPR->gpu_number == -1) 
        {
            uint32_t count_device_type[VK_PHYSICAL_DEVICE_TYPE_CPU + 1];
            memset(count_device_type, 0, sizeof(count_device_type));

            for (i = 0; i < gpu_count; i++) 
            {
                vkGetPhysicalDeviceProperties(physical_devices[i], &physicalDeviceProperties);
                assert(physicalDeviceProperties.deviceType <= VK_PHYSICAL_DEVICE_TYPE_CPU);
                count_device_type[physicalDeviceProperties.deviceType]++;
            }

            search_for_device_type = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            if(count_device_type[VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU]) 
            {
                search_for_device_type = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            } 
            else if(count_device_type[VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU]) 
            {
                search_for_device_type = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
            } 
            else if(count_device_type[VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU]) 
            {
                search_for_device_type = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
            } 
            else if(count_device_type[VK_PHYSICAL_DEVICE_TYPE_CPU]) 
            {
                search_for_device_type = VK_PHYSICAL_DEVICE_TYPE_CPU;
            } 
            else if(count_device_type[VK_PHYSICAL_DEVICE_TYPE_OTHER]) 
            {
                search_for_device_type = VK_PHYSICAL_DEVICE_TYPE_OTHER;
            }

            for(i = 0; i < gpu_count; i++) 
            {
                vkGetPhysicalDeviceProperties(physical_devices[i], &physicalDeviceProperties);
                if(physicalDeviceProperties.deviceType == search_for_device_type) 
                {
                    vkGPR->gpu_number = i;
                    break;
                }
            }
        }

        assert(vkGPR->gpu_number >= 0);
        vkGPR->gpu = physical_devices[vkGPR->gpu_number];
        {
            VkPhysicalDeviceProperties physicalDeviceProperties;
            vkGetPhysicalDeviceProperties(vkGPR->gpu, &physicalDeviceProperties);
            fprintf(stderr, 
                    "Selected GPU %d: %s, type: %u\n", 
                    vkGPR->gpu_number, 
                    physicalDeviceProperties.deviceName,
                    physicalDeviceProperties.deviceType);
        }
        free(physical_devices);

        /* Look for device extensions */
        vkGPR->enabled_extension_count = 0;
        memset(vkGPR->extension_names, 0, sizeof(vkGPR->extension_names));

        err = vkEnumerateDeviceExtensionProperties(vkGPR->gpu, NULL, &device_extension_count, NULL);
        assert(!err);

        if(device_extension_count > 0) 
        {
            VkExtensionProperties *device_extensions = malloc(sizeof(VkExtensionProperties) * device_extension_count);
            err = vkEnumerateDeviceExtensionProperties(vkGPR->gpu, NULL, &device_extension_count, device_extensions);
            assert(!err);

            for(i = 0; i < device_extension_count; i++) 
            {
                if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, device_extensions[i].extensionName)) 
                {
                    swapchainExtFound = 1;
                    vkGPR->extension_names[vkGPR->enabled_extension_count++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
                }
                if (!strcmp("VK_KHR_portability_subset", device_extensions[i].extensionName)) 
                {
                    vkGPR->extension_names[vkGPR->enabled_extension_count++] = "VK_KHR_portability_subset";
                }
                assert(vkGPR->enabled_extension_count < 64);
            }

            if(vkGPR->VK_KHR_incremental_present_enabled) 
            {
                /*  Even though the user "enabled" the extension via the command
                    line, we must make sure that it's enumerated for use with the
                    device.  Therefore, disable it here, and re-enable it again if
                    enumerated.
                */
                vkGPR->VK_KHR_incremental_present_enabled = false;
                for(i = 0; i < device_extension_count; i++) 
                {
                    if(!strcmp(VK_KHR_INCREMENTAL_PRESENT_EXTENSION_NAME, device_extensions[i].extensionName)) 
                    {
                        vkGPR->extension_names[vkGPR->enabled_extension_count++] = VK_KHR_INCREMENTAL_PRESENT_EXTENSION_NAME;
                        vkGPR->VK_KHR_incremental_present_enabled = true;
                        DbgMsg("VK_KHR_incremental_present extension enabled\n");
                    }
                    assert(vkGPR->enabled_extension_count < 64);
                }
                if(!vkGPR->VK_KHR_incremental_present_enabled) 
                {
                    DbgMsg("VK_KHR_incremental_present extension NOT AVAILABLE\n");
                }
            }

            if(vkGPR->VK_GOOGLE_display_timing_enabled) 
            {
                /*  Even though the user "enabled" the extension via the command
                    line, we must make sure that it's enumerated for use with the
                    device.  Therefore, disable it here, and re-enable it again if
                    enumerated.
                */
                vkGPR->VK_GOOGLE_display_timing_enabled = false;
                for(i = 0; i < device_extension_count; i++) 
                {
                    if(!strcmp(VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME, device_extensions[i].extensionName)) 
                    {
                        vkGPR->extension_names[vkGPR->enabled_extension_count++] = VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME;
                        vkGPR->VK_GOOGLE_display_timing_enabled = true;
                        DbgMsg("VK_GOOGLE_display_timing extension enabled\n");
                    }
                    assert(vkGPR->enabled_extension_count < 64);
                }
                if(!vkGPR->VK_GOOGLE_display_timing_enabled) 
                {
                    DbgMsg("VK_GOOGLE_display_timing extension NOT AVAILABLE\n");
                }
            }

            free(device_extensions);
        }

        if(!swapchainExtFound) 
        {
            ERR_EXIT("vkEnumerateDeviceExtensionProperties failed to find the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
                 " extension.\n\nDo you have a compatible Vulkan installable client driver (ICD) installed?\n"
                 "Please look at the Getting Started guide for additional information.\n",
                 "vkCreateInstance Failure");
        }

        if(vkGPR->validate) 
        {
            /*  Setup VK_EXT_debug_utils function pointers always (we use them for
                debug labels and names).
            */
            vkGPR->CreateDebugUtilsMessengerEXT =
                (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkGPR->instance, "vkCreateDebugUtilsMessengerEXT");
            
            vkGPR->DestroyDebugUtilsMessengerEXT =
                (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkGPR->instance, "vkDestroyDebugUtilsMessengerEXT");
            
            vkGPR->SubmitDebugUtilsMessageEXT =
                (PFN_vkSubmitDebugUtilsMessageEXT)vkGetInstanceProcAddr(vkGPR->instance, "vkSubmitDebugUtilsMessageEXT");
            
            vkGPR->CmdBeginDebugUtilsLabelEXT =
                (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(vkGPR->instance, "vkCmdBeginDebugUtilsLabelEXT");
        
            vkGPR->CmdEndDebugUtilsLabelEXT =
                (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(vkGPR->instance, "vkCmdEndDebugUtilsLabelEXT");
            
            vkGPR->CmdInsertDebugUtilsLabelEXT =
                (PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(vkGPR->instance, "vkCmdInsertDebugUtilsLabelEXT");
            
            vkGPR->SetDebugUtilsObjectNameEXT =
                (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(vkGPR->instance, "vkSetDebugUtilsObjectNameEXT");
        
            if(NULL == vkGPR->CreateDebugUtilsMessengerEXT || NULL == vkGPR->DestroyDebugUtilsMessengerEXT  ||
               NULL == vkGPR->SubmitDebugUtilsMessageEXT   || NULL == vkGPR->CmdBeginDebugUtilsLabelEXT     ||
               NULL == vkGPR->CmdEndDebugUtilsLabelEXT     || NULL == vkGPR->CmdInsertDebugUtilsLabelEXT    ||
               NULL == vkGPR->SetDebugUtilsObjectNameEXT) 
            {
                ERR_EXIT("GetProcAddr: Failed to init VK_EXT_debug_utils\n", "GetProcAddr: Failure");
            }

            err = vkGPR->CreateDebugUtilsMessengerEXT(vkGPR->instance, &dbg_messenger_create_info, NULL, &vkGPR->dbg_messenger);
            switch(err) 
            {
                case VK_SUCCESS:
                    break;
                case VK_ERROR_OUT_OF_HOST_MEMORY:
                    ERR_EXIT("CreateDebugUtilsMessengerEXT: out of host memory\n", "CreateDebugUtilsMessengerEXT Failure");
                    break;
                default:
                    ERR_EXIT("CreateDebugUtilsMessengerEXT: unknown failure\n", "CreateDebugUtilsMessengerEXT Failure");
                    break;
            }
        }
        vkGetPhysicalDeviceProperties(vkGPR->gpu, &vkGPR->gpu_props);

        /* Call with NULL data to get count */
        vkGetPhysicalDeviceQueueFamilyProperties(vkGPR->gpu, &vkGPR->queue_family_count, NULL);
        assert(vkGPR->queue_family_count >= 1);

        vkGPR->queue_props = (VkQueueFamilyProperties *)malloc(vkGPR->queue_family_count * sizeof(VkQueueFamilyProperties));
        vkGetPhysicalDeviceQueueFamilyProperties(vkGPR->gpu, &vkGPR->queue_family_count, vkGPR->queue_props);

        /* Query fine-grained feature support for this device.
            If app has specific feature requirements it should check supported
            features based on this query
        */
        vkGetPhysicalDeviceFeatures(vkGPR->gpu, &physDevFeatures);

        GET_INSTANCE_PROC_ADDR(vkGPR->instance, GetPhysicalDeviceSurfaceSupportKHR);
        GET_INSTANCE_PROC_ADDR(vkGPR->instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
        GET_INSTANCE_PROC_ADDR(vkGPR->instance, GetPhysicalDeviceSurfaceFormatsKHR);
        GET_INSTANCE_PROC_ADDR(vkGPR->instance, GetPhysicalDeviceSurfacePresentModesKHR);
        GET_INSTANCE_PROC_ADDR(vkGPR->instance, GetSwapchainImagesKHR);

    }

    void vk_create_device(LE_VKGPR *vkGPR) 
    {
        VkResult U_ASSERT_ONLY err;
        VkDeviceCreateInfo device;
        float queue_priorities[1] = {0.0};
        VkDeviceQueueCreateInfo queues[2];
    
        queues[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queues[0].pNext = NULL;
        queues[0].queueFamilyIndex = vkGPR->graphics_queue_family_index;
        queues[0].queueCount = 1;
        queues[0].pQueuePriorities = queue_priorities;
        queues[0].flags = 0;

        memset(&device, 0, sizeof(device));
        device.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device.pNext = NULL;
        device.queueCreateInfoCount = 1;
        device.pQueueCreateInfos = queues;
        device.enabledLayerCount = 0;
        device.ppEnabledLayerNames = NULL;
        device.enabledExtensionCount = vkGPR->enabled_extension_count;
        device.ppEnabledExtensionNames = (const char *const *)vkGPR->extension_names;
        device.pEnabledFeatures = NULL;  /* Se recursos específicos são necessários, passe-os aqui */


        if(vkGPR->separate_present_queue) 
        {
            queues[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queues[1].pNext = NULL;
            queues[1].queueFamilyIndex = vkGPR->present_queue_family_index;
            queues[1].queueCount = 1;
            queues[1].pQueuePriorities = queue_priorities;
            queues[1].flags = 0;
            device.queueCreateInfoCount = 2;
        }
        err = vkCreateDevice(vkGPR->gpu, &device, NULL, &vkGPR->device);
        assert(!err);
    }

    static void vk_create_surface(LE_VKGPR *vkGPR, LE_Display *display0) 
    {
        VkResult                U_ASSERT_ONLY err;
        uint32_t                i = 0;
        VkWin32SurfaceCreateInfoKHR createInfo;
        
        /* Create a WSI surface for the window: */
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.pNext = NULL;
        createInfo.flags = 0;
        createInfo.hinstance = display0->platformConnection;
        createInfo.hwnd = display0->platformHandle;

        err = vkCreateWin32SurfaceKHR(vkGPR->instance, &createInfo, NULL, &vkGPR->surface);


        assert(!err);
    }

    VkSurfaceFormatKHR pick_surface_format(const VkSurfaceFormatKHR *surfaceFormats, 
                                            uint32_t count) 
    {
        uint32_t i;

        /* Prefer non-SRGB formats... */
        for(i = 0; i < count; i++) 
        {
            const VkFormat format = surfaceFormats[i].format;

            if (format == VK_FORMAT_R8G8B8A8_UNORM           || 
                format == VK_FORMAT_B8G8R8A8_UNORM           ||
                format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 || 
                format == VK_FORMAT_A2R10G10B10_UNORM_PACK32 ||
                format == VK_FORMAT_R16G16B16A16_SFLOAT) 
            {
                return surfaceFormats[i];
            }
        }

        printf("Can't find our preferred formats... Falling back to first exposed format. Rendering may be incorrect.\n");

        assert(count >= 1);
        return surfaceFormats[0];
    }

    void vk_Init_Swapchain(LE_VKGPR *vkGPR, LE_Display *display0)
    {
        VkResult                U_ASSERT_ONLY err;
        VkBool32                *supportsPresent;
        uint32_t                i;
        uint32_t                graphicsQueueFamilyIndex = UINT32_MAX;
        uint32_t                presentQueueFamilyIndex = UINT32_MAX;
        uint32_t                formatCount;
        VkSurfaceFormatKHR      *surfFormats;
        VkSurfaceFormatKHR      surfaceFormat;
        VkSemaphoreCreateInfo   semaphoreCreateInfo;
        VkFenceCreateInfo       fence_ci;

        vk_create_surface(vkGPR, display0);

        /* Iterate over each queue to learn whether it supports presenting: */
        supportsPresent = (VkBool32 *)malloc(vkGPR->queue_family_count * sizeof(VkBool32));
        for(i = 0; i < vkGPR->queue_family_count; i++) 
        {
            vkGPR->fpGetPhysicalDeviceSurfaceSupportKHR(vkGPR->gpu, i, vkGPR->surface, &supportsPresent[i]);
        }

        /*  Search for a graphics and a present queue in the array of queue 
            families, try to find one that supports both 
        */
        for(i = 0; i < vkGPR->queue_family_count; i++) 
        {
            if((vkGPR->queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) 
            {
                if(graphicsQueueFamilyIndex == UINT32_MAX) 
                {
                    graphicsQueueFamilyIndex = i;
                }

                if(supportsPresent[i] == VK_TRUE) 
                {
                    graphicsQueueFamilyIndex = i;
                    presentQueueFamilyIndex = i;
                    break;
                }
            }
        }

        if(presentQueueFamilyIndex == UINT32_MAX) 
        {
            /*  If didn't find a queue that supports both graphics and present, then
                find a separate present queue.
            */
            for(i = 0; i < vkGPR->queue_family_count; ++i) 
            {
                if(supportsPresent[i] == VK_TRUE) 
                {
                    presentQueueFamilyIndex = i;
                    break;
                }
            }
        }

        /* Generate error if could not find both a graphics and a present queue */
        if(graphicsQueueFamilyIndex == UINT32_MAX || presentQueueFamilyIndex == UINT32_MAX) 
        {
            ERR_EXIT("Could not find both graphics and present queues\n", "Swapchain Initialization Failure");
        }

        vkGPR->graphics_queue_family_index = graphicsQueueFamilyIndex;
        vkGPR->present_queue_family_index = presentQueueFamilyIndex;
        vkGPR->separate_present_queue = (vkGPR->graphics_queue_family_index != vkGPR->present_queue_family_index);
        free(supportsPresent);

        vk_create_device(vkGPR);

        GET_DEVICE_PROC_ADDR(vkGPR->device, CreateSwapchainKHR);
        GET_DEVICE_PROC_ADDR(vkGPR->device, DestroySwapchainKHR);
        GET_DEVICE_PROC_ADDR(vkGPR->device, GetSwapchainImagesKHR);
        GET_DEVICE_PROC_ADDR(vkGPR->device, AcquireNextImageKHR);
        GET_DEVICE_PROC_ADDR(vkGPR->device, QueuePresentKHR);
        if (vkGPR->VK_GOOGLE_display_timing_enabled) 
        {
            GET_DEVICE_PROC_ADDR(vkGPR->device, GetRefreshCycleDurationGOOGLE);
            GET_DEVICE_PROC_ADDR(vkGPR->device, GetPastPresentationTimingGOOGLE);
        }

        vkGetDeviceQueue(vkGPR->device, vkGPR->graphics_queue_family_index, 0, &vkGPR->graphics_queue);

        if (!vkGPR->separate_present_queue) 
        {
            vkGPR->present_queue = vkGPR->graphics_queue;
        } 
        else 
        {
            vkGetDeviceQueue(vkGPR->device, vkGPR->present_queue_family_index, 0, &vkGPR->present_queue);
        }

        /* Get the list of VkFormat's that are supported: */
        err = vkGPR->fpGetPhysicalDeviceSurfaceFormatsKHR(vkGPR->gpu, vkGPR->surface, &formatCount, NULL);
        assert(!err);
        surfFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
        err = vkGPR->fpGetPhysicalDeviceSurfaceFormatsKHR(vkGPR->gpu, vkGPR->surface, &formatCount, surfFormats);
        assert(!err);
        surfaceFormat = pick_surface_format(surfFormats, formatCount);
        vkGPR->format = surfaceFormat.format;
        vkGPR->color_space = surfaceFormat.colorSpace;
        free(surfFormats);

        vkGPR->quit = false;
        vkGPR->curFrame = 0;

        /* Create semaphores to synchronize acquiring presentable buffers before
           rendering and waiting for drawing to be complete before presenting */

        memset(&semaphoreCreateInfo, 0, sizeof(semaphoreCreateInfo));
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.pNext = NULL;
        semaphoreCreateInfo.flags = 0;


        /* Create fences that we can use to throttle if we get too far
           ahead of the image presents
        */

        memset(&fence_ci, 0, sizeof(fence_ci));
        fence_ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_ci.pNext = NULL;
        fence_ci.flags = VK_FENCE_CREATE_SIGNALED_BIT;


        for(i = 0; i < FRAME_LAG; i++) 
        {
            err = vkCreateFence(vkGPR->device, &fence_ci, NULL, &vkGPR->fences[i]);
            assert(!err);

            err = vkCreateSemaphore(vkGPR->device, &semaphoreCreateInfo, NULL, &vkGPR->image_acquired_semaphores[i]);
            assert(!err);

            err = vkCreateSemaphore(vkGPR->device, &semaphoreCreateInfo, NULL, &vkGPR->draw_complete_semaphores[i]);
            assert(!err);

            if (vkGPR->separate_present_queue) 
            {
                err = vkCreateSemaphore(vkGPR->device, &semaphoreCreateInfo, NULL, &vkGPR->image_ownership_semaphores[i]);
                assert(!err);
            }
        }
        vkGPR->frame_index = 0;

        /* Get Memory information and properties */
        vkGetPhysicalDeviceMemoryProperties(vkGPR->gpu, &vkGPR->memory_properties);

    }


    int VK_Init(LE_Display *display)
    {
        memset(vkApp, 0, sizeof(*vkApp));
        vkApp->presentMode = VK_PRESENT_MODE_FIFO_KHR;
        vkApp->frameCount = INT32_MAX;
        /* Autodetect suitable / best GPU by default */
        vkApp->gpu_number = -1;

    	vk_Init(vkApp);   

        vk_Init_Swapchain(vkApp, display);

    	return 0;
    }

    void VK_Update(LE_Display *display)
    {

    }

    void VK_SetVSync(int interval) 
    {

    }

    void VK_Cleanup(LE_Display *display)
    {

    }

#endif /* _WIN32 */


















