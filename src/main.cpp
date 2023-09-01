/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/

#include "imgui.h"
#include "imgui_internal.h"
#include "Application.h"
#include "ui/SceneView.h"
#include "ui/PropertyPanel.h"
#include "utility/Debug.h"
#include "utility/FileSystem.h"
#include "utility/Utils.h"
#include "particles/Effect.h"

//--------------------------------------------------------------------------------------------------

namespace nhahn
{
	const double renderTickrate = 1.0 / 60.0;

	std::unique_ptr<SceneView> sceneView;
	std::unique_ptr<PropertyPanel> propertyPanel;
	std::shared_ptr<Texture> displayTex;

	std::shared_ptr<IEffect> _tunnelEffect;
	std::shared_ptr<IEffect> _attractorEffect;
	std::shared_ptr<IEffect> _fountainEffect;
	std::shared_ptr<IEffect> _burningEffect;

	void splitDockspace()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGui::Begin("MainDockspaceWindow");

			ImGuiID dockSpaceId = ImGui::GetID("MainDockspace");
			ImGuiID dockspace_id_copy = dockSpaceId;

			ImGui::DockBuilderRemoveNode(dockSpaceId); // clear any previous layout
			ImGui::DockBuilderAddNode(dockSpaceId, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockSpaceId, ImGui::GetMainViewport()->Size);

			auto dock_id_left = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.25f, nullptr, &dockSpaceId);
			ImGui::DockBuilderDockWindow("Properties", dock_id_left);
			ImGui::DockBuilderDockWindow("Scene View", dockSpaceId);
			ImGui::DockBuilderFinish(dockSpaceId);
			DBG("UI", DebugLevel::DEBUG, "docked child windows to main dockspace\n");

			ImGui::End();
		}
	}

	void render(double dt)
	{
		// split dockspace and dock subwindows on programs first run
		std::string path = nhahn::FileSystem::getModuleDirectory() + "imgui.ini";
		static auto firstCall = true;
		bool firstTimeEver = firstCall && !FileSystem::fileExists(path.c_str());
		if (firstTimeEver) {
			firstCall = false;
			splitDockspace();
		}

		// render ui elements
		sceneView->render(dt);
		propertyPanel->render();
		//ImGui::ShowDemoWindow();
	}
}

//------------------------------------ application entry point -------------------------------------

int main()
{
	using namespace nhahn;

	{
		Application app("Roosters OpenGL Particle System", true);
		app.setRenderCallback(render, renderTickrate);

		displayTex = std::make_shared<Texture>(TextureType::TEXTURE_2D, 640, 320);
		sceneView = std::make_unique<SceneView>(displayTex);
		propertyPanel = std::make_unique<PropertyPanel>();

		// particle effects
		_tunnelEffect = EffectFactory::create("tunnel");
		_tunnelEffect->initialize(200000);
		_tunnelEffect->initializeRenderer("gl");

		_attractorEffect = EffectFactory::create("attractors");
		_attractorEffect->initialize(100000);
		_attractorEffect->initializeRenderer("gl");

		_fountainEffect = EffectFactory::create("fountain");
		_fountainEffect->initialize(IEffect::DEFAULT_PARTICLE_NUM_FLAG);
		_fountainEffect->initializeRenderer("gl");

		_burningEffect = EffectFactory::create("burning");
		_burningEffect->initialize(200000);
		_burningEffect->initializeRenderer("gl");

		sceneView->setEffect(_fountainEffect.get());
		propertyPanel->addEffect("Fountain", _fountainEffect);
		propertyPanel->addEffect("Attractor", _attractorEffect);
		propertyPanel->addEffect("Tunnel", _tunnelEffect);
		propertyPanel->addEffect("Burning", _burningEffect);

		// notify scene view when settings change
		propertyPanel->setEffectSwitchedCallback([](std::shared_ptr<IEffect> eff) {
			sceneView->setEffect(eff.get());
		});

		// run main loop
		app.run();

		_tunnelEffect->clean();
		_fountainEffect->clean();
		_attractorEffect->clean();
		_burningEffect->clean();

		propertyPanel.reset();
		sceneView.reset();
		displayTex.reset();
	}

	exit(EXIT_SUCCESS);
}