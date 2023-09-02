/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "Application.h"

#include "input/Input.h"
#include "utility/Debug.h"
#include "utility/Timer.h"


namespace nhahn
{
	Application::Application(const std::string& appname, bool customTitlebar) : _dt(0)
	{
		DBG("Application", DebugLevel::INFO, "Application start\n");
		_mainWindow = std::make_shared<Window>(appname.c_str(), 1280, 720, customTitlebar);

		// update input at poll rate (replayer manages own tickrate)
		addUpdateCallback([&](double dt) { gInput().update(dt); });

		// Input should recheck gamepad connection every 10s
		gInput().connectionRecheck(true);
		gInput().setConnectionRecheckTime(10);
	}

	Application::~Application()
	{
		DBG("Application", DebugLevel::INFO, "Application end\n");
	}

	void Application::setRenderCallback(std::function<void(double)> func, double tickrate)
	{
		ApplicationCallback ac;
		ac.callback = func;
		ac.tickrate = tickrate;
		_renderCallback = ac;
		DBG("Application", DebugLevel::DEBUG, "changed render callback with tickrate %f\n", tickrate);
	}

	void Application::addUpdateCallback(std::function<void(double)> func, double tickrate)
	{
		ApplicationCallback ac;
		ac.callback = func;
		ac.tickrate = tickrate;
		_updateCallbacks.push_back(ac);
		DBG("Application", DebugLevel::DEBUG, "update callback with tickrate %f added\n", tickrate);
	}

	void Application::run()
	{
		DBG("Application", DebugLevel::DEBUG, "run main loop\n");
		Timer appTimer;

		// main message loop
		double frameStartTime = (double)appTimer.getMicroseconds()/(double)1e6;
		while (_mainWindow->isRunning())
		{
			double now = (double)appTimer.getMicroseconds()/(double)1e6;
			_dt = now - frameStartTime;
			frameStartTime = now;

			// update
			for (ApplicationCallback& uc : _updateCallbacks)
			{
				if (uc.callback)
				{
					if (uc.nextCall <= 0.0)
					{
						double ucNow = (double)appTimer.getMicroseconds()/(double)1e6;
						uc.dt = ucNow - uc.frameStartTime;
						uc.frameStartTime = ucNow;

						uc.nextCall = uc.tickrate;
						uc.callback(uc.dt);
					}
					else
						uc.nextCall -= _dt;
				}
			}

			// render
			if (_renderCallback.nextCall <= 0.0)
			{
				double rcNow = (double)appTimer.getMicroseconds() / (double)1e6;
				_renderCallback.dt = rcNow - _renderCallback.frameStartTime;
				_renderCallback.frameStartTime = rcNow;
				_mainWindow->preRender(_renderCallback.dt);
				_renderCallback.nextCall = _renderCallback.tickrate;
				if (_renderCallback.callback)
					_renderCallback.callback(_renderCallback.dt);
				_mainWindow->postRender(_renderCallback.dt);
			}
			else
				_renderCallback.nextCall -= _dt;
			
		}
	}
}