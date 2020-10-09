/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: LogPanel
Timestamp: 6/7/2020 8:56:39 PM

*/
#pragma once

#ifndef LogPanel_HPP
#define LogPanel_HPP

#include "Panels/EditorPanel.hpp"

namespace LinaEditor
{
	class GUILayer;

	class LogPanel : public EditorPanel
	{
		
	public:
		
		LogPanel(LinaEngine::Vector2 position, LinaEngine::Vector2 size, GUILayer& guiLayer) : EditorPanel(position, size, guiLayer) {};
		virtual ~LogPanel() {};

		virtual void Draw(float frameTime) override;
		virtual void Setup() override;
	
	private:
	
	};
}

#endif
