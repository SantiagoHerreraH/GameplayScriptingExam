

#include "../ExternalIncludes.h"

#define _USE_MATH_DEFINES 
#include <math.h>

#include "../Math/STrigonometry.h"
#include "../Math/SVectors.h"

namespace Pillar {

	

	float STrigonometry::Area(const FTriangle2f& triangle)
	{
		return
			SVectors::Determinant(
				SVectors::Subtract(
					triangle.Vertices[1], triangle.Vertices[0]),
				SVectors::Subtract(
					triangle.Vertices[2], triangle.Vertices[0])) / 2;
	}

	FVector2f STrigonometry::Center(const FTriangle2f& triangle)
	{
		FVector2f result{};
		for (size_t i = 0; i < 3; i++)
		{
			result.X += triangle.Vertices[i].X;
			result.Y += triangle.Vertices[i].Y;
		}

		result.X /= 3.f;
		result.Y /= 3.f;

		return result;
	}

	double STrigonometry::AbsoluteHypotenuse(const double adjacentSide, const double oppositeSide)
	{
		//a^2 + b^2 = c^2
		return sqrt(pow(adjacentSide, 2) + pow(oppositeSide, 2));
	}

	double STrigonometry::AbsoluteAdjacentSide(const double hypotenuse, const double oppositeSide)
	{
		//b^2 = -a^2 + c^2
		return sqrt(pow(hypotenuse, 2) - pow(oppositeSide, 2));
	}

	double STrigonometry::AbsoluteOppositeSide(const double hypotenuse, const double adjacentSide)
	{
		//a^2 = -b^2 + c^2
		return sqrt(pow(hypotenuse, 2) - pow(adjacentSide, 2));
	}

}