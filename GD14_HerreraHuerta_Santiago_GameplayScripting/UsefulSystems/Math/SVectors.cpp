

#include "../ExternalIncludes.h"

#define _USE_MATH_DEFINES 
#include <math.h>

#include "../Math/SVectors.h"
#include "../Math/STrigonometry.h"
#include "../Math/SAngles.h"
#include "../Math/SMatrix.h"
#include "../Math/STools.h"

namespace Pillar {

	/*

		VECTORS

	*/

	const FVector2d SVectors::CastToDoublePoint(const FVector2i& point)
	{
		return { static_cast<double>(point.X), static_cast<double>(point.Y) };
	}

	const FVector2d SVectors::CastToDoublePoint(const FVector2f& point)
	{
		return { static_cast<double>(point.X), static_cast<double>(point.Y) };
	}

	const FVector2i SVectors::CastToIntPoint(const FVector2d& point)
	{
		return { static_cast<int>(point.X), static_cast<int>(point.Y) };
	}

	const FVector2i SVectors::CastToIntPoint(const FVector2f& point)
	{
		return { static_cast<int>(point.X), static_cast<int>(point.Y) };
	}

	const FVector2f SVectors::CastToFloatPoint(const FVector2d& point)
	{
		return { static_cast<float>(point.X), static_cast<float>(point.Y) };
	}

	const FVector2f SVectors::CastToFloatPoint(const FVector2i& point)
	{
		return { static_cast<float>(point.X), static_cast<float>(point.Y) };
	}

	const FVector2f SVectors::Add(const FVector2f& first, const FVector2f& second)
	{
		return { first.X + second.X, first.Y + second.Y };
	}

	const FVector2d SVectors::Add(const FVector2d& first, const FVector2d& second)
	{
		return { first.X + second.X, first.Y + second.Y };
	}

	const FVector2i SVectors::Add(const FVector2i& first, const FVector2i& second)
	{
		return { first.X + second.X, first.Y + second.Y };
	}

	const FVector2d SVectors::Subtract(const FVector2d& end, const FVector2d& start)
	{
		return { end.X - start.X, end.Y - start.Y };
	}

	const FVector2f SVectors::Subtract(const FVector2f& end, const FVector2f& start)
	{
		return { end.X - start.X, end.Y - start.Y };
	}

	const FVector2i SVectors::Subtract(const FVector2i& end, const FVector2i& start)
	{
		return { end.X - start.X, end.Y - start.Y };
	}

	const FVector2f SVectors::ObtainRelativeVectorComponent(const FVector& direction, const FVector2f& centerPoint)
	{
		double adjacentOverHypotenuse = cos((SAngles::InRadians(direction.AngleInDegrees)));
		double oppositeOverHypotenuse = sin((SAngles::InRadians(direction.AngleInDegrees)));

		double adjacent = adjacentOverHypotenuse * direction.Length;
		double opposite = oppositeOverHypotenuse * direction.Length;

		return
		{
		float(centerPoint.X + adjacent),
		float(centerPoint.Y + opposite) };
	}

	const FVector2f SVectors::OrthogonalVector(const FVector2f& upVector)
	{
		return FVector2f{upVector.Y, -(upVector.X)};
	}

	const FVector2d SVectors::Scale(const FVector2d& point, const double scale)
	{
		return FVector2d{ point.X * scale, point.Y * scale };
	}

	const FVector2f SVectors::Scale(const FVector2f& point, const float scale)
	{
		return FVector2f{ point.X * scale, point.Y * scale };
	}

	const FVector2i SVectors::Scale(const FVector2i& point, const float scale)
	{
		return FVector2i{ int(point.X * scale), int(point.Y * scale) };
	}

	const bool SVectors::HaveSameQuadrant(const FVector2f& one, const FVector2f& two)
	{
		return 
			((one.X >= 0 && two.X >= 0) || (one.X <= 0 && two.X <= 0)) &&
			((one.Y >= 0 && two.Y >= 0) || (one.Y <= 0 && two.Y <= 0));
	}

	const FVector2d SVectors::GetUnitRotationVector(const FVector2d& centerReference, const FVector2d& point)
	{
		double xDelta = point.X - centerReference.X;
		double yDelta = point.Y - centerReference.Y;

		double hypotenuse = STrigonometry::AbsoluteHypotenuse(xDelta, yDelta);

		return FVector2d{ xDelta / hypotenuse, yDelta / hypotenuse };
	}

	const float SVectors::Distance(const FVector2i& pointOne, const FVector2i& pointTwo)
	{
		return STrigonometry::AbsoluteHypotenuse(pointTwo.X - pointOne.X, pointTwo.Y - pointOne.Y);;
	}

	const float SVectors::Distance(const FVector2f& pointOne, const FVector2f& pointTwo)
	{
		return STrigonometry::AbsoluteHypotenuse(pointTwo.X - pointOne.X, pointTwo.Y - pointOne.Y);;
	}

	const double SVectors::Distance(const FVector2d& pointOne, const FVector2d& pointTwo)
	{
		return STrigonometry::AbsoluteHypotenuse(pointTwo.X - pointOne.X, pointTwo.Y - pointOne.Y);
	}

	const double SVectors::Length(const FVector2d& vector)
	{
		return STrigonometry::AbsoluteHypotenuse(vector.X, vector.Y);
	}

	const float SVectors::Length(const FVector2i& vector)
	{
		return STrigonometry::AbsoluteHypotenuse((float)vector.X, (float)vector.Y);
	}

