#ifndef SIMPLE_RENDER_H
#define SIMPLE_RENDER_H

#define VK_NO_PROTOTYPES

#include "../../render/scene_mgr.h"
#include "../../render/render_common.h"
#include "../../render/render_gui.h"
#include "../../../resources/shaders/common.h"
#include <geom/vk_mesh.h>
#include <vk_descriptor_sets.h>
#include <vk_fbuf_attachment.h>
#include <vk_images.h>
#include <vk_swapchain.h>
#include <string>
#include <iostream>

class SimpleRender : public IRender
{
public:
  const std::string VERTEX_SHADER_PATH = "../resources/shaders/simple.vert";
  const std::string FRAGMENT_SHADER_PATH = "../resources/shaders/simple.frag";
  const std::string FRUSTUM_CULLING_SHADER_PATH = "../resources/shaders/frustum_culling.comp";

  const std::string TRAJECTORY_SAVE_PATH = "trajectory.txt";

  SimpleRender(uint32_t a_width, uint32_t a_height);
  ~SimpleRender()  { Cleanup(); };

  inline uint32_t     GetWidth()      const override { return m_width; }
  inline uint32_t     GetHeight()     const override { return m_height; }
  inline VkInstance   GetVkInstance() const override { return m_instance; }
  void InitVulkan(const char** a_instanceExtensions, uint32_t a_instanceExtensionsCount, uint32_t a_deviceId) override;

  void InitPresentation(VkSurfaceKHR& a_surface, bool initGUI) override;

  void ProcessInput(const AppInput& input) override;
  void UpdateCamera(const Camera* cams, uint32_t a_camsCount) override;
  Camera GetCurrentCamera() override {return m_cam;}
  void UpdateView();

  void LoadScene(const char *path, bool transpose_inst_matrices) override;
  void DrawFrame(float a_time, DrawMode a_mode) override;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // debugging utils
  //
  static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(
    VkDebugReportFlagsEXT                       flags,
    VkDebugReportObjectTypeEXT                  objectType,
    uint64_t                                    object,
    size_t                                      location,
    int32_t                                     messageCode,
    const char* pLayerPrefix,
    const char* pMessage,
    void* pUserData)
  {
    (void)flags;
    (void)objectType;
    (void)object;
    (void)location;
    (void)messageCode;
    (void)pUserData;
    std::cout << pLayerPrefix << ": " << pMessage << std::endl;
    return VK_FALSE;
  }

  VkDebugReportCallbackEXT m_debugReportCallback = nullptr;
protected:

  VkInstance       m_instance       = VK_NULL_HANDLE;
  VkCommandPool    m_commandPool    = VK_NULL_HANDLE;
  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
  VkDevice         m_device         = VK_NULL_HANDLE;
  VkQueue          m_graphicsQueue  = VK_NULL_HANDLE;
  VkQueue          m_transferQueue  = VK_NULL_HANDLE;

  vk_utils::QueueFID_T m_queueFamilyIDXs {UINT32_MAX, UINT32_MAX, UINT32_MAX};

  struct
  {
    mat4 projView;
    vec4 bbox1;
    vec4 bbox2;
    uint instanceNum;
  } pushConstFC;

  struct
  {
    uint32_t    currentFrame      = 0u;
    VkQueue     queue             = VK_NULL_HANDLE;
    VkSemaphore imageAvailable    = VK_NULL_HANDLE;
    VkSemaphore renderingFinished = VK_NULL_HANDLE;
  } m_presentationResources;

  std::vector<VkFence> m_frameFences;
  std::vector<VkCommandBuffer> m_cmdBuffersDrawMain;

  struct
  {
    LiteMath::float4x4 projView;
    LiteMath::float4x4 model;
  } pushConst2M;

  UniformParams m_uniforms {};
  VkBuffer m_ubo = VK_NULL_HANDLE;
  VkDeviceMemory m_uboAlloc = VK_NULL_HANDLE;
  void* m_uboMappedMem = nullptr;

  pipeline_data_t m_basicForwardPipeline {};
  pipeline_data_t m_FCPipeline{};

  VkDescriptorSet m_dFCSet = VK_NULL_HANDLE;
  VkDescriptorSetLayout m_dFCSetLayout = VK_NULL_HANDLE;

  VkBuffer m_matrixInstance = VK_NULL_HANDLE;
  VkDeviceMemory m_matrixInstanceAlloc = VK_NULL_HANDLE;
  void *m_matrixInstanceMappedMem = nullptr;

  VkDrawIndexedIndirectCommand m_drawCmd;
  VkBuffer m_drawCmdBuff = VK_NULL_HANDLE;
  VkDeviceMemory m_drawCmdBuffAlloc = VK_NULL_HANDLE;
  void *m_drawCmdBuffMappedMem = nullptr;

  VkBuffer m_visibleInstancesIdxs = VK_NULL_HANDLE;
  VkDeviceMemory m_visibleInstancesAlloc = VK_NULL_HANDLE;
  void *m_visibleInstancesMappedMem = nullptr;

  VkDescriptorSet m_dSet = VK_NULL_HANDLE;
  VkDescriptorSetLayout m_dSetLayout = VK_NULL_HANDLE;
  VkRenderPass m_screenRenderPass = VK_NULL_HANDLE; // main renderpass

  std::shared_ptr<vk_utils::DescriptorMaker> m_pFCBindings = nullptr;
  std::shared_ptr<vk_utils::DescriptorMaker> m_pBindings = nullptr;

  // *** presentation
  VkSurfaceKHR m_surface = VK_NULL_HANDLE;
  VulkanSwapChain m_swapchain;
  std::vector<VkFramebuffer> m_frameBuffers;
  vk_utils::VulkanImageMem m_depthBuffer{};
  // ***

  // *** GUI
  std::shared_ptr<IRenderGUI> m_pGUIRender;
  virtual void SetupGUIElements();
  void DrawFrameWithGUI();

  bool m_trackCameraTrajectory = false;
  bool m_saveCameraTrajectory = false;
  std::vector<float4x4> m_cameraTrajectory;
  uint32_t m_updateCounter = 0;
  int32_t m_saveFreq = 100;
  //

  Camera   m_cam;
  uint32_t m_width  = 1024u;
  uint32_t m_height = 1024u;
  uint32_t m_framesInFlight  = 2u;
  bool m_vsync = false;

  VkPhysicalDeviceFeatures m_enabledDeviceFeatures = {};
  std::vector<const char*> m_deviceExtensions      = {};
  std::vector<const char*> m_instanceExtensions    = {};

  bool m_enableValidation;
  std::vector<const char*> m_validationLayers;

  std::shared_ptr<SceneManager> m_pScnMgr;

  void DrawFrameSimple();

  void CreateInstance();
  void CreateDevice(uint32_t a_deviceId);

  void BuildCommandBufferSimple(VkCommandBuffer cmdBuff, VkFramebuffer frameBuff,
                                VkImageView a_targetImageView, VkPipeline a_pipeline);

  virtual void SetupSimplePipeline();
  void CleanupPipelineAndSwapchain();
  void RecreateSwapChain();

  void CreateUniformBuffer();
  void UpdateUniformBuffer(float a_time);

  void Cleanup();
  void DestroyBuffer(VkBuffer& el);
  void FreeMemory(VkDeviceMemory& el);

  void SetupDeviceFeatures();
  void SetupDeviceExtensions();
  void SetupValidationLayers();
};


#endif //SIMPLE_RENDER_H
