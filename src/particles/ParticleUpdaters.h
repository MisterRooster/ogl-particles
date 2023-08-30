/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <glm/vec4.hpp>
#include "ParticleSystem.h"


namespace nhahn
{
	class EulerUpdater : public ParticleUpdater
	{
	public:
		virtual void update(double dt, ParticleData* p) override;

	public:
		glm::vec4 m_globalAcceleration{ 0.0f };
	};

	// collision with the floor - note: not a proper collision model
	class FloorUpdater : public ParticleUpdater
	{
	public:
		virtual void update(double dt, ParticleData* p) override;

	public:
		float m_floorY{ 0.0f };
		float m_bounceFactor{ 0.5f };
	};

	class AttractorUpdater : public ParticleUpdater
	{
	public:
		virtual void update(double dt, ParticleData* p) override;

		size_t collectionSize() const { return m_attractors.size(); }
		void add(const glm::vec4& attr) { m_attractors.push_back(attr); }
		glm::vec4& get(size_t id) { return m_attractors[id]; }

	protected:
		std::vector<glm::vec4> m_attractors; // .w is force
	};

	class BasicColorUpdater : public ParticleUpdater
	{
	public:
		virtual void update(double dt, ParticleData* p) override;
	};

	class PosColorUpdater : public ParticleUpdater
	{
	public:
		virtual void update(double dt, ParticleData* p) override;

	public:
		glm::vec4 m_minPos{ 0.0 };
		glm::vec4 m_maxPos{ 1.0 };
	};

	class VelColorUpdater : public ParticleUpdater
	{
	public:
		virtual void update(double dt, ParticleData* p) override;

	public:
		glm::vec4 m_minVel{ 0.0 };
		glm::vec4 m_maxVel{ 1.0 };
	};

	class BasicTimeUpdater : public ParticleUpdater
	{
	public:
		virtual void update(double dt, ParticleData* p) override;
	};
}
