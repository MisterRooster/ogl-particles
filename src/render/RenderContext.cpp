/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "RenderContext.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "ui/Window.h"
#include "input/Input.h"
#include "utility/Debug.h"

#include "GL/glew.h"
#define GLFW_INCLUDE_GLEXT
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#ifdef _WIN32
#	include <dwmapi.h>
#endif // _WIN32

namespace nhahn
{
	static void on_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		gInput()._notifyKeyChanged(key, scancode, action, mods);

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			auto pWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			pWindow->_onClose();
		}
	}

	static void on_mouse_btn_callback(GLFWwindow* window, int button, int action, int mods)
	{
		gInput()._notifyMouseButtonPressed(button, action, mods);
	}

	static void on_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
	{
		gInput()._notifyCursorMoved(xpos, ypos);
	}

	static void on_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		gInput()._notifyMouseWheelScrolled(xoffset, yoffset);
	}

	static void on_window_size_callback(GLFWwindow* window, int width, int height)
	{
		auto pWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
		pWindow->_onResize(width, height);
	}

	static void on_window_close_callback(GLFWwindow* window)
	{
		Window* pWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
		pWindow->_onClose();
	}

	void error_callback(int error, const char* description)
	{
		FAIL("Error: %s\n", description);
	}

#ifdef _WIN32
	WNDPROC original_proc;

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
	    switch (uMsg)
	    {
	        case WM_NCCALCSIZE:
	        {
	            // Remove the window's standard sizing border
	            if (wParam == TRUE && lParam != NULL)
	            {
	                NCCALCSIZE_PARAMS* pParams = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
	                pParams->rgrc[0].top += 1;
	                pParams->rgrc[0].right -= 2;
	                pParams->rgrc[0].bottom -= 2;
	                pParams->rgrc[0].left += 2;
	            }
	            return 0;
	        }
	        case WM_NCPAINT:
	        {
	            // Prevent the non-client area from being painted
	            return 0;
	        }
	        case WM_NCHITTEST:
	        {
	            // Expand the hit test area for resizing
	            const int borderWidth = 4; // Adjust this value to control the hit test area size
	
	            POINTS mousePos = MAKEPOINTS(lParam);
	            POINT clientMousePos = { mousePos.x, mousePos.y };
	            ScreenToClient(hWnd, &clientMousePos);
	
	            RECT windowRect;
	            GetClientRect(hWnd, &windowRect);
	
	            if (clientMousePos.y >= windowRect.bottom - borderWidth)
	            {
	                if (clientMousePos.x <= borderWidth)
	                    return HTBOTTOMLEFT;
	                else if (clientMousePos.x >= windowRect.right - borderWidth)
	                    return HTBOTTOMRIGHT;
	                else
	                    return HTBOTTOM;
	            }
	            else if (clientMousePos.y <= borderWidth)
	            {
	                if (clientMousePos.x <= borderWidth)
	                    return HTTOPLEFT;
	                else if (clientMousePos.x >= windowRect.right - borderWidth)
	                    return HTTOPRIGHT;
	                else
	                    return HTTOP;
	            }
	            else if (clientMousePos.x <= borderWidth)
	            {
	                return HTLEFT;
	            }
	            else if (clientMousePos.x >= windowRect.right - borderWidth)
	            {
	                return HTRIGHT;
	            }
	
	            break;
	        }
			case WM_NCACTIVATE:
			{
				// Prevent non-client area from being redrawn during window activation
				return TRUE;
			}
	    }
	    
	    return CallWindowProc(original_proc, hWnd, uMsg, wParam, lParam);
	}

	void disableTitlebarWin32(GLFWwindow* window)
	{
		HWND hWnd = glfwGetWin32Window(window);

		LONG_PTR lStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
		lStyle |= WS_THICKFRAME;
		lStyle &= ~WS_CAPTION;
		SetWindowLongPtr(hWnd, GWL_STYLE, lStyle);

		// Set the window shape and rounded corners
		DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
		DwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));

		// Extend the frame into the client area
		MARGINS margins = { -1 };
		DwmExtendFrameIntoClientArea(hWnd, &margins);

		RECT windowRect;
		GetWindowRect(hWnd, &windowRect);
		int width = windowRect.right - windowRect.left;
		int height = windowRect.bottom - windowRect.top;

		original_proc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
		(WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc));
		SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_FRAMECHANGED | SWP_NOMOVE);
	}
