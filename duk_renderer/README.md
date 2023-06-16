# Renderer

## Types

- Renderer:
  - Main object, configures the rendering environment and holds all command queues;
- Command:
  - Base class for everything that is executed in the GPU. Most of the time these are CommandBuffers, but can also be specialized commands for presentation (e.g. acquiring a swapchain image, presenting)
- CommandQueue:
  - Wraps a TaskQueue (see duk_task) to offload work to a different thread, provides a CommandBuffer as a parameter on submit callback;
- CommandBuffer:
  - All rendering is recorded in CommandBuffers,
- CommandScheduler:
  - Responsible for synchronizing different commands, allows the user to define dependencies between them, like: acquire image -> main render pass -> present. 
- DataSource:
  - Base class for everything that holds information on the host (meshes, images, etc...)
- FrameBuffer:
  - Defines a rendering target, can contain multiple attachments (Images) that can be written to and read on shaders.
- RenderPass:
  - Defines the rendering layout that will be acted upon a FrameBuffer, how different attachments are read from/written to.

## TODO
### Interface
- Shader creation
  - ShaderDataSource:
      - Holds information for all modules (vertex, fragment, etc...)
      - Contains the shader source code: methods for getting the SPIR-V binary code.
      - Plans on adding support for glsl and hlsl code in the future.
      - At first, only have implementation for loading from disk, in the future we'll have code generation, and static content will be possible.
  - Shader:
    - Contains information related to required resources and attributes.
- GraphicsPipeline:
  - Describes all of the current pipeline state, can be modified on the fly while rendering.
### Vulkan
- VulkanCommandScheduler:
  - Only create fences for Commands that actually require them.
- Multi-Buffering:
  - Guarantee that there are always exactly the same number of frames as there are swapchain images.
- VulkanSwapchain recreation:
  - Needs to make sure that every resource that depends on it is recreated as well.
  