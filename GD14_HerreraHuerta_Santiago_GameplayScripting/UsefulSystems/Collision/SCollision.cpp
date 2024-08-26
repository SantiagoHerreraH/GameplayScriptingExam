
#include "../ExternalIncludes.h"

#include "../Collision/SCollision.h"
#include "../Math/SVectors.h"
#include "../Math/SMatrix.h"
#include "../Math/STrigonometry.h"
#include "../Structs/FMatrix.h"

namespace Pillar {

#pragma region IsPointInCircle

	bool SCollision::IsPointInCircle(const FVector2f& point, const FCirclef& circle)
	{
		
		return 
			IsPointInCircle(
				SVectors::CastToDoublePoint(point),
			 {  SVectors::CastToDoublePoint(circle.Center), (double)circle.Radius });
	}

	bool SCollision::IsPointInCircle(const FVector2i& point, const FCirclei& circle)
	{
		return
			IsPointInCircle(
				SVectors::CastToDoublePoint(point),
				{ SVectors::CastToDoublePoint(circle.Center), (double)circle.Radius });
	}

	bool SCollision::IsPointInCircle(const FVector2d& point, const FCircled& circle)
	{
		const double distance{ SVectors::Distance(point, circle.Center) };
		bool result{ distance <= circle.Radius };

		return result;
	}

#pragma endregion

#pragma region IsPointInRect

	bool SCollision::IsPointInRect(const FVector2f& point, const FRecti& rect, bool isYInverted)
	{
		bool horizontalCheck{};
		bool verticalCheck{};


		horizontalCheck = (point.X >= rect.Left) && (point.X <= (rect.Left + rect.Width));

		if (isYInverted)
		{
			verticalCheck =  (point.Y >= rect.Top) && (point.Y <= (rect.Top + rect.Height));
		}
		else
		{
			verticalCheck = (point.Y <= rect.Top) && (point.Y >= (rect.Top - rect.Height)) ;
		}

		return(horizontalCheck && verticalCheck);
	}

	bool SCollision::IsPointInRect(const FVector2f& point, const FRectf& rect, bool isYInverted)
	{
		bool horizontalCheck{};
		bool verticalCheck{};


		horizontalCheck = (point.X >= rect.Left) && (point.X <= (rect.Left + rect.Width));

		if (isYInverted)
		{
			verticalCheck = (point.Y >= rect.Top) && (point.Y <= (rect.Top + rect.Height));
		}
		else
		{
			verticalCheck = (point.Y <= rect.Top) && (point.Y >= (rect.Top - rect.Height));
		}

		return(horizontalCheck && verticalCheck);
	}

	bool SCollision::IsPointInRect(const FVector2f& point, const FRectd& rect, bool isYInverted)
	{
		bool horizontalCheck{};
		bool verticalCheck{};


		horizontalCheck = (point.X >= rect.Left) && (point.X <= (rect.Left + rect.Width));

		if (isYInverted)
		{
			verticalCheck = (point.Y >= rect.Top) && (point.Y <= (rect.Top + rect.Height));
		}
		else
		{
			verticalCheck = (point.Y <= rect.Top) && (point.Y >= (rect.Top - rect.Height));
		}

		return(horizontalCheck && verticalCheck);
	}

	

#pragma endregion

#pragma region IsPointInLine

	bool SCollision::IsPointInLine(const FVector2f& point, const FLine2f& line, float permittedDistanceFromLine)
	{
		FVector2f startToEnd{ SVectors::Subtract(line.End, line.Start) };
		FVector2f startToPoint{ SVectors::Subtract(point, line.Start) };
		float startToPointLength{ SVectors::Length(startToPoint) };

		FVector2f localProjectedVector = 
			SVectors::ProjectedVector(startToEnd, startToPoint);

		FVector2f actualProjectedVector
			= SVectors::Add(localProjectedVector, line.Start);

		bool isAtAPassablePerpendicularDistance = SVectors::Distance(actualProjectedVector, point) <= permittedDistanceFromLine;
		bool isAtAPassableParallelDistance = 
			(SVectors::Distance(line.Start, line.End) >= startToPointLength &&
			(SVectors::HaveSameQuadrant(startToEnd, startToPoint) || 
				startToPointLength <= permittedDistanceFromLine));

		return isAtAPassableParallelDistance && isAtAPassablePerpendicularDistance;
	}

#pragma endregion

#pragma region IsPointInTriangle

