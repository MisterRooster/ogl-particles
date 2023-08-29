/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "Effect.h"

#include <string>
#include "TunnelEffect.h"
#include "FountainEffect.h"
#include "AttractorEffect.h"


namespace nhahn
{
	std::shared_ptr<IEffect> EffectFactory::create(const char* name)
	{
		std::string effect{ name };

		if (effect == "tunnel")
			return std::make_shared<TunnelEffect>();
		else if (effect == "attractors")
			return std::make_shared<AttractorEffect>();
		else if (effect == "fountain")
			return std::make_shared<FountainEffect>();

		return nullptr;
	}
}