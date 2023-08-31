/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "ParticleUpdaters.h"

#include <algorithm>
#include <glm/common.hpp>
#include <glm/gtc/random.hpp>
#include <xmmintrin.h>

#define SSE_MODE_NONE 0
#define SSE_MODE_SSE2 1
#define SSE_MODE_AVX 2
#define SSE_MODE SSE_MODE_AVX

#define RESTRICT __restrict


namespace nhahn
{
	void EulerUpdater::update(double dt, ParticleData* p)
	{
		const glm::vec4 globalA{ (float)dt * m_globalAcceleration.x, (float)dt * m_globalAcceleration.y, (float)dt * m_globalAcceleration.z, 0.0f };
		const float localDT = (float)dt;
		const unsigned int endId = p->m_countAlive;

		glm::vec4* RESTRICT acc = p->m_acc;
		glm::vec4* RESTRICT vel = p->m_vel;
		glm::vec4* RESTRICT pos = p->m_pos;

	#if SSE_MODE == SSE_MODE_NONE
		for (size_t i = 0; i < endId; ++i)
			acc[i] += globalA;

		for (size_t i = 0; i < endId; ++i)
			vel[i] += localDT * acc[i];

		for (size_t i = 0; i < endId; ++i)
			pos[i] += localDT * vel[i];
	#elif SSE_MODE == SSE_MODE_SSE2
		__m128 ga = *(__m128*)(&globalA.data);
		__m128* pa, * pb, pc;
		__m128 ldt = _mm_set_ps1(localDT);

		size_t i;
		for (i = 0; i < endId; i++)
		{
			pa = (__m128*)(&acc[i].x);
			*pa = _mm_add_ps(*pa, ga);
	}

		for (i = 0; i < endId; i++)
		{
			pa = (__m128*)(&vel[i].x);
			pb = (__m128*)(&acc[i].x);
			pc = _mm_mul_ps(*pb, ldt);
			*pa = _mm_add_ps(*pa, pc);
		}

		for (size_t i = 0; i < endId; i++)
		{
			pa = (__m128*)(&pos[i].x);
			pb = (__m128*)(&vel[i].x);
			pc = _mm_mul_ps(*pb, ldt);
			*pa = _mm_add_ps(*pa, pc);
		}
	#elif SSE_MODE == SSE_MODE_AVX
		__m256 ga = _mm256_set_m128(*(__m128*)(&globalA.data), *(__m128*)(&globalA.data));
		__m256* pa, * pb, pc;
		__m256 ldt = _mm256_set1_ps(localDT);
		size_t i;

		for (i = 0; i < endId; i += 2)
		{
			pa = (__m256*)(&acc[i].x);
			*pa = _mm256_add_ps(*pa, ga);
		}
		for (; i < endId; i++)
		{
			acc[i] += globalA;
		}

		for (i = 0; i < endId; i += 2)
		{
			pa = (__m256*)(&vel[i].x);
			pb = (__m256*)(&acc[i].x);
			pc = _mm256_mul_ps(*pb, ldt);
			*pa = _mm256_add_ps(*pa, pc);
		}
		for (; i < endId; i++)
		{
			vel[i] += localDT * acc[i];
		}

		for (size_t i = 0; i < endId; i += 2)
		{
			pa = (__m256*)(&pos[i].x);
			pb = (__m256*)(&vel[i].x);
			pc = _mm256_mul_ps(*pb, ldt);
			*pa = _mm256_add_ps(*pa, pc);
		}
		for (; i < endId; i++)
		{
			pos[i] += localDT * vel[i];
		}
	#endif // AVX
	}

