#include "IDrawableDeleter.h"

namespace touchpoints { namespace drawing
{
	IDrawableDeleter::IDrawableDeleter() {}

	void IDrawableDeleter::operator()(IDrawable*ptr) const { delete ptr; }
}}