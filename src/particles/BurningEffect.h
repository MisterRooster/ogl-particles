/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <memory>
#include "Effect.h"
#include "ParticleSystem.h"
#include "ParticleGenerators.h"
#include "ParticleRenderer.h"
#include "ParticleUpdaters.h"


namespace nhahn
{
	class BurningEffect : public IEffect
	{
	public:
		BurningEffect() { }
		~BurningEffect() { }

		bool initialize(size_t numParticles) override;
		bool initializeRenderer(const char* name) override;
		void reset() override { m_system->reset(); }
		void clean() override;

		void update(double dt) override;
		void cpuUpdate(double dt) override;
		void gpuUpdate(double dt) override;
		void render() override;
		void renderUI() override;

		int numAllParticles() override { return m_system->numAllParticles(); }
		int numAliveParticles() override { return m_system->numAliveParticles(); }
		double aliveToAllRatio() override { return m_system->getAliveToAllRatio(); }

	private:
		std::shared_ptr<ParticleSystem> m_system;
		std::shared_ptr<IParticleRenderer> m_renderer;
		std::shared_ptr<SpherePosGen> m_posGenerator;
		std::shared_ptr<BasicColorGen> m_colGenerator;
		std::shared_ptr<EulerUpdater> m_eulerUpdater;
	};
}