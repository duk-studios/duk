# Renderer

## Types

- Renderer:
  - Main object, configures the rendering environment and holds all command queues;
- CommandQueue:
  - Wraps a TaskQueue (see duk_task) to offload work to a different thread, provides a CommandInterface as a parameter on enqueue callback;
- CommandInterface:
  - Holds the implementation of all commands for the underlying graphics API, it is used to create resources and render them. Is always used inside a CommandQueue callback;
- Data Source:
  - Base class for everything that holds information on the host (meshes, images, etc...)

## WIP
### Interface
- Usage still being decided:
  - Frame presentation
  - Shader creation
  - Pipeline creation
  - Mesh creation
### Vulkan
- Currently the following features are being implemented in vulkan:
  - CommandInterface:
    - Render methods
    - Create methods
  