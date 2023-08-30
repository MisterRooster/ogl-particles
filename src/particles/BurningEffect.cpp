/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "BurningEffect.h"

#include <string>
#include "imgui.h"
#include "utility/Debug.h"
#include "utility/Utils.h"


namespace nhahn
{
	bool BurningEffect::initialize(size_t numParticles)
	{
		const size_t NUM_PARTICLES = numParticles == 0 ? IEffect::DEFAULT_PARTICLE_COUNT : numParticles;
		m_system = std::make_shared<ParticleSystem>(NUM_PARTICLES);

		// emitter
		auto particleEmitter = std::make_shared<ParticleEmitter>();
		{
			particleEmitter->m_emitRate = (float)NUM_PARTICLES * 0.45f;

			m_posGenerator = std::make_shared<SpherePosGen>();
			m_posGenerator->m_center = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
			m_posGenerator->m_radius = 0.1f;
			particleEmitter->addGenerator(m_posGenerator);

			m_colGenerator = std::make_shared<BasicColorGen>();
			m_colGenerator->m_minStartCol = glm::vec4{ 0, 0.69, 1.0, 1.0 };
			m_colGenerator->m_maxStartCol = glm::vec4{ 0.74, 0.93, 1.0, 1.0 };
			m_colGenerator->m_minEndCol = glm::vec4{ 1.0, 0.49, 0.0, 0.0 };
			m_colGenerator->m_maxEndCol = glm::vec4{ 1.0, 0.26, 0.0, 0.0 };
			particleEmitter->addGenerator(m_colGenerator);

			auto velGenerator = std::make_shared<BasicVelGen>();
			velGenerator->m_minStartVel = glm::vec4{ -0.05f, -0.05f, -0.05f, 0.0f };
			velGenerator->m_maxStartVel = glm::vec4{ 0.05f, 0.05f, 0.05f, 0.0f };
			particleEmitter->addGenerator(velGenerator);

			auto timeGenerator = std::make_shared<BasicTimeGen>();
			timeGenerator->m_minTime = 1.0;
			timeGenerator->m_maxTime = 5.5;
			particleEmitter->addGenerator(timeGenerator);
		}
		m_system->addEmitter(particleEmitter);

		auto timeUpdater = std::make_shared<BasicTimeUpdater>();
		m_system->addUpdater(timeUpdater);

		auto colorUpdater = std::make_shared<BasicColorUpdater>();
		m_system->addUpdater(colorUpdater);

		m_eulerUpdater = std::make_shared<EulerUpdater>();
		m_eulerUpdater->m_globalAcceleration = glm::vec4{ 0.0, 5.0, 0.0, 0.0 };
		m_system->addUpdater(m_eulerUpdater);

		DBG("BurningEffect", DebugLevel::DEBUG, "Particles memory usage: %dmb\n", (int)(m_system->computeMemoryUsage() / (1024 * 1024)));
		return true;
	}

	bool BurningEffect::initializeRenderer(const char* name)
	{
		m_renderer = ParticleRendererFactory::create(name);

		if (!m_renderer)
			return false;

		m_renderer->generate(m_system.get(), false);

		return true;
	}

	void BurningEffect::clean()
	{
		if (m_renderer) m_renderer->destroy();
	}

	void BurningEffect::update(double dt)
	{
		static double time = 0.0;
		time += dt;

		m_posGenerator->m_center.x = 0.2f * sinf((float)time * 1.1f);
		m_posGenerator->m_center.z = 0.2f * cosf((float)time * 1.74f);
		m_posGenerator->m_radius = 0.1f + 0.01f * sinf((float)time);
	}

	void BurningEffect::cpuUpdate(double dt)
	{
		m_system->update(dt);
	}

	void BurningEffect::gpuUpdate(double dt)
	{
		m_renderer->update();
	}

	void BurningEffect::render()
	{
		m_renderer->render();
	}

	void BurningEffect::renderUI()
	{
		ImGui::NewLine();
		ImGui::TextWrapped(
			"Burning Effect where particles spawn from a spere and rise up."
		);
		ImGui::Spacing();
		ImGui::NewLine();

		ImGui::SeparatorText("Settings:");

		ImGui::SliderFloat("rise speed", &m_eulerUpdater->m_globalAcceleration.y, 0.0f, 20.0f, "%.2f");
		ImGui::SameLine(); Utils::UIHelpMarker("CTRL+click to input value.");

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