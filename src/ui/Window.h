/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <memory>
#include <string>


namespace nhahn
{
	class GLContext;
	class UIContext;

	/** GLFW3 Window with Dear imgui GUI. */
	class Window
	{
	public:
		Window(const char* title, unsigned width, unsigned height, bool customTitlebar = false);
		~Window();

		void setNativeWindow(void* window) { _nativeWindow = window; }
		void* getNativeWindow() const { return _nativeWindow; }

		inline int getWidth() const  { return _width; }
		inline int getHeight() const { return _height; }
		inline std::string getTitle() const { return _title; }

		void preRender(double dt) const;
		void postRender(double dt) const;

		bool isRunning() { return _isRunning; }
		bool hasCustomTitlebar() { return _hasCustomTitlebar; }

		void _onResize(int width, int height);
		void _onClose();

	private:
		bool init();

	private:
		int _width = 0;
		int _height = 0;
		std::string _title;

		void* _nativeWindow = nullptr;
		bool _isRunning = false;

		bool _hasCustomTitlebar = false;

		// Render contexts
		std::unique_ptr<UIContext> _uiContext;
		std::unique_ptr<GLContext> _renderContext;
	};
}
