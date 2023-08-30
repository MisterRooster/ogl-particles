/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "ui/Window.h"


namespace nhahn
{
	struct ApplicationCallback
	{
		std::function<void(double)> callback = {};
		double tickrate = 0.0;
		double nextCall = 0.0;
		double dt = 0.0;
		double frameStartTime = 0.0;
	};

	class Application
	{
	public:
		Application(const std::string& appname);

		static Application& instance() { return *_sInstance; }
		Window* getWindow() const { return _mainWindow.get(); }

		void setRenderCallback(std::function<void(double)> func, double tickrate = 0.0);
		void addUpdateCallback(std::function<void(double)> func, double tickrate = 0.0);

		void run();

	private:
		static Application* _sInstance;

		std::shared_ptr<Window> _mainWindow;
		double _dt;

		std::vector<ApplicationCallback> _updateCallbacks = {};
		ApplicationCallback _renderCallback;
	};
}