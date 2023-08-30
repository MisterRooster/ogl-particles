/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "ParticleSystem.h"


namespace nhahn
{
	void ParticleEmitter::emit(double dt, ParticleData* p)
	{
		const size_t maxNewParticles = static_cast<size_t>(dt * m_emitRate);
		const size_t startId = p->m_countAlive;
		const size_t endId = std::min(startId + maxNewParticles, p->m_count - 1);

		for (auto& gen : m_generators)
			gen->generate(dt, p, startId, endId);

		for (size_t i = startId; i < endId; ++i)
		{
			p->wake(i);
		}
	}

	ParticleSystem::ParticleSystem(size_t maxCount)
	{
		m_count = maxCount;
		m_particles.generate(maxCount);
		m_aliveParticles.generate(maxCount);

		for (size_t i = 0; i < maxCount; ++i)
			m_particles.m_alive[i] = false;
	}

	void ParticleSystem::update(double dt)
	{
		for (auto& em : m_emitters)
		{
			em->emit(dt, &m_particles);
		}

		for (size_t i = 0; i < m_count; ++i)
		{
			m_particles.m_acc[i] = glm::vec4(0.0f);
		}

		for (size_t i = 0; i < m_count; i += 2)
		{
			m_particles.m_acc[i] = glm::vec4(0.0f);
			m_particles.m_acc[i + 1] = glm::vec4(0.0f);
		}
		if (m_count % 2 != 0)
		{
			m_particles.m_acc[m_count - 1] = glm::vec4(0.0f);
		}

		for (auto& up : m_updaters)
		{
			up->update(dt, &m_particles);
		}

		double ratio = (double)m_particles.m_countAlive / (double)m_count;
		m_aliveToAllRatio = m_aliveToAllRatio * 0.5 + ratio * 0.5;

		//ParticleData::copyOnlyAlive(&m_particles, &m_aliveParticles);
	}

	void ParticleSystem::reset()
	{
		m_particles.m_countAlive = 0;
	}

	size_t ParticleSystem::computeMemoryUsage()
	{
		return 2 * ParticleData::computeMemoryUsage(m_particles);
	}
}