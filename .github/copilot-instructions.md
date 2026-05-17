# Duk Engine - Copilot Instructions

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

## Code Patterns & Conventions
- Namespace: `duk::submodule` (e.g., `duk::renderer`, `duk::objects`)
- Includes: `<duk_module/header.h>` (e.g., `<duk_renderer/renderer.h>`)
- Headers use include guards: `DUK_MODULE_HEADER_H`
- Methods/functions use snake_case for signatures, camelCase for variables
- Use `auto` and `const` when appropriate
- Always separate declaration and implementation:
  - Do not implement logic in headers, not even small getters
  - Templates are an exception, but should be placed at the end of the file
- Avoid over-commenting; code should be self-explanatory. Use comments for non-obvious logic or decisions.
- Again about comments: DO NOT COMMENT THE OBVIOUS. If you find yourself writing a comment to explain what a line of code does, consider if the code can be rewritten to be clearer instead.
- Never use non-ASCI characters in code or comments (e.g., emojis, non-English characters)

### Error Handling
- Use `duk::log::fatal()` for critical errors
- Return `std::expected` or `Result<T>` types for fallible operations
- Check validity with `.valid()` on IDs/locations

### Macros & Utilities
- `DUK_NO_DISCARD`: Mark functions requiring return value usage
- `DUK_PLATFORM_IS_WINDOWS`: Platform detection
- Utilities in `duk_tools`: `Singleton`, `FixedVector`, `BitBlock`

