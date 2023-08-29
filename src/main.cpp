/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/

#include "imgui.h"
#include "Application.h"
#include "ui/SceneView.h"
#include "ui/PropertyPanel.h"
#include "utility/Debug.h"
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

	void render(double dt)
	{
		// render ui elements
		sceneView->render(dt);
		propertyPanel->render();
	}
}

//------------------------------------ application entry point -------------------------------------

int main()
{
	using namespace nhahn;

	{
		Application app("Particle System");
		app.setRenderCallback(render, renderTickrate);

		displayTex = std::make_shared<Texture>(TextureType::TEXTURE_2D, 640, 320);
		sceneView = std::make_unique<SceneView>(displayTex);
		propertyPanel = std::make_unique<PropertyPanel>();

		// particle effects
		_tunnelEffect = EffectFactory::create("tunnel");
		_tunnelEffect->initialize(IEffect::DEFAULT_PARTICLE_NUM_FLAG);
		_tunnelEffect->initializeRenderer("gl");

		_attractorEffect = EffectFactory::create("attractors");
		_attractorEffect->initialize(IEffect::DEFAULT_PARTICLE_NUM_FLAG);
		_attractorEffect->initializeRenderer("gl");

		_fountainEffect = EffectFactory::create("fountain");
		_fountainEffect->initialize(IEffect::DEFAULT_PARTICLE_NUM_FLAG);
		_fountainEffect->initializeRenderer("gl");

		sceneView->setEffect(_attractorEffect.get());
		propertyPanel->addEffect("Attractor", _attractorEffect);
		propertyPanel->addEffect("Fountain", _fountainEffect);
		propertyPanel->addEffect("Tunnel", _tunnelEffect);

		// notify scene view when settings change
		propertyPanel->setEffectSwitchedCallback([](std::shared_ptr<IEffect> eff) {
			sceneView->setEffect(eff.get());
		});

		// run main loop
		app.run();

		_tunnelEffect->clean();
		_fountainEffect->clean();
		_attractorEffect->clean();

		propertyPanel.reset();
		sceneView.reset();
		displayTex.reset();
	}

	exit(EXIT_SUCCESS);
}