	void FloorUpdater::update(double dt, ParticleData* p)
	{
		const float localDT = (float)dt;

		glm::vec4* RESTRICT acc = p->m_acc;
		glm::vec4* RESTRICT vel = p->m_vel;
		glm::vec4* RESTRICT pos = p->m_pos;

		const size_t endId = p->m_countAlive;
		for (size_t i = 0; i < endId; ++i)
		{
			if (pos[i].y < m_floorY)
			{
				glm::vec4 force = acc[i];
				float normalFactor = glm::dot(force, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
				if (normalFactor < 0.0f)
					force -= glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * normalFactor;

				float velFactor = glm::dot(vel[i], glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
				//if (velFactor < 0.0)
				vel[i] -= glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * (1.0f + m_bounceFactor) * velFactor;

				acc[i] = force;
			}
		}

	}

	inline float inverse(float x)
	{
		// re-interpret as a 32 bit integer
		unsigned int* i = (unsigned int*)&x;

		// adjust exponent
		//*i = 0x7F000000 - *i;
		*i = 0x7EEEEEEE - *i;
		return x;
	}

	inline float inverse2(float f)
	{
		int x = *reinterpret_cast<int*>(&f);
		x = 0x7EF311C2 - x;
		float inv = *reinterpret_cast<float*>(&x);
		//inv = inv * (2 - inv * f);
		return inv;
	}

	void AttractorUpdater::update(double dt, ParticleData* p)
	{
		const size_t countAttractors = m_attractors.size();
		glm::vec4 attr[8];
		for (size_t i = 0; i < countAttractors; ++i)
			attr[i] = glm::vec4(m_attractors[i]);

		const float localDT = (float)dt;

		glm::vec4* RESTRICT acc = p->m_acc;
		glm::vec4* RESTRICT vel = p->m_vel;
		glm::vec4* RESTRICT pos = p->m_pos;

		const size_t endId = p->m_countAlive;
		glm::vec4 off = glm::vec4(0.0f);
	#if SSE_MODE == SSE_MODE_NONE
		float dist;
	#elif SSE_MODE == SSE_MODE_SSE2 || SSE_MODE == SSE_MODE_AVX
		__m128 tempDist;
	#endif

		size_t a;
		for (size_t i = 0; i < endId; ++i)
		{
			for (a = 0; a < countAttractors; ++a)
			{
		#if SSE_MODE == SSE_MODE_NONE
				off = attr[a] - p->m_pos[i];
				dist = off.x * off.x + off.y * off.y + off.z * off.z;
				dist = attr[a].w / dist;// *inverse(dist);
				p->m_acc[i] += off * dist;
		#elif SSE_MODE == SSE_MODE_SSE2 || SSE_MODE == SSE_MODE_AVX
				off = attr[a] - p->m_pos[i];
				tempDist = _mm_dp_ps(*(__m128*)(&off.data), *(__m128*)(&off.data), 0x71);
				tempDist.m128_f32[0] = attr[a].w / tempDist.m128_f32[0];// *inverse2(fabs(tempDist.m128_f32[0]);
				p->m_acc[i] += off * tempDist.m128_f32[0];
		#endif
			}
		}
	}

	void BasicColorUpdater::update(double dt, ParticleData* p)
	{
		const size_t endId = p->m_countAlive;

		glm::vec4* RESTRICT col = p->m_col;
		glm::vec4* RESTRICT startCol = p->m_startCol;
		glm::vec4* RESTRICT endCol = p->m_endCol;
		glm::vec4* RESTRICT ti = p->m_time;

	#if SSE_MODE == SSE_MODE_NONE
		for (size_t i = 0; i < endId; ++i)
			col[i] = glm::mix(startCol[i], endCol[i], glm::vec4(ti[i].z));
	#elif SSE_MODE == SSE_MODE_SSE2 || SSE_MODE == SSE_MODE_AVX
		__m128 x, y;
		__m128 t;

		for (size_t i = 0; i < endId; i++)
		{
			t = _mm_set1_ps(ti[i].z);
			x = _mm_sub_ps(*(__m128*)(&endCol[i].data), *(__m128*)(&startCol[i].data));
			y = _mm_mul_ps(t, x);
			*(__m128*)(&col[i].data) = _mm_add_ps(*(__m128*)(&startCol[i].data), y);
		}
	#endif
	}

	void PosColorUpdater::update(double dt, ParticleData* p)
	{
		glm::vec4* RESTRICT pos = p->m_pos;
		glm::vec4* RESTRICT col = p->m_col;
		glm::vec4* RESTRICT startCol = p->m_startCol;
		glm::vec4* RESTRICT endCol = p->m_endCol;
		glm::vec4* RESTRICT time = p->m_time;

		const size_t endId = p->m_countAlive;
		float scaler, scaleg, scaleb;
		float diffr = m_maxPos.x - m_minPos.x;
		float diffg = m_maxPos.y - m_minPos.y;
		float diffb = m_maxPos.z - m_minPos.z;
		for (size_t i = 0; i < endId; ++i)
		{
			scaler = (pos[i].x - m_minPos.x) / diffr;
			scaleg = (pos[i].y - m_minPos.y) / diffg;
			scaleb = (pos[i].z - m_minPos.z) / diffb;
			col[i].r = scaler;// glm::mix(p->m_startCol[i].r, p->m_endCol[i].r, scaler);
			col[i].g = scaleg;// glm::mix(p->m_startCol[i].g, p->m_endCol[i].g, scaleg);
			col[i].b = scaleb;// glm::mix(p->m_startCol[i].b, p->m_endCol[i].b, scaleb);
			col[i].a = glm::mix(startCol[i].a, endCol[i].a, time[i].z);
		}
	}

	void VelColorUpdater::update(double dt, ParticleData* p)
	{
		glm::vec4* RESTRICT vel = p->m_vel;
		glm::vec4* RESTRICT col = p->m_col;
		glm::vec4* RESTRICT startCol = p->m_startCol;
		glm::vec4* RESTRICT endCol = p->m_endCol;
		glm::vec4* RESTRICT time = p->m_time;

		const size_t endId = p->m_countAlive;
		float scaler, scaleg, scaleb;
		float diffr = m_maxVel.x - m_minVel.x;
		float diffg = m_maxVel.y - m_minVel.y;
		float diffb = m_maxVel.z - m_minVel.z;
		for (size_t i = 0; i < endId; ++i)
		{
			scaler = (vel[i].x - m_minVel.x) / diffr;
			scaleg = (vel[i].y - m_minVel.y) / diffg;
			scaleb = (vel[i].z - m_minVel.z) / diffb;
			col[i].r = scaler;// glm::mix(p->m_startCol[i].r, p->m_endCol[i].r, scaler);
			col[i].g = scaleg;// glm::mix(p->m_startCol[i].g, p->m_endCol[i].g, scaleg);
			col[i].b = scaleb;// glm::mix(p->m_startCol[i].b, p->m_endCol[i].b, scaleb);
			col[i].a = glm::mix(startCol[i].a, endCol[i].a, time[i].z);
		}
	}

	void BasicTimeUpdater::update(double dt, ParticleData* p)
	{
		unsigned int endId = p->m_countAlive;
		const float localDT = (float)dt;

		if (endId == 0) return;

		glm::vec4* RESTRICT t = p->m_time;

		for (size_t i = 0; i < endId; i += 2)
		{
			t[i].x -= localDT;
			// interpolation: from 0 (start of life) till 1 (end of life)
			t[i].z = 1.0f - (t[i].x * t[i].w); // .w is 1.0/max life time	

			t[i + 1].x -= localDT;
			t[i + 1].z = 1.0f - (t[i + 1].x * t[i + 1].w); // .w is 1.0/max life time	
		}

		if (endId % 2 != 0)
		{
			t[endId - 1].x -= localDT;
			t[endId - 1].z = 1.0f - (t[endId - 1].x * t[endId - 1].w); // .w is 1.0/max life time
		}

		for (size_t i = 0; i < endId; ++i)
		{
			if (t[i].x < 0.0f)
			{
				p->kill(i);
				endId = p->m_countAlive < p->m_count ? p->m_countAlive : p->m_count;
			}
		}
	}
}