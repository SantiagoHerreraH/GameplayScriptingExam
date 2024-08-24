#pragma once

#include "../Structs/FVector.h"

namespace Pillar {

	class SAngles {
	
	public:
	
		static const double CalculateDegreeBetweenVectorComponents(const FVector2i& from, const FVector2i& to);
		static const double CalculateRadiansBetweenVectorComponents(const FVector2i& from, const FVector2i& to);
	
		static const double InDegrees(const double radians);
		static const double InRadians(const double degrees);
	
		static const double ClampToDegrees(const double value);
		static const double ClampToRadians(const double value);
	
	};
}