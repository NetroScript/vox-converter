# vox-converter

This is a simple C++ tool to convert binary `vox` files into the more popular OpenVDB or NanoVDB format.
The `vox` format in question are the ones used by the [CGV framework](https://github.com/sgumhold/cgv), not the MagickaVoxel format.


## Usage

```bash	
./<app_name> -i <path> [-dim <x> <y> <z>] [-nanovdb]
```

### Arguments

* `-i <path>` : specifies the file path to the binary .vox file containing the voxel data.
* `-dim <x> <y> <z>` : specifies the dimensions of the voxel data in the file. This argument is optional, and if not specified, the dimensions will be read from a .hd header file located in the same directory as the .vox file.
* `-nanovdb` : specifies that NanoVDB should be used instead of OpenVDB. This argument is optional, and by default OpenVDB is used.

## Dependencies

* OpenVDB 10.0.0 and later
* CMake 

## Build Instructions

1. Clone the repository

```bash
git clone https://github.com/NetroScript/vox-converter.git
cd vox-converter
```

2. Create a build directory 

```bash
mkdir build
cd build
```

3. Run Cmake to configure the project

```bash
cmake ..
```

4. Build the project

```bash
cmake --build .
```

This assumes that you have OpenVDB installed in a standard location. If not, you may need to manually specifiy the location of the OpenVDB installation in the `CMakeLists.txt` file.
