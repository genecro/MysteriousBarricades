## Quick start guide
1. Download and install [Ares](https://ares-emu.net/)
2. Download the rom ([mystbar.z64](https://github.com/genecro/MysteriousBarricades/blob/master/mystbar.z64))
3. Run Ares and load the rom

## Dependencies
1. [libdragon](https://github.com/DragonMinded/libdragon) (preview branch)
2. [tiny3d](https://github.com/HailToDodongo/tiny3d)
3. [tinygltf](https://github.com/syoyo/tinygltf)

   a. Download json.hpp, stb_image.h, stb_image_write.h, and tiny_gltf.h

   b. Place them in tools/save_triangles/tinygltf

## Build
1. Run 'make' in tools/save_triangles to build the collision geometry extraction script.
2. Run 'make' in the main directory to build the rom.

## Run
You can run the rom on a Nintendo 64 using a flashcart like [SummerCart64](https://summercart64.dev/), or on your PC using [Ares](https://ares-emu.net/).

I have not personally tested other flashcarts like Everdrive 64 or N64 clones like the Analogue 64, but they should work. Note that this will NOT work with other emulators like Project64.
