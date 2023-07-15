# gpu_opencl
Getting opencl working on Macbook Pro 2019 (Intel UHD Graphics 630 &amp; AMD Radeon Pro 5500M)

# Check OpenCL Installation 
- OpenCL is typically integrated into MacOS
- You can verify this by running a simple OpenCL program, or by using an OpenCL-capable hardware information tool. For a quick check, you can type `clinfo` in the Terminal, which should display detailed information about all OpenCL-capable devices on your system. If clinfo is not installed, you may install it using Homebrew with `brew install clinfo`.

## Find installation location 
- Locate the OpenCL header file on your system. Open Terminal and use the find command to search for cl.h: `find / -name cl.h 2>/dev/null`

## Update GCC Command if `cl.h` file is in a non-standard location
If the cl.h file is present but in a non-standard location, you might need to tell GCC where to find it. You can do this by adding an -I flag followed by the directory containing cl.h to your GCC command. For example: `gcc -framework OpenCL -I/path/to/opencl/headers test_host_program.c -o test_host_program`
