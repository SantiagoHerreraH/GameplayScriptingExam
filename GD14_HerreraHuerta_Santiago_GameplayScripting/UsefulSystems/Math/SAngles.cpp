
//#include "../../pch.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include "../Math/SAngles.h"
#include "../Math/STools.h"

namespace Pillar {

	const double SAngles::ClampToDegrees(const double value) {
		return STools::Clamp(value, -360, 360);
	}
	
	const double SAngles::ClampToRadians(const double value)
	{
		return STools::Clamp(value, -(2. * M_PI), 2. * M_PI);
	}
	
	const double SAngles::CalculateDegreeBetweenVectorComponents(const FVector2i& from, const FVector2i& to)
	{
		return InDegrees(atan2(to.Y - from.Y, to.X - to.X));
	}
	
	const double SAngles::CalculateRadiansBetweenVectorComponents(const FVector2i& from, const FVector2i& to)
	{
		return atan2(to.Y - from.Y, to.X - to.X);
	}
	
	const double SAngles::InDegrees(const double radians) {
	
		return (360. * ClampToRadians(radians)) / (2. * M_PI);
	}
	
	const double SAngles::InRadians(const double degrees) {
	
		return (2. * M_PI * ClampToDegrees(degrees)) / 360.;
	
	}

}
