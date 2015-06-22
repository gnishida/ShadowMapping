#pragma once

#include <vector>
#include "Vertex.h"

class OBJLoader
{
protected:
	OBJLoader() {}

public:
	static void load(char* filename, std::vector<Vertex>& vertices);
};

