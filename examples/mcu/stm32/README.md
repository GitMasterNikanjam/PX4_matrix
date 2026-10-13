# STM32 examples

Put STM32 firmware demonstrations under this directory and group them by topic
when needed (for example, `linear_algebra/` or `attitude/`). Keep STM32 HAL,
CMSIS, board startup, and peripheral dependencies here. The host CMake build
does not compile this directory.

`Matrix_exp1` is a console demonstration for desktop operating systems. An
STM32 example should instead show how to call the matrix library from a board
application and document the target MCU, toolchain, floating-point settings,
and required STM32Cube project setup.
