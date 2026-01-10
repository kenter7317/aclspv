# aclspv
> it is incomplete project. please keep your expectation of this application low.

- OpenCL C -> SPIR-V compiler application & library for Vulkan.
- Uses llvm-c, clang-c to avoid memory corruption caused by static fiasco 
- `lib` for library source to be compiled
- `inc` for library header
- This project compiles the dialect of OpenCL C. For further, see [here](./OpenCL.md)

## How to build
```sh
cmake -S. -Bbuild
cmake --build build
```

## language
- ISO C90 with strict rule with libclang

## license
- [this project](./LICENSE)
- [llvm related programmes](./LICENSE-llvm)
