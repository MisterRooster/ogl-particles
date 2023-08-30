/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "ParticleGenerators.h"

#include <glm/gtc/random.hpp>
#include "utility/Utils.h"

#ifndef M_PI
	#define M_PI 		3.1415926535897932384626433832795f
	#define M_2_PI 		6.28318530717958647692528676655901f		// PI*2
#endif

#define RESTRICT __restrict

#define NO_RAND 0
#define RAND_DEFAULT 1
#define RAND_FAST 2
#define RAND_MODE RAND_FAST

static int RandSeed = 1;


namespace nhahn
{
	inline float randFloat(float a, float b)
	{
	#if RAND_MODE == RAND_DEFAULT
		return glm::linearRand(a, b);
	#elif RAND_MODE == NO_RAND	
		// to test the bottleneck:
		return a;
	#elif RAND_MODE == RAND_FAST
		// using fastest method?
		return (0.5f + 0.5f * Utils::sfrand()) * (b - a) + a;
	#endif
	}

	inline glm::vec4 randVec4(const glm::vec4& a, const glm::vec4& b)
	{
	#if RAND_MODE == RAND_DEFAULT
		return glm::vec4(glm::linearRand(a, b));
	#elif RAND_MODE == NO_RAND	
		// to test the bottleneck
		return glm::vec4(a);
	#elif RAND_MODE == RAND_FAST

		// using fastest method?
		return  glm::vec4(randFloat(a.x, b.x), randFloat(a.y, b.y), randFloat(a.z, b.z), randFloat(a.w, b.w));
	#endif
	}

	void BoxPosGen::generate(double dt, ParticleData* p, size_t startId, size_t endId)
	{
		glm::vec4 * RESTRICT pos = p->m_pos.get();

		const glm::vec4 posMin{ m_pos.x - m_maxStartPosOffset.x, m_pos.y - m_maxStartPosOffset.y, m_pos.z - m_maxStartPosOffset.z, 1.0 };
		const glm::vec4 posMax{ m_pos.x + m_maxStartPosOffset.x, m_pos.y + m_maxStartPosOffset.y, m_pos.z + m_maxStartPosOffset.z, 1.0 };

		for (size_t i = startId; i < endId; i += 2)
		{
			pos[i] = randVec4(posMin, posMax);
			pos[i + 1] = randVec4(posMin, posMax);
		}
		if (endId % 2 != 0)
		{
			pos[endId - 1] = randVec4(posMin, posMax);
		}
	}

	void RoundPosGen::generate(double dt, ParticleData* p, size_t startId, size_t endId)
	{
		glm::vec4* RESTRICT pos = p->m_pos.get();

		float ang;
		for (size_t i = startId; i < endId; i += 2)
		{
			ang = randFloat(0.0f, M_PI * 2.0f);
			pos[i] = glm::vec4(m_center + glm::vec4(m_radX * sinf(ang), m_radY * cosf(ang), 0.0f, 1.0f));

			ang = randFloat(0.0f, M_PI * 2.0f);
			pos[i + 1] = glm::vec4(m_center + glm::vec4(m_radX * sinf(ang), m_radY * cosf(ang), 0.0f, 1.0f));
		}
		if (endId % 2 != 0)
		{
			ang = randFloat(0.0f, M_PI * 2.0f);
			pos[endId - 1] = glm::vec4(m_center + glm::vec4(m_radX * sinf(ang), m_radY * cosf(ang), 0.0f, 1.0f));
		}
	}

	void SpherePosGen::generate(double dt, ParticleData* p, size_t startId, size_t endId)
	{
		glm::vec4* RESTRICT pos = p->m_pos.get();

		float rad;
		float phi;
		float theta;
		for (size_t i = startId; i < endId; i += 2)
		{
			phi = randFloat(0.0f, M_PI * 2.0f);
			theta = randFloat(0.0f, M_PI);
			rad = randFloat(0.0f, m_radius);
			pos[i] = glm::vec4(m_center + glm::vec4(rad * sinf(theta) * cosf(phi), rad * sinf(theta) * sinf(phi), rad * cosf(theta), 1.0f));

			phi = randFloat(0.0f, M_PI * 2.0f);
			theta = randFloat(0.0f, M_PI);
			rad = randFloat(0.0f, m_radius);
			pos[i + 1] = glm::vec4(m_center + glm::vec4(rad * sinf(theta) * cosf(phi), rad * sinf(theta) * sinf(phi), rad * cosf(theta), 1.0f));
		}
		if (endId % 2 != 0)
		{
			phi = randFloat(0.0f, M_PI * 2.0f);
			theta = randFloat(0.0f, M_PI);
			rad = randFloat(0.0f, m_radius);
			pos[endId - 1] = glm::vec4(m_center + glm::vec4(rad * sinf(theta) * cosf(phi), rad * sinf(theta) * sinf(phi), rad * cosf(theta), 1.0f));
		}
	}

