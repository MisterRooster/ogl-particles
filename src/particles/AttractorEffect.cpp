/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "AttractorEffect.h"

#include <string>
#include "imgui.h"
#include "utility/Debug.h"
#include "ui/CustomWidgets.h"


namespace nhahn
{
	bool AttractorEffect::initialize(size_t numParticles)
	{
		const size_t NUM_PARTICLES = numParticles == 0 ? IEffect::DEFAULT_PARTICLE_COUNT : numParticles;
		m_system = std::make_shared<ParticleSystem>(NUM_PARTICLES);

		// common
		m_colGenerator = std::make_shared<BasicColorGen>();
		m_colGenerator->m_minStartCol = glm::vec4{ 0.99f, 0.99f, 0.99f, 1.0f };
		m_colGenerator->m_maxStartCol = glm::vec4{ 0.99f, 0.99f, 1.0f, 1.0f };
		m_colGenerator->m_minEndCol = glm::vec4{ 0.99f, 0.99f, 0.99f, 0.0f };
		m_colGenerator->m_maxEndCol = glm::vec4{ 0.99f, 0.99f, 1.0f, 0.25f };

		auto velGenerator = std::make_shared<SphereVelGen>();
		velGenerator->m_minVel = 0.1f;
		velGenerator->m_maxVel = 0.1f;

		auto timeGenerator = std::make_shared<BasicTimeGen>();
		timeGenerator->m_minTime = 1.6f;
		timeGenerator->m_maxTime = 4.0f;

		// emitter 1
		auto particleEmitter = std::make_shared<ParticleEmitter>();
		{
			particleEmitter->m_emitRate = (float)NUM_PARTICLES * 0.1f;

			m_posGenerators[0] = std::make_shared<BoxPosGen>();
			m_posGenerators[0]->m_pos = glm::vec4{ 0.0, 0.0, -0.25, 0.0 };
			m_posGenerators[0]->m_maxStartPosOffset = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
			particleEmitter->addGenerator(m_posGenerators[0]);

			particleEmitter->addGenerator(m_colGenerator);
			particleEmitter->addGenerator(velGenerator);
			particleEmitter->addGenerator(timeGenerator);
		}
		m_system->addEmitter(particleEmitter);

		// emitter 2
		auto particleEmitter2 = std::make_shared<ParticleEmitter>();
		{
			particleEmitter2->m_emitRate = (float)NUM_PARTICLES * 0.1f;

			m_posGenerators[1] = std::make_shared<BoxPosGen>();
			m_posGenerators[1]->m_pos = glm::vec4{ 0.0, 0.0, 0.25, 0.0 };
			m_posGenerators[1]->m_maxStartPosOffset = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
			particleEmitter2->addGenerator(m_posGenerators[1]);

			particleEmitter2->addGenerator(m_colGenerator);
			particleEmitter2->addGenerator(velGenerator);
			particleEmitter2->addGenerator(timeGenerator);
		}
		m_system->addEmitter(particleEmitter2);

		// emitter 3
		auto particleEmitter3 = std::make_shared<ParticleEmitter>();
		{
			particleEmitter3->m_emitRate = (float)NUM_PARTICLES * 0.1f;

			m_posGenerators[2] = std::make_shared<BoxPosGen>();
			m_posGenerators[2]->m_pos = glm::vec4{ 0.0f, 0.0f, 0.25f, 0.0f };
			m_posGenerators[2]->m_maxStartPosOffset = glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f };
			particleEmitter3->addGenerator(m_posGenerators[2]);

			particleEmitter3->addGenerator(m_colGenerator);
			particleEmitter3->addGenerator(velGenerator);
			particleEmitter3->addGenerator(timeGenerator);
		}
		m_system->addEmitter(particleEmitter3);

		// updaters
		auto timeUpdater = std::make_shared<BasicTimeUpdater>();
		m_system->addUpdater(timeUpdater);

		auto colorUpdater = std::make_shared<VelColorUpdater>();
		colorUpdater->m_minVel = glm::vec4{ -0.5f, -0.5f, -0.5f, 0.0f };
		colorUpdater->m_maxVel = glm::vec4{ 2.0f, 2.0f, 2.0f, 2.0f };
		m_system->addUpdater(colorUpdater);