	bool SCollision::IsPointInTriangle(const FVector2f& point, const FTriangle2f& triangle)
	{
		float triangleArea{ std::abs(STrigonometry::Area(triangle))};
		float composedTriangleArea{};

		FTriangle2f currentTriangle{};

		for (size_t i = 0; i < 3; i++)
		{
			currentTriangle.Vertices[0] = point;
			currentTriangle.Vertices[1] = triangle.Vertices[i];
			currentTriangle.Vertices[2] = triangle.Vertices[(i + 1) % 3];

			composedTriangleArea += std::abs(STrigonometry::Area(currentTriangle));

		}
	
		return triangleArea == composedTriangleArea;
	}

#pragma endregion


#pragma region IsOverlappingLine

	// reference video (https://www.youtube.com/watch?v=5FkOO1Wwb8w)

	bool SCollision::IsOverlapping(const FLine2f& line1, const FLine2f& line2, FLineOverlapInfo& outOverlapInfo)
	{
		FVector2f scalarVector{};

		//AC = CD * t1 + AB * t2
		FVector2f AC{ SVectors::Subtract( line2.Start, line1.Start) };
		FVector2f AB{ SVectors::Subtract( line1.End  , line1.Start) };
		FVector2f CD{ SVectors::Subtract( line2.End  , line2.Start) };

		DMatrix2x2f tempMatrix{};
		tempMatrix[0][0] = AB.X;
		tempMatrix[0][1] = CD.X;
		tempMatrix[1][0] = AB.Y;
		tempMatrix[1][1] = CD.Y;

		SMatrix::Inverse(tempMatrix);

		SMatrix::Multiply(tempMatrix, AC, scalarVector);
		
		
		outOverlapInfo.Line1Scalar = scalarVector.X;
		outOverlapInfo.Line2Scalar = -scalarVector.Y;
		outOverlapInfo.IntersectionPoint = SVectors::Add(line1.Start, SVectors::Scale(AB, outOverlapInfo.Line1Scalar));

		if (outOverlapInfo.Line1Scalar < 0 || outOverlapInfo.Line2Scalar < 0 ||
			outOverlapInfo.Line1Scalar > 1 || outOverlapInfo.Line2Scalar > 1)
		{
			outOverlapInfo.isOverlapping = false;
			return false;
		}

		outOverlapInfo.isOverlapping = true;
		return outOverlapInfo.isOverlapping;
	}

#pragma endregion

#pragma region IsOverlappingCircle
	bool SCollision::IsOverlapping(const FCirclef& circle1, const FCirclef& circle2)
	{
		const float distance{ SVectors::Distance(circle1.Center, circle2.Center) };

		return (distance <= (circle1.Radius + circle2.Radius));
	}

	bool SCollision::IsOverlapping(const FCirclei& circle1, const FCirclei& circle2)
	{
		const float distance{ SVectors::Distance(circle1.Center, circle2.Center) };

		return (distance <= (circle1.Radius + circle2.Radius));
	}

	bool SCollision::IsOverlapping(const FCircled& circle1, const FCircled& circle2)
	{
		const double distance{ SVectors::Distance(circle1.Center, circle2.Center) };

		return (distance <= (circle1.Radius + circle2.Radius));
	}

	bool SCollision::IsOverlapping(const FCirclef& circle1, const FCirclef& circle2, FOverlapInfo& outOverlapInfoRect1, FOverlapInfo& outOverlapInfoRect2)
	{
		FVector2f deltaFromOneToTwo{SVectors::Subtract(circle2.Center, circle1.Center)};
		const float distance{ SVectors::Length(deltaFromOneToTwo) };

		outOverlapInfoRect1.isOverlapping = (distance <= (circle1.Radius + circle2.Radius));
		outOverlapInfoRect2.isOverlapping = outOverlapInfoRect1.isOverlapping;

		FVector2f intersectionDeltaFromCenterOne = SVectors::Scale(
			SVectors::NormalizeVector(
				deltaFromOneToTwo),
			circle1.Radius);

		outOverlapInfoRect1.IntersectionPoint = SVectors::Add(intersectionDeltaFromCenterOne, circle1.Center);

		FVector2f intersectionDeltaFromCenterTwo = SVectors::Scale(
			SVectors::NormalizeVector(
				SVectors::Subtract(circle1.Center, circle2.Center)),
			circle2.Radius);

		outOverlapInfoRect2.IntersectionPoint = SVectors::Add(intersectionDeltaFromCenterTwo, circle2.Center);

		//not sure if this is one or two
		outOverlapInfoRect2.TranslationVector = 
			SVectors::Subtract(outOverlapInfoRect1.IntersectionPoint, outOverlapInfoRect2.IntersectionPoint);
		
		outOverlapInfoRect1.TranslationVector =
			SVectors::Subtract(outOverlapInfoRect2.IntersectionPoint, outOverlapInfoRect1.IntersectionPoint);

		return outOverlapInfoRect1.isOverlapping;
	}

