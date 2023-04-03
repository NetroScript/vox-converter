#include <iostream>
#include <fstream>
#include <iosfwd>
#include <openvdb/openvdb.h>
#include <openvdb/io/Stream.h>
#include <nanovdb/util/OpenToNanoVDB.h> // converter from OpenVDB to NanoVDB (includes NanoVDB.h and GridManager.h)
#include <nanovdb/util/IO.h>

int main(int argc, const char * argv[]) {

    // This application should read in a binary .vox file which just contains a 3D array of unsigned chars representing the density of the voxel.
    // Which is the default and also what we are using

    // We need the filepath to the file as a parameter from the command line
    std::string filepath = "";

    // To read in the file, we get the dimensions of the file as parameters from the command line
    int x = 128;
    int y = 128;
    int z = 128;

    // Optional argument if we want to use NanoVDB instead of OpenVDB
    bool useNanoVDB = false;

    // Parse command line arguments

    // Parse -i <path> for file path
    // Parse -dim <x> <y> <z> for dimensions
    // Parse -nanovdb to use NanoVDB instead of OpenVDB

    bool dim_spezified = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-i") {
            // Check if the next argument exists
            if (i + 1 >= argc) {
                std::cout << "Error: No file path specified after -i" << std::endl;
                return 1;
            }
            filepath = argv[i+1];

            i++;
        } else if (arg == "-dim") {
            // Check if the next 3 arguments exist
            if (i + 3 >= argc) {
                std::cout << "Error: No dimensions specified after -dim" << std::endl;
                return 1;
            }
            x = std::stoi(argv[i+1]);
            y = std::stoi(argv[i+2]);
            z = std::stoi(argv[i+3]);
            dim_spezified = true;
            i += 3;
        } else if (arg == "-nanovdb") {
            useNanoVDB = true;
        }
    }

    // Check if the file path is empty
    if (filepath.empty()) {
        std::cout << "Error: No file path specified" << std::endl;

        // Print usage
        std::cout << "Usage: " << argv[0] << " -i <path> [-dim <x> <y> <z>] [-nanovdb]" << std::endl;

        return 1;
    }

    // Check if together with the file path, there exists a .hd header file
    // First get the file path without the extension
    std::string filepath_without_extension = filepath.substr(0, filepath.find_last_of('.'));

    // Try to load a header file
    std::ifstream header_file(filepath_without_extension + ".hd");

    // If the header file exists, we read in the dimensions
    if (header_file.is_open()) {
        std::cout << "Reading header file for dimensions" << std::endl;

        // Get the content of the header file as a stringstream
        std::stringstream content;
        content << header_file.rdbuf();

        // Close the header file
        header_file.close();

        // Parse the header file by splitting "Size XxYxZ" into "X", "Y" and "Z"

        // First offset the stringstream by 5 characters
        content.seekg(5);
        // Next read in the x dimension by using getline and splitting the string at the "x"
        std::string x_str;
        std::getline(content, x_str, 'x');
        x = std::stoi(x_str);

        // Next read in the y dimension by using getline and splitting the string at the "x"
        std::string y_str;
        std::getline(content, y_str, 'x');
        y = std::stoi(y_str);

        // Next read in the z dimension by using getline and splitting the string at the "x"
        std::string z_str;
        std::getline(content, z_str, 'x');
        // Remove everything after a newline character
        z_str = z_str.substr(0, z_str.find_first_of('\n'));
        z = std::stoi(z_str);

    } else {
        // If the header file does not exist, we check if the dimensions were specified
        if (!dim_spezified) {
            std::cout << "Error: No dimensions specified" << std::endl;

            // Print usage
            std::cout << "Usage: " << argv[0] << " -i <path> [-dim <x> <y> <z>] [-nanovdb]" << std::endl;

            return 1;
        }
    }

    // We need to initialize the OpenVDB library
    openvdb::initialize();

    // We create a new grid which uses a single byte to store the values
    openvdb::FloatGrid::Ptr grid = openvdb::FloatGrid::create();

    // We set the grid's transform to a scale of 1.0
    grid->setTransform(openvdb::math::Transform::createLinearTransform(1.0));

    // We create an accessor to the grid
    openvdb::FloatGrid::Accessor accessor = grid->getAccessor();

    std::cout << "Reading file..." << std::endl;

    // We open the file
    FILE *file = fopen(filepath.c_str(), "rb");

    // We read in the data
    auto *data = new unsigned char[x*y*z];
    fread(data, sizeof(unsigned char), x*y*z, file);

    // We close the file
    fclose(file);

    std::cout << "Converting to VDB with the dimensions: " << x << "x" << y << "x" << z << " ..." << std::endl;

    // We iterate through each voxel
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < z; k++) {

                // Fetch the value of the voxel
                unsigned char value = data[i + j*x + k*x*y];

                // Normalize the value to 0.0 - 1.0
                float normalized = (float)value / 255.0f;

                // We set the voxel's value
                accessor.setValue(openvdb::Coord(i, j, k), normalized);
            }
        }
    }

    // We delete the data
    delete[] data;

    // If we want to use NanoVDB, we convert the OpenVDB grid to a NanoVDB grid
    if (useNanoVDB) {
        std::cout << "Converting to NanoVDB..." << std::endl;

        // We create a new NanoVDB grid manager
        auto handle = nanovdb::openToNanoVDB(*grid);

        // We write the NanoVDB grid to a file
        std::cout << "Writing NanoVDB..." << std::endl;

        nanovdb::io::writeGrid(filepath_without_extension + ".nvdb", handle); // Write the NanoVDB grid to file and throw if writing fails

        std::cout << "Done!" << std::endl;

        // We exit the program
        return 0;
    }

    std::cout << "Writing VDB..." << std::endl;

    // We create a new VDB output file
    openvdb::io::File(filepath_without_extension + ".vdb").write({grid});



    std::cout << "Done!" << std::endl;
    return 0;
}
