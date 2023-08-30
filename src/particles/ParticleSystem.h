/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <vector>
#include "ParticleData.h"


namespace nhahn
{
#	define M_PI 3.1415936f


	class ParticleGenerator
	{
	public:
		ParticleGenerator() { }
		virtual ~ParticleGenerator() { }

		virtual void generate(double dt, ParticleData* p, size_t startId, size_t endId) = 0;
	};

	class ParticleEmitter
	{
	public:
		ParticleEmitter() { }
		virtual ~ParticleEmitter() { }

		/* calls all the generators and activates(wakes) particle */ 
		virtual void emit(double dt, ParticleData* p);

		void addGenerator(std::shared_ptr<ParticleGenerator> gen) { m_generators.push_back(gen); }

	public:
		float m_emitRate{ 0.0 };

	protected:
		std::vector<std::shared_ptr<ParticleGenerator>> m_generators;
	};

	class ParticleUpdater
	{
	public:
		ParticleUpdater() { }
		virtual ~ParticleUpdater() { }

		virtual void update(double dt, ParticleData* p) = 0;
	};

	class ParticleSystem
	{
	public:
		explicit ParticleSystem(size_t maxCount);
		virtual ~ParticleSystem() { }

		ParticleSystem(const ParticleSystem&) = delete;
		ParticleSystem& operator=(const ParticleSystem&) = delete;

		virtual void update(double dt);
		virtual void reset();

		virtual size_t numAllParticles() const { return m_particles.m_count; }
		virtual size_t numAliveParticles() const { return m_particles.m_countAlive; }

		void addEmitter(std::shared_ptr<ParticleEmitter> em) { m_emitters.push_back(em); }
		void addUpdater(std::shared_ptr<ParticleUpdater> up) { m_updaters.push_back(up); }

		ParticleData* finalData() { return &m_particles; }

		double getAliveToAllRatio() const { return m_aliveToAllRatio; }

		size_t computeMemoryUsage();

	protected:
		ParticleData m_particles;
		ParticleData m_aliveParticles;

		size_t m_count{ 0 };

		std::vector<std::shared_ptr<ParticleEmitter>> m_emitters;
		std::vector<std::shared_ptr<ParticleUpdater>> m_updaters;

		double m_aliveToAllRatio{ 0.0 };
	};
}
