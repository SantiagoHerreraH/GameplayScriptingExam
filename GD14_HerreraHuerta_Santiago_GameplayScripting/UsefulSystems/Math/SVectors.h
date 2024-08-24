
#pragma once

#include "../Structs/FVector.h"

namespace Pillar {

	class SVectors {
	public:

		/*

			2D

		*/

		//2D casts
		static const FVector2d CastToDoublePoint(const FVector2i& vector);
		static const FVector2d CastToDoublePoint(const FVector2f& vector);

		static const FVector2i CastToIntPoint(const FVector2d& vector);
		static const FVector2i CastToIntPoint(const FVector2f& vector);

		static const FVector2f CastToFloatPoint(const FVector2d& vector);
		static const FVector2f CastToFloatPoint(const FVector2i& vector);

		//Operations
		static const FVector2f Add(const FVector2f& first, const FVector2f& second);
		static const FVector2d  Add(const FVector2d& first, const FVector2d& second);
		static const FVector2i Add(const FVector2i& first, const FVector2i& second);


		static const FVector2d Subtract(const  FVector2d&  end, const FVector2d&  start);
		static const FVector2f Subtract(const FVector2f& end, const FVector2f& start);
		static const FVector2i Subtract(const FVector2i& end, const FVector2i& start);

		//put this function in SVectors
		static const FVector2f ObtainRelativeVectorComponent(const FVector& direction, const FVector2f& centerPoint);

		static const FVector2f OrthogonalVector(const FVector2f& upVector);

		/*

			3D

		*/

		static const FVector2d  Scale(const FVector2d& vector, const double scale);
		static const FVector2f Scale(const FVector2f& vector, const float scale);
		static const FVector2i Scale(const FVector2i& vector, const float scale);

		static const bool HaveSameQuadrant(const FVector2f& one, const FVector2f& two);

		/*

			CALCULATIONS

		*/

		static const FVector2d GetUnitRotationVector(const FVector2d& centerReference, const FVector2d& point);

		static const float  Distance(const FVector2i& pointOne, const FVector2i& pointTwo);
		static const float  Distance(const FVector2f& pointOne, const FVector2f& pointTwo);
		static const double Distance(const FVector2d& pointOne, const FVector2d& pointTwo);

		static const float Length(const FVector2i& vector);
		static const float Length(const FVector2f& vector);
		static const double Length(const FVector2d& vector);

		static const double ProjectedPercentage(const FVector2d& vectorReference, const FVector2d& vectorToProject);

		static const double ProjectedLength(const FVector2d& vectorReference, const FVector2d& vectorToProject);
		static const float  ProjectedLength(const FVector2f& vectorReference, const FVector2f& vectorToProject);
		static const float  ProjectedLength(const FVector2i& vectorReference, const FVector2i& vectorToProject);

		static const FVector2d  ProjectedVector(const FVector2d& vectorReference, const FVector2d& vectorToProject);
		static const FVector2f ProjectedVector(const FVector2f& vectorReference, const FVector2f& vectorToProject);
		static const FVector2i ProjectedVector(const FVector2i& vectorReference, const FVector2i& vectorToProject);

		static const double DotProduct(const FVector2d& one, const FVector2d& two);
		static const float  DotProduct(const FVector2f& one, const FVector2f& two);
		static const float	DotProduct(const FVector2i& one, const FVector2i& two);

		static const FVector Direction(const FVector2d& from, const FVector2d& to);
		static const FVector Direction(const FVector2f& vector);

		static const FVector2f RotateVector(const FVector2f& vectorToRotate, double rotation);

		static const FVector2f NormalizeVector(const FVector2f& vectorToNormalize);

		static const double Determinant(const FVector2d& sideVector, const FVector2d& upVector);
		static const float  Determinant(const FVector2f& sideVector, const FVector2f& upVector);
		static const double Determinant(const double sideVectorX, const double sideVectorY, const double upVectorX, const double upVectorY);

	};

}