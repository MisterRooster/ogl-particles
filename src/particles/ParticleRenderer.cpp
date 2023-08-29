/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "ParticleRenderer.h"

#include "GLParticleRenderer.h"


namespace nhahn
{
	template <typename T> std::shared_ptr<IParticleRenderer> createRenderer() { return std::make_shared<T>(); }
	std::unique_ptr<ParticleRendererFactory::MapOfRendererGenerators> ParticleRendererFactory::s_generatorMap;

	void ParticleRendererFactory::initGeneratorMap()
	{
		if (s_generatorMap)
			return;

		s_generatorMap = std::make_unique<std::map<std::string, std::shared_ptr<IParticleRenderer>(*)()>>();
		(*s_generatorMap)["gl"] = &createRenderer<GLParticleRenderer>;
		(*s_generatorMap)["gl_map"] = &createRenderer<GLParticleRendererUseMap>;
		(*s_generatorMap)["gl_double"] = &createRenderer<GLParticleRendererDoubleVao>;
		(*s_generatorMap)["gl_persistent"] = &createRenderer<GLParticleRendererPersistent>;
	}

	std::shared_ptr<IParticleRenderer> ParticleRendererFactory::create(const char* name)
	{
		if (!s_generatorMap)
			initGeneratorMap();

		if (isAvailable(name))
			return (*s_generatorMap)[name]();

		return nullptr;
	}

	bool ParticleRendererFactory::isAvailable(const char* name)
	{
		if (!s_generatorMap)
			initGeneratorMap();

		return (s_generatorMap->find(name) != s_generatorMap->end());
	}
}