	void BasicColorGen::generate(double dt, ParticleData* p, size_t startId, size_t endId)
	{
		glm::vec4* RESTRICT startCol = p->m_startCol.get();
		glm::vec4* RESTRICT endCol = p->m_endCol.get();

		for (size_t i = startId; i < endId; i += 2)
		{
			startCol[i] = randVec4(m_minStartCol, m_maxStartCol);
			endCol[i] = randVec4(m_minEndCol, m_maxEndCol);

			startCol[i + 1] = randVec4(m_minStartCol, m_maxStartCol);
			endCol[i + 1] = randVec4(m_minEndCol, m_maxEndCol);
		}
		if (endId % 2 != 0)
		{
			startCol[endId - 1] = randVec4(m_minStartCol, m_maxStartCol);
			endCol[endId - 1] = randVec4(m_minEndCol, m_maxEndCol);
		}
	}

	void BasicVelGen::generate(double dt, ParticleData* p, size_t startId, size_t endId)
	{
		glm::vec4* RESTRICT vel = p->m_vel.get();

		for (size_t i = startId; i < endId; i += 2)
		{
			vel[i] = randVec4(m_minStartVel, m_maxStartVel);
			vel[i + 1] = randVec4(m_minStartVel, m_maxStartVel);
		}
		if (endId % 2 != 0)
		{
			vel[endId - 1] = randVec4(m_minStartVel, m_maxStartVel);
		}
	}

	void SphereVelGen::generate(double dt, ParticleData* p, size_t startId, size_t endId)
	{
		glm::vec4* RESTRICT vel = p->m_vel.get();

		float phi, theta, v, r;
		for (size_t i = startId; i < endId; ++i)
		{
			phi = randFloat(-M_PI, M_PI);
			theta = randFloat(-M_PI, M_PI);
			v = randFloat(m_minVel, m_maxVel);

			r = v * sinf(phi);
			vel[i] = glm::vec4(v * cosf(phi), r * cosf(theta), r * sinf(theta), 1.0f);
		}
	}

	void VelFromPosGen::generate(double dt, ParticleData* p, size_t startId, size_t endId)
	{
		glm::vec4* RESTRICT veloc = p->m_vel.get();
		glm::vec4* RESTRICT pos = p->m_pos.get();

		glm::vec4 scalev;
		for (size_t i = startId; i < endId; ++i)
		{
			float scale = static_cast<float>(randFloat(m_minScale, m_maxScale));
			scalev = glm::vec4(scale, scale, scale, scale);
			glm::vec4 vel = (pos[i] - glm::vec4(m_offset));
			veloc[i] = glm::vec4(scalev * vel);
		}
	}

	void BasicTimeGen::generate(double dt, ParticleData* p, size_t startId, size_t endId)
	{
		glm::vec4 * RESTRICT time = p->m_time.get();

		for (size_t i = startId; i < endId; i += 2)
		{
			time[i].x = time[i].y = randFloat(m_minTime, m_maxTime);
			time[i].z = (float)0.0;
			time[i].w = (float)1.0 / time[i].x;

			time[i + 1].x = time[i + 1].y = randFloat(m_minTime, m_maxTime);
			time[i + 1].z = (float)0.0;
			time[i + 1].w = (float)1.0 / time[i + 1].x;
		}
		if (endId % 2 != 0)
		{
			time[endId - 1].x = time[endId - 1].y = randFloat(m_minTime, m_maxTime);
			time[endId - 1].z = (float)0.0;
			time[endId - 1].w = (float)1.0 / time[endId - 1].x;
		}
	}
}