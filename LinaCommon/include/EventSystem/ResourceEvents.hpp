/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
Class: ResourceEvents



Timestamp: 12/25/2021 12:18:35 PM
*/

#pragma once

#ifndef ResourceEvents_HPP
#define ResourceEvents_HPP

// Headers here.
#include "Core/CommonResources.hpp"

namespace Lina::Event
{
	// Resources.
	struct ELoadResourceFromFile { Resources::ResourceType m_resourceType;  std::string m_path; std::string m_assetDataPath; };
	struct ELoadResourceFromMemory { Resources::ResourceType m_resourceType; std::string m_path;  unsigned char* m_data; size_t m_dataSize;  std::string m_assetDataPath; unsigned char* m_assetDataBuffer; size_t m_assetDataSize; };
	struct EResourceLoadCompleted { Resources::ResourceType m_type; StringIDType m_sid; };
	struct EResourceProgressStarted {};
	struct EResourceProgressEnded {};
	struct EAllResourcesLoaded {};
	struct EResourceLoadUpdated { std::string m_currentResource; float m_percentage; };
}

#endif