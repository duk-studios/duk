# Renderer

## Types

- Renderer:
  - Main object, configures the rendering environment and holds all command queues;
- Command:
  - Base class for everything that is executed in the GPU. Most of the time these are CommandBuffers, but can also be specialized commands for presentation (e.g. acquiring a swapchain image, presenting)
- CommandQueue:
  - Wraps a TaskQueue (see duk_task) to offload work to a different thread, provides a CommandBuffer as a parameter on enqueue callback;
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
- Pipeline creation
- Mesh creation
### Vulkan
- VulkanCommandQueue:
  - Add support for multiple CommandBuffers in a single frame
  