#endif // _WIN32

	//----------------------------------------------------------------------------------------------

	bool GLContext::init(Window* window)
	{
		__super::init(window);
		glfwSetErrorCallback(error_callback);

		// initialize glfw
		if (!glfwInit())
		{
			FAIL("Failed to initialize GLFW!");
			return false;
		}

		// initialize window
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_RESIZABLE, true);
		//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		auto gl_Window = glfwCreateWindow(window->getWidth(), window->getHeight(), window->getTitle().c_str(), NULL, NULL);
		window->setNativeWindow(gl_Window);
		if (!gl_Window)
		{
			FAIL("Failed to create GLFW Window!");
			return false;
		}

		glfwSetWindowUserPointer(gl_Window, window);
		glfwSetKeyCallback(gl_Window, on_key_callback);
		glfwSetMouseButtonCallback(gl_Window, on_mouse_btn_callback);
		glfwSetCursorPosCallback(gl_Window, on_cursor_pos_callback);
		glfwSetScrollCallback(gl_Window, on_scroll_callback);
		glfwSetWindowSizeCallback(gl_Window, on_window_size_callback);
		glfwSetWindowCloseCallback(gl_Window, on_window_close_callback);
		glfwMakeContextCurrent(gl_Window);
		glfwSwapInterval(0); // use 1 to enable vsync

		// load extension libraries
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			FAIL("Error: %s\n", glewGetErrorString(err));
			return false;
		}

		glEnable(GL_DEPTH_TEST);

		DBG("UI", DebugLevel::DEBUG, "OpenGL context created successfully\n");
		return true;
	}

	void GLContext::preRender()
	{
		int display_w, display_h;
		glfwGetFramebufferSize((GLFWwindow*)_window->getNativeWindow(), &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, _clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void GLContext::postRender()
	{
		glfwPollEvents();
		glfwSwapBuffers((GLFWwindow*)_window->getNativeWindow());
	}

	void GLContext::end()
	{
		auto win = (GLFWwindow*)_window->getNativeWindow();
		if (win)
			glfwDestroyWindow(win);
		glfwTerminate();
		DBG("UI", DebugLevel::DEBUG, "OpenGL context destroyed\n");
	}

	//----------------------------------------------------------------------------------------------

	bool UIContext::init(Window* window)
	{
		__super::init(window);

		// GL 3.0 + GLSL 410
		const char* glsl_version = "#version 410";

		// setup dear imgui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// set platform handlers
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;		// Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;			// Enable Multi-Viewport / Platform Windows
		io.ConfigDockingWithShift = true;
		//io.ConfigViewportsNoAutoMerge = true;
		io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)_window->getNativeWindow(), true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		// Setup Dear ImGui style
		setStyleDarkOrange();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			//style.WindowRounding = 0.0f;
			//style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		DBG("UI", DebugLevel::DEBUG, "UI context created successfully\n");
		return true;
	}

	void UIContext::preRender()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (_window->hasCustomTitlebar())
		{
			ImGuiWindowFlags titlebar_flags =
				ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar
				| ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollWithMouse;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();

			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(ImVec2{ viewport->WorkSize.x, 25.0f });
			ImGui::SetNextWindowViewport(viewport->ID);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{ 0.0f, 0.0f });
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 40.0f, 2.0f });
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.06f, 1.0f));

			ImGui::Begin("window-frame-titlebar", nullptr, titlebar_flags);
			ImGui::PopStyleVar(3);
			ImGui::PopStyleColor(1);

			//ImGui::Image(img_icon, ImVec2{ 1.0f, 1.0f });
			//ImGui::SetWindowFontScale(0.75f);
			//ImGui::PushFont(font_k12hl2);
			ImGui::TextColored(ImVec4{ 1.0f,1.0f,1.0f,1.0f }, _window->getTitle().c_str());
			//ImGui::PopFont();

			ImGui::End();
		}
		
		// Create the docking environment
		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();

		if (_window->hasCustomTitlebar())
		{
			ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + 25.0f));
			ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - 25.0f));
			ImGui::SetNextWindowViewport(viewport->ID);
		}
		else
		{
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
		}
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("MainDockspaceWindow", nullptr, windowFlags);
		ImGui::PopStyleVar(3);

		ImGuiID dockSpaceId = ImGui::GetID("MainDockspace");
		ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::End();

		if (_window->hasCustomTitlebar()) attemptDragWindow();
	}

	void UIContext::postRender()
	{
		// Dear imgui Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void UIContext::end()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		DBG("UI", DebugLevel::DEBUG, "UI context destroyed\n");
	}

	bool UIContext::disableTitlebar() {
#	ifdef _WIN32
		auto win = (GLFWwindow*)_window->getNativeWindow();
		if (win)
		{
			disableTitlebarWin32(win);
			DBG("UI", DebugLevel::DEBUG, "removed windows titlebar");
		}
		return true;
#	else
		DBG("UI", DebugLevel::WARNING ,"NOT IMPLEMENTED: Disable Titlebar not implemented for this platform!");
		return false;
#	endif
	}

	void UIContext::attemptDragWindow() {
		auto window = (GLFWwindow*)_window->getNativeWindow();

		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && dragState == 0) {
			glfwGetCursorPos(window, &s_xpos, &s_ypos);
			glfwGetWindowSize(window, &w_xsiz, &w_ysiz);
			dragState = 1;
		}
		if (glfwGetMouseButton(window, 0) == GLFW_PRESS && dragState == 1) {
			double c_xpos, c_ypos;
			int w_xpos, w_ypos;
			glfwGetCursorPos(window, &c_xpos, &c_ypos);
			glfwGetWindowPos(window, &w_xpos, &w_ypos);
			if (
				s_xpos >= 0 && s_xpos <= ((double)w_xsiz - 170) &&
				s_ypos >= 0 && s_ypos <= 25) {
				glfwSetWindowPos(window, w_xpos + (c_xpos - s_xpos + 6), w_ypos + (c_ypos - s_ypos + 30));
			}
			if (
				s_xpos >= ((double)w_xsiz - 15) && s_xpos <= ((double)w_xsiz) &&
				s_ypos >= ((double)w_ysiz - 15) && s_ypos <= ((double)w_ysiz)) {
				glfwSetWindowSize(window, w_xsiz + (c_xpos - s_xpos), w_ysiz + (c_ypos - s_ypos));
			}
		}
		if (glfwGetMouseButton(window, 0) == GLFW_RELEASE && dragState == 1) {
			dragState = 0;
		}
	}

	void UIContext::setStyleDarkOrange()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.80f, 0.41f, 0.24f, 0.0f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.202f, 0.202f, 0.202f, 0.240f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.41f, 0.24f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.41f, 0.24f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.72f, 0.20f, 0.14f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.68f, 0.35f, 0.20f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.06f, 0.06f, 0.06f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.06f, 0.06f, 0.06f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.80f, 0.41f, 0.24f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.296f, 0.404f, 0.497f, 0.290f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.80f, 0.41f, 0.24f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.80f, 0.42f, 0.24f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.64f, 0.27f, 0.10f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.46f, 0.93f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.76f, 0.29f, 0.08f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.86f, 0.49f, 0.33f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.86f, 0.49f, 0.33f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.86f, 0.49f, 0.33f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.75f, 0.75f, 0.75f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.22f, 0.22f, 0.22f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.22f, 0.22f, 0.22f, 0.35f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(5.00f, 2.00f);
		style.CellPadding = ImVec2(4.00f, 2.00f);
		style.ItemSpacing = ImVec2(6.00f, 4.00f);
		style.ItemInnerSpacing = ImVec2(3.00f, 4.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 21;
		style.ScrollbarSize = 13;
		style.GrabMinSize = 9;
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 1;
		style.TabBorderSize = 1;
		style.WindowRounding = 3;
		style.ChildRounding = 3;
		style.FrameRounding = 3;
		style.PopupRounding = 3;
		style.ScrollbarRounding = 3;
		style.GrabRounding = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding = 4;
		style.DockingSeparatorSize = 4;
		style.WindowMenuButtonPosition = ImGuiDir_Right;
	}
}