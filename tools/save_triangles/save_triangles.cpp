#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"
#include <bit>
#include <vector>
#include <iostream>
#include <fstream>


typedef union {
    struct {
        float x, y, z;  ///< Named fields to access individual vector components directly.
    };
    float v[3];         ///< Array field to access the vector components via index.
} T3DVec3;

struct Triangle {
    T3DVec3 v0, v1, v2;
};

float ReverseFloat( const float inFloat )
{
   float retVal;
   char *floatToConvert = ( char* ) & inFloat;
   char *returnFloat = ( char* ) & retVal;

   // swap the bytes into a temporary buffer
   returnFloat[0] = floatToConvert[3];
   returnFloat[1] = floatToConvert[2];
   returnFloat[2] = floatToConvert[1];
   returnFloat[3] = floatToConvert[0];

   return retVal;
}

uint32_t ReverseUInt32(uint32_t number) {
    return ((number & 0xFF) << 24) | ((number & 0xFF00) << 8) | ((number & 0xFF0000) >> 8) | (number >> 24);
}

/*
bool isDegenerate(const Triangle& tri) {
    // Check if any two vertices are very close (within a small epsilon)
    constexpr float EPSILON = 1e-6f;
    if (glm::distance(tri.v0, tri.v1) < EPSILON ||
        glm::distance(tri.v1, tri.v2) < EPSILON ||
        glm::distance(tri.v2, tri.v0) < EPSILON) {
        return true;
    }
    
    // Check if the area of the triangle is effectively zero
    glm::vec3 v1 = tri.v1 - tri.v0;
    glm::vec3 v2 = tri.v2 - tri.v0;
    glm::vec3 crossProduct = glm::cross(v1, v2);
    return glm::length(crossProduct) < EPSILON;
}
*/

//extract triangles and save to binary
void SaveTrianglesToBinary(const std::string& glbFilename, const std::string& binaryFilename) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    if (!loader.LoadBinaryFromFile(&model, &err, &warn, glbFilename)) {
        throw std::runtime_error("Failed to load GLB: " + err);
    }

    std::vector<Triangle> triangles;

    //iterate through meshes
    for(const auto& mesh : model.meshes) {
        for(const auto& primitive: mesh.primitives) {
            if(primitive.mode != TINYGLTF_MODE_TRIANGLES) continue;

            //ignore triangles whose material name starts with "noColl_"
            if(primitive.material >=0 && primitive.material < model.materials.size()) {
                if (model.materials[primitive.material].name.rfind("noColl_", 0)==0) {
                    continue;
                }
            }

            //extract indices
            const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
            const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
            const unsigned short* indices = reinterpret_cast<const unsigned short*>(
                &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]
            );

            //extract vertex positions
            const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.at("POSITION")];
            const tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];
            const tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];
            const float* positions = reinterpret_cast<const float*>(
                &posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]
            );

            //store triangles
            for (size_t i = 0; i < indexAccessor.count; i += 3) {
                Triangle tri;
                float v0x = positions[indices[i]*3];
                float v0xSwap = ReverseFloat(v0x);
                float v0y = positions[indices[i]*3 + 1];
                float v0ySwap = ReverseFloat(v0y);
                float v0z = positions[indices[i]*3 + 2];
                float v0zSwap = ReverseFloat(v0z);

                float v1x = positions[indices[i+1]*3];
                float v1xSwap = ReverseFloat(v1x);
                float v1y = positions[indices[i+1]*3 + 1];
                float v1ySwap = ReverseFloat(v1y);
                float v1z = positions[indices[i+1]*3 + 2];
                float v1zSwap = ReverseFloat(v1z);

                float v2x = positions[indices[i+2]*3];
                float v2xSwap = ReverseFloat(v2x);
                float v2y = positions[indices[i+2]*3 + 1];
                float v2ySwap = ReverseFloat(v2y);
                float v2z = positions[indices[i+2]*3 + 2];
                float v2zSwap = ReverseFloat(v2z);
                tri.v0 = (T3DVec3){{v0xSwap, v0ySwap, v0zSwap}};
                tri.v1 = (T3DVec3){{v1xSwap, v1ySwap, v1zSwap}};
                tri.v2 = (T3DVec3){{v2xSwap, v2ySwap, v2zSwap}};

                //tri.v0 = (T3DVec3){{v0x, v0y, v0z}};
                //tri.v1 = (T3DVec3){{v1x, v1y, v1z}};
                //tri.v2 = (T3DVec3){{v2x, v2y, v2z}};

                //printf("v0 x: %.2f, y: %.2f, z: %.2f\n", v0x, v0y, v0z);
                //printf("v1 x: %.2f, y: %.2f, z: %.2f\n", v1x, v1y, v1z);
                //printf("v2 x: %.2f, y: %.2f, z: %.2f\n\n", v2x, v2y, v2z);

                //if(!isDegenerate(tri)) {
                    triangles.push_back(tri);
                //}
                //else {
                //    printf("Discarded degenerate triangle");
                //}
            }
        }
    }

    //save to binary file
    std::ofstream outFile(binaryFilename, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open binary file for writing.");
    }

    uint32_t triangleCount = triangles.size();
    uint32_t swappedTriCt = ReverseUInt32(triangleCount);
    
    //outFile.write(reinterpret_cast<const char*>(&triangleCount), sizeof(uint32_t));
    outFile.write(reinterpret_cast<const char*>(&swappedTriCt), sizeof(uint32_t));
    outFile.write(reinterpret_cast<const char*>(triangles.data()), triangleCount * sizeof(Triangle));

    outFile.close();
    printf("Saved %d triangles to %s", triangleCount, binaryFilename.c_str());
    std::cout << "Saved " << triangleCount << " triangles to " << binaryFilename << std::endl;
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.glb> <output.bin>" << std::endl;
        return 1;
    }
    SaveTrianglesToBinary(argv[1], argv[2]);    
}