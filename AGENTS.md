# Duk Engine - AI Agent Guidelines

## Architecture Overview

Duk is a modular C++ game engine focused on rendering and game development. The engine follows a component-based architecture with clear module boundaries:

- **duk_animation**: Animation system with clips, controllers, and properties
- **duk_api**: API bindings for scripting or external interfaces
- **duk_audio**: Audio handling and playback
- **duk_cli**: Command line interface tools
- **duk_engine**: Core engine lifecycle and main loop
- **duk_event**: Event system with listeners and emitters
- **duk_hash**: Hashing utilities and DataSource interface
- **duk_log**: Logging with configurable sinks and levels
- **duk_macros**: Common macros and preprocessor utilities
- **duk_math**: Math utilities (Smoothstep, AABB, Frustum, etc.)
- **duk_objects**: Data-oriented ECS with objects and components
- **duk_platform**: Platform abstraction (Windows-only)
- **duk_project**: Project management and configuration
- **duk_renderer**: Resource loading and Vulkan-based rendering pipeline
- **duk_resource**: Resource management with pools and IDs
- **duk_rhi**: Rendering hardware interface (Vulkan)
- **duk_runtime**: Runtime execution and scripting
- **duk_sample**: Sample application and demos
- **duk_serial**: Serialization and data persistence
- **duk_shader_generator**: Shader generation and compilation
- **duk_system**: System-level utilities and abstractions
- **duk_task**: Multi-threaded task queues
- **duk_tools**: General utilities (Singleton, FixedVector, BitBlock)
- **duk_type**: Type system and reflection
- **duk_ui**: User interface system and components

Data flows from high-level objects/components through renderer pools to RHI commands.

## Build & Development Workflow

### Building
- Use CMake presets: `cmake --preset visual-studio` or `ninja`
- Dependencies managed via vcpkg (see `vcpkg.json`)
- Builds output to `.duk/builds/`
- Enable tests/samples: `DUK_BUILD_TESTS=ON`, `DUK_BUILD_SAMPLE=ON`

### Formatting
- Run `just format` to format all C++ files with clang-format
- Applies to `.cpp`/`.h` files in `duk_*` directories
- Check formatting: `python scripts/format.py --check`

### Branching & Commits
Follow strict naming conventions:
- Branches: `(new|chg|fix)/(short-message)`
- Commits: `(new|chg|fix): (short message)`

Examples:
```
new/add-new-things
fix/fix-broken-things
new: add specific thing
```

### Testing
- Tests use Catch2 framework
- Run via `ctest` after building with `DUK_BUILD_TESTS=ON`
- Each module has `test/` subdirectory with CMakeLists.txt

## Code Patterns & Conventions

### Namespaces & Includes
- Namespace: `duk::submodule` (e.g., `duk::renderer`, `duk::objects`)
- Includes: `<duk_module/header.h>` (e.g., `<duk_renderer/renderer.h>`)
- Headers use include guards: `DUK_MODULE_HEADER_H`
- Methods/functions use snake_case for signatures, camelCase for variables
- Use auto and const when appropriate
- Always separate declaration and implementation:
  - Do not implement logic in headers, not even small getters
  - Templates are an exception, but should be placed at the end of the file

### ECS Usage
Objects are ID-based with attached components:
```cpp
auto obj = objects->add_object();
obj.add<duk::renderer::Position3D>()->value = glm::vec3(0,0,0);
obj.add<duk::renderer::MeshDrawing>()->mesh = renderer->mesh_pool()->cube();
```

Iterate with component filters:
```cpp
for (auto object : objects->all_with<Position3D, MeshDrawing>()) {
    auto [pos, mesh] = object.components<Position3D, MeshDrawing>();
    // use components
}
```

### Resource Management
Resources use pools with IDs:
```cpp
auto material = renderer->material_pool()->create_phong(duk::pool::Id(666));
auto mesh = renderer->mesh_pool()->cube();
```

### Events
Event-driven communication:
```cpp
duk::event::Listener listener;
duk::event::EventT<int> myEvent;
listener.listen(myEvent, [](int value) { /* handle */ });
myEvent.emit(42);
```

### Rendering Pipeline
Materials bind shaders and descriptors:
```cpp
MaterialCreateInfo info = { /* shader, bindings */ };
auto material = std::make_shared<Material>(info);
material->set("uColor", glm::vec4(1.0f));
renderer->render(objects);
```

### Error Handling
- Use `duk::log::fatal()` for critical errors
- Return `std::expected` or `Result<T>` types for fallible operations
- Check validity with `.valid()` on IDs/locations

### Macros & Utilities
- `DUK_NO_DISCARD`: Mark functions requiring return value usage
- `DUK_PLATFORM_IS_WINDOWS`: Platform detection
- Utilities in `duk_tools`: `Singleton`, `FixedVector`, `BitBlock`

## Key Directories & Files

- `duk_renderer/src/duk_renderer/material/material.cpp`: Material system implementation with binding management
- `duk_objects/include/duk_objects/objects.h`: ECS core with component pools
- `CMakeLists.txt`: Root build configuration with module subdirs
- `CMakePresets.json`: Build presets for VS/Ninja with vcpkg
- `justfile`: Development tasks (format, convert scripts)
- `scripts/format.py`: Clang-format automation for C++ files
