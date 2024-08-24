#pragma once

#include <vector>

namespace Pillar {

	/*

		2D

	*/

	template<typename Type>
	struct FVector2 {
		Type X{};
		Type Y{};
	};

	using FVector2d = FVector2<double>;
	using FVector2f = FVector2<float>;
	using FVector2i = FVector2<int>;


	struct FVector {

		double AngleInDegrees{};
		double Length{};
	};

	struct FPoint2AndLength {

		FVector2f Point{};
		double Length{};
	};

	struct FRotation3 {
		double X{};
		double Y{};
		double Z{};
	};

	struct FRotation3f {
		float X{};
		float Y{};
		float Z{};
	};

}