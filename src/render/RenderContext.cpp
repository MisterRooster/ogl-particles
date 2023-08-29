/*------------------------------------------------------------------------------------------------*\
| verlet_basic
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
#include "GLFW/glfw3.h"


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

		// Create the docking environment
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
		ImGui::PopStyleVar(3);
		ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");
		ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::End();
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

	void UIContext::setStyleDarkOrange()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
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
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.80f, 0.41f, 0.24f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
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
	}
}