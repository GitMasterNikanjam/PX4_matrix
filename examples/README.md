# Examples

Examples are grouped first by target class, then by operating system or MCU,
and then by subject:

```text
examples/
├── os/
│   ├── shared/<topic>/    # Portable host example, with its CMakeLists.txt
│   ├── linux/<topic>/     # Linux-only APIs or services
│   └── windows/<topic>/   # Windows-only APIs or services
└── mcu/
    └── stm32/<topic>/     # STM32 HAL, CMSIS, board, or peripheral examples
```

Put an example in `os/shared/` when the same source builds and runs on multiple
desktop operating systems. Put it under `os/linux/` or `os/windows/` when it
uses that OS's APIs. Put embedded firmware examples under `mcu/<family>/`.
Each example's directory contains its own `CMakeLists.txt`. The shared and OS
CMake files only collect examples; MCU examples are kept out of the host build
and should use a suitable cross-toolchain or vendor project setup.

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

The target and its source are kept together in `os/shared/linear_algebra/`.
Each example directory can be configured on its own or included in the
aggregate `examples` build.

To configure only this example, run these commands from the repository root:

```powershell
cmake -S examples/os/shared/linear_algebra -B build/Matrix_exp1
cmake --build build/Matrix_exp1 --config Release
```
