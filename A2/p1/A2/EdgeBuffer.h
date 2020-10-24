#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct VFB
{
    unsigned int V;
    bool F;
    bool B;
    bool Fa; //Not required in A#2
    bool Ba; //Not required in A#2
};

class EdgeBuffer
{
    //Constructor
    EdgeBuffer(int numVertices)
    {
        //Here we initialize the matrix to have numVertices number of rows
        //There are no columns yet, We add them using the Edge-Buffer construction algorithm!
        for (int i = 0; i < numVertices; i++)
        {
            eb.push_back(std::vector<VFB>());
        }
    }

    //The 2D vector of VFB's
    std::vector<std::vector<VFB>> eb;
};