	bool SCollision::IsOverlapping(const FCirclef& circle, const FRectf& rect, bool isYInverted)
	{
		// temporary variables to set edges for testing
		float testX = circle.Center.X;
		float testY = circle.Center.Y;

		// which edge is closest?
		if (circle.Center.X < rect.Left)         testX = rect.Left;      // test left edge
		else if (circle.Center.X > rect.Left + rect.Width) testX = rect.Left + rect.Width;   // right edge

		if (isYInverted)
		{
			if (circle.Center.Y < rect.Top)         testY = rect.Top;      // top edge
			else if (circle.Center.Y > rect.Top + rect.Height) testY = rect.Top + rect.Height;   // bottom edge
		}
		else
		{
			if (circle.Center.Y > rect.Top)         testY = rect.Top;      // top edge
			else if (circle.Center.Y < (rect.Top - rect.Height)) testY = rect.Top - rect.Height;   // bottom edge
		}


		// get distance from closest edges
		float distX = circle.Center.X - testX;
		float distY = circle.Center.Y - testY;
		float distance = sqrt((distX * distX) + (distY * distY));

		// if the distance is less than the radius, collision!
		if (distance <= circle.Radius) {
			return true;
		}
		return false;
	}

	bool SCollision::IsOverlapping(const FCirclef& circle, const FRectf& rect, FOverlapInfo& outOverlapInfo1, bool isYInverted)
	{
		// temporary variables to set edges for testing
		float testX = circle.Center.X;
		float testY = circle.Center.Y;

		// which edge is closest?
		if (circle.Center.X < rect.Left)         testX = rect.Left;      // test left edge
		else if (circle.Center.X > rect.Left + rect.Width) testX = rect.Left + rect.Width;   // right edge

		if (isYInverted)
		{
			if (circle.Center.Y < rect.Top)         testY = rect.Top;      // top edge
			else if (circle.Center.Y > (rect.Top + rect.Height)) testY = rect.Top + rect.Height;   // bottom edge
		}
		else
		{
			if (circle.Center.Y > rect.Top)         testY = rect.Top;      // top edge
			else if (circle.Center.Y < (rect.Top - rect.Height)) testY = rect.Top - rect.Height;   // bottom edge
		}


		// get distance from closest edges
		float distX = circle.Center.X - testX;
		float distY = circle.Center.Y - testY;
		float distance = sqrt((distX * distX) + (distY * distY));

		//NOT IMPORTANT:
		//outOverlapInfo1.isOverlapping = true;
		//outOverlapInfo1.IntersectionPoint;
		//outOverlapInfo1.OtherIsOptimized;
		 
		
		// if the distance is less than the radius, collision!
		if (distance <= circle.Radius) {

			float relativeRadiusX{ distX > 0 ? circle.Radius : -circle.Radius };
			float relativeRadiusY{ distY > 0 ? circle.Radius : -circle.Radius };

			float deltaX{ relativeRadiusX - distX };
			float deltaY{ relativeRadiusY - distY };

			if (abs(deltaX) < abs(deltaY))
			{
				outOverlapInfo1.TranslationVector.X = deltaX;
				outOverlapInfo1.TranslationVector.Y = 0;


				outOverlapInfo1.DirectionOfCollided.X = deltaX > 0 ? -1 : 1;
				outOverlapInfo1.DirectionOfCollided.Y = 0;
			}
			else
			{
				outOverlapInfo1.TranslationVector.Y = deltaY;
				outOverlapInfo1.TranslationVector.X = 0;


				if (isYInverted)
				{
					outOverlapInfo1.DirectionOfCollided.Y = deltaY > 0 ? 1 : -1;
					outOverlapInfo1.DirectionOfCollided.X = 0;
				}
				else
				{

					outOverlapInfo1.DirectionOfCollided.Y = deltaY > 0 ? -1 : 1;
					outOverlapInfo1.DirectionOfCollided.X = 0;
				}
			}

			
			

			return true;
		}
		return false;
	}

#pragma endregion

#pragma region IsOverlappingRect

	
	//I think I have to half the output
	bool SCollision::IsOverlapping(
		const FRectf& rect1,
		const FRectf& rect2,
		FOverlapInfo& outOverlapInfo1,
		FOverlapInfo& outOverlapInfo2,
		bool isYInverted)
	{
		outOverlapInfo1.isOverlapping = false;
		outOverlapInfo1.IntersectionPoint = FVector2f{ 0,0 };
		outOverlapInfo1.TranslationVector = FVector2f{ 0,0 };
		outOverlapInfo1.DirectionOfCollided = FVector2i{ 0,0 };

		outOverlapInfo2.isOverlapping = false;
		outOverlapInfo2.IntersectionPoint = FVector2f{ 0,0 };
		outOverlapInfo2.TranslationVector = FVector2f{ 0,0 };
		outOverlapInfo2.DirectionOfCollided = FVector2i{ 0,0 };

		if (!IsOverlapping(rect1, rect2, isYInverted))
		{
			return false;
		}

		float bottom1{ rect1.Top - rect1.Height };
		float bottom2{ rect2.Top - rect2.Height };

		float right1{ rect1.Left + rect1.Width };
		float right2{ rect2.Left + rect2.Width };

		float deltaX_One{ abs(rect1.Left - right2) };
		float deltaX_Two{ abs(rect2.Left - right1) };

		float deltaY_One{ abs(rect1.Top - bottom2) };
		float deltaY_Two{ abs(rect2.Top - bottom1) };

		FVector2f absoluteDelta{};
		absoluteDelta.X = deltaX_One < deltaX_Two ? deltaX_One : deltaX_Two;
		absoluteDelta.Y = deltaY_One < deltaY_Two ? deltaY_One : deltaY_Two;
		

		if (absoluteDelta.X < absoluteDelta.Y)
		{
			if (rect1.Left < rect2.Left)
			{
				outOverlapInfo1.TranslationVector.X = -absoluteDelta.X;
				outOverlapInfo2.TranslationVector.X = absoluteDelta.X;

				outOverlapInfo1.DirectionOfCollided = FVector2i{ 1,0 };
				outOverlapInfo2.DirectionOfCollided = FVector2i{ -1,0 };
			}
			else
			{
				outOverlapInfo1.TranslationVector.X = absoluteDelta.X;
				outOverlapInfo2.TranslationVector.X = -absoluteDelta.X;

				outOverlapInfo1.DirectionOfCollided = FVector2i{ -1,0 };
				outOverlapInfo2.DirectionOfCollided = FVector2i{ 1,0 };
			}
		}
		else
		{
			if (rect1.Top < rect2.Top)
			{
				outOverlapInfo1.TranslationVector.Y = -absoluteDelta.Y;
				outOverlapInfo2.TranslationVector.Y = absoluteDelta.Y;

				outOverlapInfo1.DirectionOfCollided = FVector2i{ 0, 1 };
				outOverlapInfo2.DirectionOfCollided = FVector2i{ 0,-1 };
			}
			else
			{
				outOverlapInfo1.TranslationVector.Y = absoluteDelta.Y;
				outOverlapInfo2.TranslationVector.Y = -absoluteDelta.Y;

				outOverlapInfo1.DirectionOfCollided = FVector2i{ 0,-1 };
				outOverlapInfo2.DirectionOfCollided = FVector2i{ 0,1 };
			}
		}

		float rightMostLeft{ rect1.Left < rect2.Left ? rect2.Left : rect1.Left };
		float leftMostRight{ right1 < right2 ? right1 : right2 };
		float lowestTop{ rect1.Top < rect2.Top ? rect1.Top : rect2.Top };

		float xIntersection{ (rightMostLeft + leftMostRight) / 2.f };

		outOverlapInfo1.isOverlapping = true;
		outOverlapInfo1.IntersectionPoint = FVector2f{ xIntersection,lowestTop };

		outOverlapInfo2.isOverlapping = true;
		outOverlapInfo2.IntersectionPoint = FVector2f{ xIntersection,lowestTop };

		return true;
	}

