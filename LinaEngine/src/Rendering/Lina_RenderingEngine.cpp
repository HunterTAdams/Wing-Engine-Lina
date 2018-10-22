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
9/30/2018 4:59:52 PM

*/

#include "pch.h"
#include "Rendering/Lina_RenderingEngine.h"  
#include "Rendering/Lina_PhongShader.h"
#include "Rendering/Lina_BasicShader.h"
#include "Scene/Lina_Camera.h"
#include "Game/Lina_GameCore.h"

// Destructor.
Lina_RenderingEngine::~Lina_RenderingEngine()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Rendering deinitialized.", Lina_Console::MsgType::Deinitialization, "Render Engine");

	delete m_GameWindow;
	delete phongShader;
	delete basicShader;
}

void Lina_RenderingEngine::CreateDisplayWindow()
{
	// Initialize display.
	m_GameWindow = new Lina_Window(screenWidth, screenHeight, screenTitle);
}

// Init rendering engine.
void Lina_RenderingEngine::Initialize(Lina_GameCore* g) 
{
	// Add a console message.
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Rendering engine initialized.", Lina_Console::MsgType::Initialization, "Render Engine");

	// Assign game.
	game = g;

	// Initialize event handler.
	eventHandler.Initialize();

	// Register window events.
	eventHandler.SubscribeToAction(ActionType::SDLQuit, [this]() { m_GameWindow->CloseWindow(); });

	// Create a window.
	CreateDisplayWindow();

	// Clear colors.
	ClearColors(0.0, 0.0, 0.0, 1.0);

	// Enable Textures
	SetTextures(true);

	// Every face in clock-wise order is front.
	glFrontFace(GL_CW);

	// Get rid of the back face.
	glCullFace(GL_BACK);

	// Enable culling faces for now to prevent extra faces.
	glEnable(GL_CULL_FACE);

	// Enable depth test -> enable Z draw-order.
	glEnable(GL_DEPTH_TEST);

	// depth clamp
	glEnable(GL_DEPTH_CLAMP);

	// Get free gamma correction.
	//glEnable(GL_FRAMEBUFFER_SRGB);

	// Init shaders
	InitializeShaders();

}

void Lina_RenderingEngine::InitializeShaders()
{
	phongShader = new Lina_PhongShader();
	basicShader = new Lina_BasicShader();
	phongShader->SetRenderingEngine(this);
	basicShader->SetRenderingEngine(this);
	phongShader->Init();
	basicShader->Init();
}

void Lina_RenderingEngine::ClearScreen()
{
	// Clear color & depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Main method to render.
void Lina_RenderingEngine::Render()
{
	// Render the game.
	game->Render();

	// Check if active window points to an object.
	if (m_GameWindow == nullptr)
	{
		return;
	}

	// Call render method on active window.
	m_GameWindow->Update();
}

void Lina_RenderingEngine::CleanUp()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Cleaning up rendering engine...", Lina_Console::MsgType::Deinitialization, "Render Engine");

}

void Lina_RenderingEngine::SetTextures(bool enabled)
{
	if (enabled)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
}

void Lina_RenderingEngine::ClearColors(float r, float g, float b, float a)
{
	// Clear color.
	glClearColor(r,g,b,a);
}

void Lina_RenderingEngine::SetCurrentActiveCamera(Lina_Camera* cam)
{
	currentActiveCamera = cam;
}

Lina_Camera * Lina_RenderingEngine::GetCurrentActiveCamera()
{
	if (currentActiveCamera == nullptr)
		std::cout << "ERR! Trying to get a deleted camera!";

	return currentActiveCamera;
}

Lina_BasicShader* Lina_RenderingEngine::GetBasicShader()
{
	return basicShader;
}


Lina_PhongShader* Lina_RenderingEngine::GetPhongShader()
{
	return phongShader;
}


float Lina_RenderingEngine::GetAspectRatio()
{
	return (float)screenWidth / (float)screenHeight;
}

int Lina_RenderingEngine::GetScreenWidth()
{
	return screenWidth;
}

int Lina_RenderingEngine::GetScreenHeight()
{
	return screenHeight;
}




