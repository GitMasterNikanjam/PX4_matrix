# OS-targeted examples

- `shared/<topic>/` contains portable host examples intended to build on both
  Linux and Windows.
- `linux/<topic>/` contains examples that need Linux-specific APIs or services.
- `windows/<topic>/` contains examples that need Windows-specific APIs or
  services.

Keep the shared example build in `shared/CMakeLists.txt`. Put platform-specific
targets in the matching OS directory and its own `CMakeLists.txt`; the parent
OS build includes only the directory matching the active CMake platform.
