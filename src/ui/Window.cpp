/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "Window.h"

#include <iostream>
#include "utility/Debug.h"
#include "render/RenderContext.h"


namespace nhahn
{
	Window::Window(const char* title, unsigned width, unsigned height, bool customTitlebar)
		: _title(title), _width(width), _height(height), _nativeWindow(nullptr),
		_isRunning(true)
	{		
		_renderContext = std::make_unique<GLContext>();
		_uiContext = std::make_unique<UIContext>();

		init(customTitlebar);
		DBG("UI", DebugLevel::DEBUG, "Window with size [%u, %u] created\n", width, height);
	}

	Window::~Window()
	{
		_uiContext->end();
		_renderContext->end();
	}

	bool Window::init(bool customTitlebar)
	{
		_renderContext->init(this);
		//_renderContext->setClearColor(glm::vec4(0.19, 0.24f, 0.26f, 1.0f));
		_renderContext->setClearColor(glm::vec4(0.06f, 0.06f, 0.06f, 1.0f));
		_uiContext->init(this);

		if (customTitlebar)
		{
			_hasCustomTitlebar = _uiContext->disableTitlebar();
			//_hasCustomTitlebar = true;
		}

		return _isRunning;
	}

	void Window::preRender(double dt) const
	{
		// Clear the view
		_renderContext->preRender();
		// Initialize UI components
		_uiContext->preRender();
	}

	void Window::postRender(double dt) const
	{
		// Render the UI 
		_uiContext->postRender();
		// Render end, swap buffers
		_renderContext->postRender();
	}

	void Window::_onResize(int width, int height)
	{
		_width = width;
		_height = height;
		DBG("UI", DebugLevel::VERBOSE, "Window resized to [%i,%i]\n", width, height);
	}

	void Window::_onClose()
	{
		_isRunning = false;
		DBG("UI", DebugLevel::DEBUG, "Window shutdown requested\n");
	}
}

