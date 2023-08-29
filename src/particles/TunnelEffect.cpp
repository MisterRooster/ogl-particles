/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "TunnelEffect.h"

#include <string>
#include "imgui.h"
#include "utility/Debug.h"
#include "utility/Utils.h"


namespace nhahn
{
	bool TunnelEffect::initialize(size_t numParticles)
	{
		const size_t NUM_PARTICLES = numParticles == 0 ? IEffect::DEFAULT_PARTICLE_COUNT : numParticles;
		m_system = std::make_shared<ParticleSystem>(NUM_PARTICLES);

		// emitter
		auto particleEmitter = std::make_shared<ParticleEmitter>();
		{
			particleEmitter->m_emitRate = (float)NUM_PARTICLES * 0.45f;

			m_posGenerator = std::make_shared<RoundPosGen>();
			m_posGenerator->m_center = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
			m_posGenerator->m_radX = 0.15f;
			m_posGenerator->m_radY = 0.15f;
			particleEmitter->addGenerator(m_posGenerator);

			m_colGenerator = std::make_shared<BasicColorGen>();
			m_colGenerator->m_minStartCol = glm::vec4{ 0.344, 0.7, 0.0, 1.0 };
			m_colGenerator->m_maxStartCol = glm::vec4{ 0.65, 0.413, 0.07, 1.0 };
			m_colGenerator->m_minEndCol = glm::vec4{ 0.1, 0.6, 0.0, 0.0 };
			m_colGenerator->m_maxEndCol = glm::vec4{ 0.5, 0.975, 1.0, 0.0 };
			particleEmitter->addGenerator(m_colGenerator);

			auto velGenerator = std::make_shared<BasicVelGen>();
			velGenerator->m_minStartVel = glm::vec4{ 0.0f, 0.0f, 0.15f, 0.0f };
			velGenerator->m_maxStartVel = glm::vec4{ 0.0f, 0.0f, 0.45f, 0.0f };
			particleEmitter->addGenerator(velGenerator);

			auto timeGenerator = std::make_shared<BasicTimeGen>();
			timeGenerator->m_minTime = 1.0;
			timeGenerator->m_maxTime = 3.5;
			particleEmitter->addGenerator(timeGenerator);
		}
		m_system->addEmitter(particleEmitter);

		auto timeUpdater = std::make_shared<BasicTimeUpdater>();
		m_system->addUpdater(timeUpdater);

		auto colorUpdater = std::make_shared<BasicColorUpdater>();
		//colorUpdater->m_minPos = glm::vec4{ -1.0f };
		//colorUpdater->m_maxPos = glm::vec4{ 1.0f };
		m_system->addUpdater(colorUpdater);

		auto eulerUpdater = std::make_shared<EulerUpdater>();
		eulerUpdater->m_globalAcceleration = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
		m_system->addUpdater(eulerUpdater);

		DBG("TunnelEffect", DebugLevel::DEBUG, "Particles memory usage: %dmb\n", (int)(m_system->computeMemoryUsage() / (1024 * 1024)));
		return true;
	}

	bool TunnelEffect::initializeRenderer(const char* name)
	{
		m_renderer = ParticleRendererFactory::create(name);

		if (!m_renderer)
			return false;

		m_renderer->generate(m_system.get(), false);

		return true;
	}

	void TunnelEffect::clean()
	{
		if (m_renderer) m_renderer->destroy();
	}

	void TunnelEffect::update(double dt)
	{
		static double time = 0.0;
		time += dt;

		m_posGenerator->m_center.x = 0.1f * sinf((float)time * 2.5f);
		m_posGenerator->m_center.y = 0.1f * cosf((float)time * 2.5f);
		m_posGenerator->m_radX = 0.15f + 0.05f * sinf((float)time);
		m_posGenerator->m_radY = 0.15f + 0.05f * sinf((float)time) * cosf((float)time * 0.5f);
	}

	void TunnelEffect::cpuUpdate(double dt)
	{
		m_system->update(dt);
	}

	void TunnelEffect::gpuUpdate(double dt)
	{
		m_renderer->update();
	}

	void TunnelEffect::render()
	{
		m_renderer->render();
	}

	void TunnelEffect::renderUI()
	{
		ImGui::NewLine();
		ImGui::TextWrapped(
			"Fountain effect where the particles are affected by gravity, collide and bounces of the floor plane."
		);
		ImGui::Spacing();
		ImGui::NewLine();

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