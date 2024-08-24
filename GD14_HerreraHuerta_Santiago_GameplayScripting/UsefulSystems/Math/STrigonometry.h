#pragma once 

#include "../Structs/FShapes.h"

namespace Pillar {

	class STrigonometry {

	public:
		static float Area(const FTriangle2f& triangle); 
		static FVector2f Center(const FTriangle2f& triangle);
		static double AbsoluteHypotenuse(const double adjacentSide, const double oppositeSide);
		static double AbsoluteAdjacentSide(const double hypotenuse, const double oppositeSide);
		static double AbsoluteOppositeSide(const double hypotenuse, const double adjacentSide);
	};

}