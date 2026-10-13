# Examples

Examples are grouped first by target class, then by operating system or MCU,
and then by subject:

```text
examples/
├── os/
│   ├── shared/<topic>/    # Portable host examples (Linux and Windows)
│   ├── linux/<topic>/     # Linux-only APIs or services
│   └── windows/<topic>/   # Windows-only APIs or services
└── mcu/
    └── stm32/<topic>/     # STM32 HAL, CMSIS, board, or peripheral examples
```

Put an example in `os/shared/` when the same source builds and runs on multiple
desktop operating systems. Put it under `os/linux/` or `os/windows/` when it
uses that OS's APIs. Put embedded firmware examples under `mcu/<family>/`.
The CMake build below compiles shared OS examples and conditionally includes an
OS-specific subdirectory when it has its own `CMakeLists.txt`. MCU examples are
kept out of the host build and should use a suitable cross-toolchain or vendor
project setup.

## Build and run `Matrix_exp1`

`Matrix_exp1` is a standalone host example for the header-only matrix library.
It does not require PX4 or GoogleTest. CMake and a C++11-compatible compiler
are required.

Run these commands from the root of the `PX4_matrix` repository:

```powershell
cmake -S examples -B build/examples
cmake --build build/examples --config Release
```

Run the resulting executable:

- **Windows with Visual Studio:** `build\examples\Release\Matrix_exp1.exe`
- **Windows with MinGW or Ninja:** `build\examples\Matrix_exp1.exe`
- **Linux:** `./build/examples/Matrix_exp1`

If CMake selected a different multi-configuration generator, use the
configuration directory reported by that generator (for example, `Debug` in
place of `Release`).

The target is declared in `os/shared/CMakeLists.txt`, and its source is in
`os/shared/linear_algebra/Matrix_exp1.cpp`.
