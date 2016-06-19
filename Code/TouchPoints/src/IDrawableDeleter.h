#pragma once
#include "IDrawable.h"

namespace touchpoints { namespace drawing
{
	struct IDrawableDeleter
	{
		IDrawableDeleter();
		void operator()(touchpoints::drawing::IDrawable*) const;
	};
}}