		m_attractors = std::make_shared<AttractorUpdater>();
		m_attractors->add(glm::vec4{ 0.0, 0.0, 0.75, 1.0 });
		m_attractors->add(glm::vec4{ 0.0, 0.0, -0.75, 1.0 });
		m_attractors->add(glm::vec4{ 0.0, 0.75, 0.0, 1.0 });
		m_attractors->add(glm::vec4{ 0.0, -0.75, 0.0, 1.0 });
		m_system->addUpdater(m_attractors);

		auto eulerUpdater = std::make_shared<EulerUpdater>();
		eulerUpdater->m_globalAcceleration = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
		m_system->addUpdater(eulerUpdater);

		m_zScale = 1.0f;

		DBG("AttractorEffect", DebugLevel::DEBUG, "Particles memory usage: %dmb\n", (int)(m_system->computeMemoryUsage() / (1024 * 1024)));
		return true;
	}

	bool AttractorEffect::initializeRenderer(const char* name)
	{
		m_renderer = ParticleRendererFactory::create(name);
		m_renderer->generate(m_system.get(), false);

		return true;
	}

	void AttractorEffect::clean()
	{
		if (m_renderer) m_renderer->destroy();
	}

	void AttractorEffect::update(double dt)
	{
		static double time = 0.0;
		time += dt;

		m_posGenerators[0]->m_pos.x = 0.15f * sinf((float)time * 2.5f);
		m_posGenerators[0]->m_pos.y = 0.15f * cosf((float)time * 2.5f);
		m_posGenerators[0]->m_pos.z = m_zScale * 0.25f * cosf((float)time * 2.5f);

		m_posGenerators[1]->m_pos.x = -0.15f * sinf((float)time * 2.f);
		m_posGenerators[1]->m_pos.y = 0.15f * cosf((float)time * 2.f);
		m_posGenerators[1]->m_pos.z = m_zScale * 0.25f * cosf((float)time * 1.5f);

		m_posGenerators[2]->m_pos.x = -0.15f * sinf((float)time * 1.5f);
		m_posGenerators[2]->m_pos.y = 0.15f * cosf((float)time * 2.5f);
		m_posGenerators[2]->m_pos.z = m_zScale * 0.25f * cosf((float)time * 1.75f);
	}

	void AttractorEffect::cpuUpdate(double dt)
	{
		m_system->update(dt);
	}

	void AttractorEffect::gpuUpdate(double dt)
	{
		m_renderer->update();
	}

	void AttractorEffect::render()
	{
		m_renderer->render();
	}

	void AttractorEffect::renderUI()
	{
		ImGui::NewLine();
		ImGui::TextWrapped(
			"Effect with four attractor points at different positions which exerts a force on the particles."
		);
		ImGui::Spacing();
		ImGui::NewLine();

		ImGui::SeparatorText("Settings:");

		ImGui::SliderFloat("z scale", &m_zScale, 0.0f, 1.0f);
		ImGui::SameLine(); ImGui::HelpMarker("CTRL+click to input value.");

		ImGui::SeparatorText("Colors:");

		ImGui::ColorEdit4("start color min", &m_colGenerator->m_minStartCol.x);
		ImGui::SameLine(); ImGui::HelpMarker(
			"Click on the color square to open a color picker.\n"
			"Click and hold to use drag and drop.\n"
			"Right-click on the color square to show options.\n"
			"CTRL+click on individual component to input value.\n");

		ImGui::ColorEdit4("start color max", &m_colGenerator->m_maxStartCol.x);

		ImGui::ColorEdit4("end color min", &m_colGenerator->m_minEndCol.x);
		ImGui::ColorEdit4("end color max", &m_colGenerator->m_maxEndCol.x);

		ImGui::SeparatorText("Forces:");

		for (size_t i = 0; i < m_attractors->collectionSize(); ++i)
		{
			std::string text = "attractor " + std::to_string(i + 1);
			ImGui::SliderFloat(text.c_str(), &(m_attractors->get(i)).w, -1.0f, 1.0f, "%.2f");
		}
	}
}