	void SCollision::SolveCollision(FRectf& rect, const FOverlapInfo& overlapInfo, float deltaSeconds, float translationLengthToSmoothTranslation)
	{
		FVector2f finalTranslationAmount =
			SVectors::Length(overlapInfo.TranslationVector) < translationLengthToSmoothTranslation ?
			SVectors::Scale(overlapInfo.TranslationVector, deltaSeconds) :
			overlapInfo.TranslationVector;

		rect.Left += finalTranslationAmount.X;
		rect.Top += finalTranslationAmount.Y;
	}

	void SCollision::SolveCollision(FVector2f& position, const FOverlapInfo& overlapInfo, float deltaSeconds, float translationLengthToSmoothTranslation)
	{
		FVector2f finalTranslationAmount =
			SVectors::Length(overlapInfo.TranslationVector) < translationLengthToSmoothTranslation ?
			SVectors::Scale(overlapInfo.TranslationVector, deltaSeconds) :
			overlapInfo.TranslationVector;

		position.X += finalTranslationAmount.X;
		position.Y += finalTranslationAmount.Y;
	}

	void SCollision::SolveRectPhysicsCollision(FVector2f& velocity, const FOverlapInfo& overlapInfo)
	{
		if (overlapInfo.DirectionOfCollided.X == 1 &&
			velocity.X > 0)
		{
			velocity.X = 0;
		}
		else if (
			overlapInfo.DirectionOfCollided.X == -1 &&
			velocity.X < 0)
		{
			velocity.X = 0;
		}
		if (
			overlapInfo.DirectionOfCollided.Y == 1 &&
			velocity.Y > 0)
		{
			velocity.Y = 0;
		}
		else if (overlapInfo.DirectionOfCollided.Y == -1 &&
			velocity.Y < 0)
		{
			velocity.Y = 0;
		}
	}

