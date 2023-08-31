/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#ifndef GLM_FORCE_INTRINSICS
#define GLM_FORCE_INTRINSICS
#endif // !GLM_FORCE_INTRINSICS
#include <glm/vec4.hpp>

#include "ParticleSystem.h"


namespace nhahn
{
	class BoxPosGen : public ParticleGenerator
	{
	public:
		BoxPosGen() : m_pos(0.0), m_maxStartPosOffset(0.0) { }

		virtual void generate(double dt, ParticleData* p, size_t startId, size_t endId) override;

	public:
		glm::vec4 m_pos;
		glm::vec4 m_maxStartPosOffset;
	};

	class RoundPosGen : public ParticleGenerator
	{
	public:
		RoundPosGen() : m_center(0.0), m_radX(0.0), m_radY(0.0) { }
		RoundPosGen(const glm::vec4& center, double radX, double radY)
			: m_center(center), m_radX((float)radX), m_radY((float)radY)
		{ }

		virtual void generate(double dt, ParticleData* p, size_t startId, size_t endId) override;

	public:
		glm::vec4 m_center;
		float m_radX;
		float m_radY;
	};

	class SpherePosGen : public ParticleGenerator
	{
	public:
		SpherePosGen() : m_center(0.0), m_radius(0.0) { }
		SpherePosGen(const glm::vec4& center, double radius)
			: m_center(center), m_radius((float)radius)
		{ }

		virtual void generate(double dt, ParticleData* p, size_t startId, size_t endId) override;

	public:
		glm::vec4 m_center;
		float m_radius;
	};

	class BasicColorGen : public ParticleGenerator
	{
	public:
		BasicColorGen() : m_minStartCol(0.0), m_maxStartCol(0.0), m_minEndCol(0.0), m_maxEndCol(0.0) { }

		virtual void generate(double dt, ParticleData* p, size_t startId, size_t endId) override;

	public:
		glm::vec4 m_minStartCol;
		glm::vec4 m_maxStartCol;
		glm::vec4 m_minEndCol;
		glm::vec4 m_maxEndCol;
	};

	class BasicVelGen : public ParticleGenerator
	{
	public:
		BasicVelGen() : m_minStartVel(0.0), m_maxStartVel(0.0) { }

		virtual void generate(double dt, ParticleData* p, size_t startId, size_t endId) override;

	public:
		glm::vec4 m_minStartVel;
		glm::vec4 m_maxStartVel;
	};

	class SphereVelGen : public ParticleGenerator
	{
	public:
		SphereVelGen() : m_minVel(0.0), m_maxVel(0.0) { }

		virtual void generate(double dt, ParticleData* p, size_t startId, size_t endId) override;

	public:
		float m_minVel;
		float m_maxVel;
	};

	class VelFromPosGen : public ParticleGenerator
	{
	public:
		VelFromPosGen() : m_offset(0.0), m_minScale(0.0), m_maxScale(0.0) { }
		VelFromPosGen(const glm::vec4& off, double minS, double maxS)
			: m_offset(off), m_minScale((float)minS), m_maxScale((float)maxS)
		{ }

		virtual void generate(double dt, ParticleData* p, size_t startId, size_t endId) override;

	public:
		glm::vec4 m_offset;
		float m_minScale;
		float m_maxScale;
	};

	class BasicTimeGen : public ParticleGenerator
	{
	public:
		BasicTimeGen() : m_minTime(0.0), m_maxTime(0.0) { }

		virtual void generate(double dt, ParticleData* p, size_t startId, size_t endId) override;

	public:
		float m_minTime;
		float m_maxTime;
	};
}