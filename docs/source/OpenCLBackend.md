# About the OpenCL backend

This is a brief document/note about the OpenCL backend.

## OpenCL standard version
Currently the backend only requires and uses the features provided by OpenCL 1.2. But OpenCL 2.0 support is planned.

## Program caching
The OpenCL backend caches programs automatoically. If a program `cast` already exists in the kernel manager. Asking it to compile another version of `cast` does nothing. Set the `force_override` flag to force a compilation and override.

## Local Memory
Due to how HTM work - being very memory bangwidth hungry and the input SDR is relativelly small. The OpenCL backend tries to stores data in the GPU's local(on-chip) memory so more bandwidth can be used for fetching synapse.

However this also poses limitations. Since the input Tensor is copied into the local memory. The size of the input Tensor cannot exceed the size of local memory (48KB on NVIDIA cards and 64KB on AMD cards). If larger inputs are encountered, Etaler simply switches to a version not using local memory.
Also some GPUs - all of them are mobile GPUs -  don't have such local memory and is emulated using global memroy. Etaler uses kernels without local memory optimization.

## Global inhibition
Currently finding the top-k values are done by a quick counting based scanning methoud. Performed by a single work group and stores it's counters in Local Memory. This is fast and accurate as long as the size of the input array is small (the max value is less then the size of the counter). Thus another algorithm might be needed for big arrays.

## OpenCL kenrel distribution
The OpenCL backend searchs for the kernels it needs in the following paths: `./kernels/`, `../kernels/`, `/usr/local/share/Etaler/kernels` and `/usr/share/Etaler/kernels/`. If the file is found, then it is read and cached in memory. If not, an exception is raised.
The kernel files are installed when installing the library.

If you have your kernels stored at a different location. You can set the `ETALER_KERNEL_PATH` enviroment variable to make your path avavliable.

## JIT compiling views
The OpenCL backend generates the OpenCL kernels to copy/write to Tensor views at runtime. Thus copying from a view might be slow. It the problem turns out to be too bug a problem. It will be cchanged.

## NVIDIA's OpenCL implementation
NVIDIA's OpenCL implementation can crash without notifing the user. (kerenl can crash without abort, generating error code at the wrong places, etc...). Use POCL's CUDA backend for varification that the kernel is running correctly.

## program name mangling
Since the OpenCL backend tracks programes using a key. Name mangling (in Etaler's case, appending the hash of the compiler argumnents to the end of the key) is required to support multiple versions of the same program (with different `-D` defines, etc...).

## RPi VC4CL Support
VC4CL is **not suported** for now. The limitation of VC4CL only supporting up to 12 PE per work group is not taken into account in the OpenCL backend. (And VC4 uses global memory to emulate local memory, it is going to be slow),

## Altera AOCL / Xiinx SDAccel support
FPGA based OpenCL although interaseting, are not supported now due to the lack of a API callable compiler.