	void SCollision::SolveIfIsOverlapping(
		FRectf& rect1, FRectf& rect2, 
		FOverlapInfo& outOverlapInfo1, FOverlapInfo& outOverlapInfo2,
		float deltaSeconds, bool isYInverted, float translationLengthToSmoothTranslation)
	{
		if (IsOverlapping(rect1, rect2, outOverlapInfo1, outOverlapInfo2, isYInverted))
		{
			SolveCollision(rect1, outOverlapInfo1, deltaSeconds, translationLengthToSmoothTranslation);
			SolveCollision(rect2, outOverlapInfo2, deltaSeconds, translationLengthToSmoothTranslation);
		}
	}

	void SCollision::SolveIfIsOverlapping(
		FRectf& rect1, FRectf& rect2, 
		FVector2f& position1, FVector2f& position2,
		FVector2f& velocity1, FVector2f& velocity2,
		FOverlapInfo& outOverlapInfo1, FOverlapInfo& outOverlapInfo2, 
		float deltaSeconds, bool isYInverted, float translationLengthToSmoothTranslation)
	{
		if (IsOverlapping(rect1, rect2, outOverlapInfo1, outOverlapInfo2, isYInverted))
		{
			SolveCollision(position1, outOverlapInfo1, deltaSeconds, translationLengthToSmoothTranslation);
			SolveCollision(position2, outOverlapInfo2, deltaSeconds, translationLengthToSmoothTranslation);
			SolveRectPhysicsCollision(velocity1, outOverlapInfo1);
			SolveRectPhysicsCollision(velocity2, outOverlapInfo2);

			rect1.Left	= position1.X;
			rect1.Top	= position1.Y;
			
			rect2.Left	= position2.X;
			rect2.Top	= position2.Y;
		}
	}

	void SCollision::ApplySticky(float& velocityAxisToAffect, float stickyVelocityAxis)
	{
		if (stickyVelocityAxis >= 0 && velocityAxisToAffect >= 0)
		{
			velocityAxisToAffect =
				velocityAxisToAffect < stickyVelocityAxis ?
				stickyVelocityAxis :
				velocityAxisToAffect;
		}
		else if (stickyVelocityAxis < 0 && velocityAxisToAffect < 0)
		{
			velocityAxisToAffect =
				velocityAxisToAffect > stickyVelocityAxis ?
				stickyVelocityAxis :
				velocityAxisToAffect;
		}
		else
		{
			velocityAxisToAffect += stickyVelocityAxis;
		}
	}

	bool SCollision::IsOverlapping(const FRecti& rect1, const FRecti& rect2,bool isYInverted)
	{
		if (rect2.Left > (rect1.Left + rect1.Width)) return false;
		if (rect1.Left > (rect2.Left + rect2.Width)) return false;

		if (isYInverted)
		{
			if (rect2.Top > (rect1.Top + rect1.Height)) return false;
			if (rect1.Top > (rect2.Top + rect2.Height)) return false;
		}
		else
		{
			if (rect2.Top < (rect1.Top - rect1.Height)) return false;
			if (rect1.Top < (rect2.Top - rect2.Height)) return false;
		}
		return true;
	}

	bool SCollision::IsOverlapping(const FRectf& rect1, const FRectf& rect2, bool isYInverted)
	{
		if (rect2.Left > (rect1.Left + rect1.Width)) return false;
		if (rect1.Left > (rect2.Left + rect2.Width)) return false;

		if (isYInverted)
		{
			if (rect2.Top > (rect1.Top + rect1.Height)) return false;
			if (rect1.Top > (rect2.Top + rect2.Height)) return false;
		}
		else
		{
			if (rect2.Top < (rect1.Top - rect1.Height)) return false;
			if (rect1.Top < (rect2.Top - rect2.Height)) return false;
		}

		return true;
	}

#pragma endregion
}



