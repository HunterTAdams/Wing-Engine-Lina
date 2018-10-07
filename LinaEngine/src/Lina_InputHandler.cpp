/*
Author: Inan Evin
www.inanevin.com

BSD 2-Clause License
Lina Engine Copyright (c) 2018, Inan Evin All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.

-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO
-- THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
-- BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
-- GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
-- STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
-- OF SUCH DAMAGE.

4.0.30319.42000
10/4/2018 1:00:34 AM

*/

#include "pch.h"
#include "Lina_InputHandler.h"  



Lina_InputHandler::Lina_InputHandler()
{
	state = SDL_GetKeyboardState(NULL);
	inputDispatcher = Lina_EventDispatcher();
}

void Lina_InputHandler::HandleEvents(SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN)
	{
		//inputDispatcher.DispatchEvent<Lina_Event, SDL_Scancode>(&onKey, SDL_SCANCODE_0);
		inputDispatcher.DispatchEvent(onKey, e.key.keysym.scancode);
	}
/*	else if (it->m_KeyPressEventType == KeyPressEventType::OnKeyDown)
	{
		
	}
	}
	if (e.type == SDL_KEYUP)
	{
		// Remove a frame lock if a key that was pressed have been released.
		if (it->m_KeyFrameLock && it->m_Key == e.key.keysym.scancode)
			it->m_KeyFrameLock = false;

		// Execute if type and key matches.
		if (it->m_KeyPressEventType == KeyPressEventType::OnKeyUp)
		{
			if (it->m_Key == e.key.keysym.scancode)
				it->m_Callback();
		}
	}*/
}

void Lina_InputHandler::Bind(Lina_InputBinding& binding)
{
	// Add the binding with emplace_back and std::move so we will avoid copying, we will use the move constructor.

	if (binding.m_BindingType == BindingType::KeyPress)
		keyPressEventContainers.emplace_back(std::move(binding));
	else if (binding.m_BindingType == BindingType::MousePress)
		mousePressEventContainers.emplace_back(std::move(binding));
	else if (binding.m_BindingType == BindingType::MouseMotion)
		mousePressEventContainers.emplace_back(std::move(binding));

	/*if(binding.m_KeyPressEventType == KeyPressEventType::OnKey || binding.m_KeyPressEventType == KeyPressEventType::OnKeyDown || binding.m_EventType == InputEventType::OnKeyUp)
		keyPressEventContainers.emplace_back(std::move(binding));
	else if(binding.m_EventType == InputEventType::OnMouseButton || binding.m_EventType == InputEventType::OnMouseButtonDown || binding.m_EventType == InputEventType::OnMouseButtonUp)
		mousePressEventContainers.emplace_back(std::move(binding));*/
}

void Lina_InputHandler::Unbind(Lina_InputBinding& binding)
{
	// Iterate through the list, if the matching id is found, remove it.
	// Note that this iteration will remove EVERY object that matches the id, so unique usage of the static ID counter must be cared about.
	if (binding.m_BindingType == BindingType::KeyPress)
	{
		std::list<Lina_InputBinding>::iterator itri = keyPressEventContainers.begin();
		while (itri != keyPressEventContainers.end())
		{
			if (*itri == binding)
			{
				itri = keyPressEventContainers.erase(itri++);
			}
			else
				++itri;
		}
	}
	else if (binding.m_BindingType == BindingType::MousePress)
	{
		std::list<Lina_InputBinding>::iterator itri = mousePressEventContainers.begin();
		while (itri != mousePressEventContainers.end())
		{
			if (*itri == binding)
			{
				itri = mousePressEventContainers.erase(itri++);
			}
			else
				++itri;
		}
	}
	else if (binding.m_BindingType == BindingType::MouseMotion)
	{
		std::list<Lina_InputBinding>::iterator itri = mousePressEventContainers.begin();
		while (itri != mousePressEventContainers.end())
		{
			if (*itri == binding)
			{
				itri = mousePressEventContainers.erase(itri++);
			}
			else
				++itri;
		}
	}
}

void Lina_InputHandler::Update()
{
	SDL_PumpEvents();

}
