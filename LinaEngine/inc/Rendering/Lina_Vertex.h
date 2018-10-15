/*
Author: Inan Evin
www.inanevin.com

MIT License

Lina Engine, Copyright (c) 2018 Inan Evin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

4.0.30319.42000
10/14/2018 11:48:03 PM

*/

#pragma once

#ifndef Lina_Vertex_H
#define Lina_Vertex_H

#include "Math/Lina_Vector3F.h"
#include <glm/glm.hpp>

/*class Lina_Vertex
{

public:

	Lina_Vertex(Vector3 p) : pos(p) {}
	Vector3 GetPos() { return pos; }
	Vector3 SetPos(Vector3 p) { pos = p; }

	static const int SIZE = 3;

private:
	
	Vector3 pos;

};

typedef Lina_Vertex Vertex;*/


struct Vertex
{
public:
	Vertex(glm::vec3 position, glm::vec2 texCoords)
	{
		m_Position = position;
		m_TexCoords = texCoords;
	}

public:
	glm::vec3 m_Position;
	glm::vec2 m_TexCoords;
};





#endif