/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "FountainEffect.h"

#include <string>
#include "imgui.h"
#include "utility/Debug.h"
#include "utility/Utils.h"


namespace nhahn
{
	bool FountainEffect::initialize(size_t numParticles)
	{
		const size_t NUM_PARTICLES = numParticles == 0 ? IEffect::DEFAULT_PARTICLE_COUNT : numParticles;
		m_system = std::make_shared<ParticleSystem>(NUM_PARTICLES);

		// emitter:
		auto particleEmitter = std::make_shared<ParticleEmitter>();
		{
			particleEmitter->m_emitRate = (float)NUM_PARTICLES * 0.25f;

			m_posGenerator = std::make_shared<BoxPosGen>();
			m_posGenerator->m_pos = glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f };
			m_posGenerator->m_maxStartPosOffset = glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f };
			particleEmitter->addGenerator(m_posGenerator);

			m_colGenerator = std::make_shared<BasicColorGen>();
			m_colGenerator->m_minStartCol = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
			m_colGenerator->m_maxStartCol = glm::vec4{ 0.013f, 0.036f, 0.153f, 1.0f };
			m_colGenerator->m_minEndCol = glm::vec4{ 0.590f, 0.316f, 0.013f, 0.0f };
			m_colGenerator->m_maxEndCol = glm::vec4{ 0.350f, 0.0f, 0.0f, 0.0f };
			particleEmitter->addGenerator(m_colGenerator);

			auto velGenerator = std::make_shared<BasicVelGen>();
			velGenerator->m_minStartVel = glm::vec4{ -0.05f, 0.22f, -0.05f, 0.0f };
			velGenerator->m_maxStartVel = glm::vec4{ 0.05f, 0.25f, 0.05f, 0.0f };
			particleEmitter->addGenerator(velGenerator);

			auto timeGenerator = std::make_shared<BasicTimeGen>();
			timeGenerator->m_minTime = 3.0f;
			timeGenerator->m_maxTime = 4.0f;
			particleEmitter->addGenerator(timeGenerator);
		}
		m_system->addEmitter(particleEmitter);

		auto timeUpdater = std::make_shared<BasicTimeUpdater>();
		m_system->addUpdater(timeUpdater);

		auto colorUpdater = std::make_shared<BasicColorUpdater>();
		m_system->addUpdater(colorUpdater);

		m_eulerUpdater = std::make_shared<EulerUpdater>();
		m_eulerUpdater->m_globalAcceleration = glm::vec4{ 0.0, -12.0, 0.0, 0.0 };
		m_system->addUpdater(m_eulerUpdater);

		m_floorUpdater = std::make_shared<FloorUpdater>();
		m_system->addUpdater(m_floorUpdater);

		DBG("FountainEffect", DebugLevel::DEBUG, "Particles memory usage: %dmb\n", (int)(m_system->computeMemoryUsage() / (1024 * 1024)));
		return true;
	}

	bool FountainEffect::initializeRenderer(const char* name)
	{
		m_renderer = ParticleRendererFactory::create(name);
		m_renderer->generate(m_system.get(), false);

		return true;
	}

	void FountainEffect::clean()
	{
		if (m_renderer) m_renderer->destroy();
	}

	void FountainEffect::update(double dt)
	{
		static double time = 0.0;
		time += dt;

		m_posGenerator->m_pos.x = 0.1f * sinf((float)time * 2.5f);
		m_posGenerator->m_pos.z = 0.1f * cosf((float)time * 2.5f);
	}

	void FountainEffect::cpuUpdate(double dt)
	{
		m_system->update(dt);
	}

	void FountainEffect::gpuUpdate(double dt)
	{
		m_renderer->update();
	}

	void FountainEffect::render()
	{
		m_renderer->render();
	}

	void FountainEffect::renderUI()
	{
		ImGui::NewLine();
		ImGui::TextWrapped(
			"Fountain effect where the particles are affected by gravity, collide and bounces of the floor plane."
		);
		ImGui::Spacing();
		ImGui::NewLine();

		ImGui::SeparatorText("Settings:");

		ImGui::SliderFloat("gravity", &m_eulerUpdater->m_globalAcceleration.y, -20.0f, 0.0f, "%.2f");
		ImGui::SameLine(); Utils::UIHelpMarker("CTRL+click to input value.");

		ImGui::SliderFloat("bounce", &m_floorUpdater->m_bounceFactor, 0.0f, 1.0f, "%.3f");

		ImGui::SeparatorText("Colors:");

		ImGui::ColorEdit4("start color min", &m_colGenerator->m_minStartCol.x);
		ImGui::SameLine(); Utils::UIHelpMarker(
			"Click on the color square to open a color picker.\n"
			"Click and hold to use drag and drop.\n"
			"Right-click on the color square to show options.\n"
			"CTRL+click on individual component to input value.\n");

		ImGui::ColorEdit4("start color max", &m_colGenerator->m_maxStartCol.x);

		ImGui::ColorEdit4("end color min", &m_colGenerator->m_minEndCol.x);
		ImGui::ColorEdit4("end color max", &m_colGenerator->m_maxEndCol.x);
	}
}