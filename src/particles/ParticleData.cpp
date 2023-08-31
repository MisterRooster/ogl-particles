/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "ParticleData.h"


namespace nhahn
{
	ParticleData::~ParticleData()
	{
		_aligned_free(m_pos);
		_aligned_free(m_col);
		_aligned_free(m_startCol);
		_aligned_free(m_endCol);
		_aligned_free(m_vel);
		_aligned_free(m_acc);
		_aligned_free(m_time);
	}

	void ParticleData::generate(size_t maxSize)
	{
		m_count = maxSize;
		m_countAlive = 0;

		static_assert(sizeof(glm::vec4) == 4 * sizeof(float), "size is 16...");

		m_pos = (glm::vec4*)_aligned_malloc(sizeof(glm::vec4) * maxSize, 16);
		m_col = (glm::vec4*)_aligned_malloc(sizeof(glm::vec4) * maxSize, 16);
		m_startCol = (glm::vec4*)_aligned_malloc(sizeof(glm::vec4) * maxSize, 16);
		m_endCol = (glm::vec4*)_aligned_malloc(sizeof(glm::vec4) * maxSize, 16);
		m_vel = (glm::vec4*)_aligned_malloc(sizeof(glm::vec4) * maxSize, 16);
		m_acc = (glm::vec4*)_aligned_malloc(sizeof(glm::vec4) * maxSize, 16);
		m_time = (glm::vec4*)_aligned_malloc(sizeof(glm::vec4) * maxSize, 16);

		m_alive.reset(new bool[maxSize]);
	}

	void ParticleData::kill(size_t id)
	{
		m_alive[id] = false;
		swapData(id, m_countAlive - 1);
		m_countAlive--;
	}

	void ParticleData::wake(size_t id)
	{

		m_alive[id] = true;
		swapData(id, m_countAlive);
		m_countAlive++;
	}

	void ParticleData::swapData(size_t a, size_t b)
	{
		m_pos[a] = m_pos[b];
		m_col[a] = m_col[b];
		m_startCol[a] = m_startCol[b];
		m_endCol[a] = m_endCol[b];
		m_vel[a] = m_vel[b];
		m_acc[a] = m_acc[b];
		m_time[a] = m_time[b];
	}

	void ParticleData::copyOnlyAlive(const ParticleData* source, ParticleData* destination)
	{
		assert(source->m_count == destination->m_count);

		size_t id = 0;
		for (size_t i = 0; i < source->m_countAlive; ++i)
		{
			//if (source->m_alive[i])
			{
				destination->m_pos[id] = source->m_pos[i];
				destination->m_col[id] = source->m_col[i];
				destination->m_startCol[id] = source->m_startCol[i];
				destination->m_endCol[id] = source->m_endCol[i];
				destination->m_vel[id] = source->m_vel[i];
				destination->m_acc[id] = source->m_acc[i];
				destination->m_time[id] = source->m_time[i];
				destination->m_alive[id] = true;
				id++;
			}
		}

		destination->m_countAlive = id;
	}

	size_t ParticleData::computeMemoryUsage(const ParticleData& p)
	{
		return p.m_count * (sizeof(glm::vec4) * 7 + sizeof(bool)) + sizeof(size_t) * 2;
	}
}