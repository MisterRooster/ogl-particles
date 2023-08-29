/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <memory>

namespace nhahn
{
	class IEffect
	{
	public:
		IEffect() { }
		virtual ~IEffect() { }

		/* creates the effect with desired num of particles, (0 means default for the effect) */
		virtual bool initialize(size_t numParticles) = 0;
		virtual bool initializeRenderer(const char* name) = 0;
		virtual void reset() = 0;
		virtual void clean() = 0;

		virtual void update(double dt) = 0;
		virtual void cpuUpdate(double dt) = 0;
		virtual void gpuUpdate(double dt) = 0;
		virtual void render() = 0;
		virtual void renderUI() = 0;

		virtual int numAllParticles() = 0;
		virtual int numAliveParticles() = 0;
		virtual double aliveToAllRatio() = 0;

	public:
		static const size_t DEFAULT_PARTICLE_NUM_FLAG = 0;		// for initialize method
		static const size_t DEFAULT_PARTICLE_COUNT = 500000;	// 500k particles
		//enum Name { };
	};

	class EffectFactory
	{
	public:
		static std::shared_ptr<IEffect> create(const char* name);
	};
}