	const float SVectors::Length(const FVector2f& vector)
	{
		return STrigonometry::AbsoluteHypotenuse(vector.X, vector.Y);
	}

	const double SVectors::ProjectedPercentage(const FVector2d& vectorReference, const FVector2d& vectorToProject)
	{
		double multipliedLengths = SVectors::Length(vectorReference) * SVectors::Length(vectorToProject);

		if (multipliedLengths == 0.0)
		{
			//debug comment here
			return 0.0;
		}

		return SVectors::DotProduct(vectorReference, vectorToProject) / multipliedLengths;
	}

	//You obtain the vectortoBeProjected Length
	const double SVectors::ProjectedLength(const FVector2d& vectorReference, const FVector2d& vectorToProject)
	{
		double length = SVectors::Length(vectorReference);

		if (length == 0.0)
		{
			//debug comment here
			return 0.0;
		}

		return SVectors::DotProduct(vectorReference, vectorToProject) / length;
	}

	const float SVectors::ProjectedLength(const FVector2f& vectorReference, const FVector2f& vectorToProject)
	{
		float length = SVectors::Length(vectorReference);

		if (length == 0.0)
		{
			//debug comment here
			return 0.0;
		}

		return SVectors::DotProduct(vectorReference, vectorToProject) / length;
	}

	const float SVectors::ProjectedLength(const FVector2i& vectorReference, const FVector2i& vectorToProject)
	{
		double length = SVectors::Length(vectorReference);

		if (length == 0.0)
		{
			//debug comment here
			return 0.0;
		}

		return SVectors::DotProduct(vectorReference, vectorToProject) / length;
	}

	const FVector2d SVectors::ProjectedVector(const FVector2d& vectorReference, const FVector2d& vectorToProject)
	{
		double length{ Length(vectorReference) };
		length = length == 0 ? 0.00001 : length;
		return Scale(vectorReference, ProjectedLength(vectorReference, vectorToProject) / length);
	}

	const FVector2f SVectors::ProjectedVector(const FVector2f& vectorReference, const FVector2f& vectorToProject)
	{
		double length{ Length(vectorReference) };
		length = length == 0 ? 0.00001 : length;
		return Scale(vectorReference, ProjectedLength(vectorReference, vectorToProject) / length);
	}

	const FVector2i SVectors::ProjectedVector(const FVector2i& vectorReference, const FVector2i& vectorToProject)
	{
		double length{ Length(vectorReference) };
		length = length == 0 ? 0.00001 : length;
		return Scale(vectorReference, ProjectedLength(vectorReference, vectorToProject) / length);
	}

	const double SVectors::DotProduct(const FVector2d& one, const FVector2d& two)
	{
		return one.X * two.X + one.Y * two.Y;
	}

	const float SVectors::DotProduct(const FVector2f& one, const FVector2f& two)
	{
		return DotProduct(CastToDoublePoint(one), CastToDoublePoint(two));
	}

	const float SVectors::DotProduct(const FVector2i& one, const FVector2i& two)
	{
		return DotProduct(CastToDoublePoint(one), CastToDoublePoint(two));
	}

	const FVector SVectors::Direction(const FVector2d& from, const FVector2d& to)
	{

		FVector direction{};

		double xDelta = to.X - from.X;
		double yDelta = to.Y - from.Y;

		double radians = atan2(yDelta, xDelta);
		double degrees = SAngles::InDegrees(radians);

		direction.Length = STrigonometry::AbsoluteHypotenuse(xDelta, yDelta);
		direction.AngleInDegrees = degrees;

		return direction;
	}

	const FVector SVectors::Direction(const FVector2f& vector)
	{
		FVector direction{};
		double radians = atan2(vector.Y, vector.X);
		double degrees = SAngles::InDegrees(radians);

		direction.Length = STrigonometry::AbsoluteHypotenuse(vector.X, vector.Y);
		direction.AngleInDegrees = degrees;
		return direction;
	}

	const FVector2f SVectors::RotateVector(const FVector2f& vectorToRotate, double rotation)
	{
		DMatrix2x2f matrix{};
		SMatrix::MakeRotationMatrix(matrix, rotation);
		FVector2f result{};
		SMatrix::Multiply(matrix, vectorToRotate, result);
		return result;
	}

	const FVector2f SVectors::NormalizeVector(const FVector2f& vectorToNormalize)
	{
		float vectorLength = Length(vectorToNormalize);
		FVector2f normalizedVector{ vectorToNormalize };
		if (vectorLength != 0.f)
		{
			normalizedVector.X /= vectorLength;
			normalizedVector.Y /= vectorLength;
			return normalizedVector;
		}
		
		return FVector2f{ 0,0 };
	}

	const double SVectors::Determinant(const FVector2d& sideVector, const FVector2d& upVector)
	{
		/*
		Order Matters
		*/
		return (sideVector.X * upVector.Y) - (sideVector.Y * upVector.X);
	}

	const float SVectors::Determinant(const FVector2f& sideVector, const FVector2f& upVector)
	{
		/*
		Order Matters
		*/
		return (sideVector.X * upVector.Y) - (sideVector.Y * upVector.X);
	}

	const double SVectors::Determinant(const double sideVectorX, const double sideVectorY, const double upVectorX, const double upVectorY)
	{
		/*
		Order Matters
		*/
		return (sideVectorX * upVectorY) - (sideVectorY * upVectorX);
	}


}