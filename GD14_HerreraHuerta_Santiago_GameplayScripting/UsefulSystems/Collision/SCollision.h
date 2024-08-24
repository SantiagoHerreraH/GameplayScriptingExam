#pragma once

#include "../Structs/FCollision.h"

namespace Pillar {

	/*
	
		Remember the loop
		- Physics
			- Apply Force To Velocity
			- Apply Stickiness
			- Apply Rect Physics Collision to velocity
			- Apply velocity to pos (can mute velocity by rect boundaries if necessary)
			- Reset Net Force

		- Scene 
			- Position to Rect

		- Collision
			- if collide -> Change Position

		- Scene (Again)
			- Position to Rect (again)

	*/

	//WROTE THESE FUNCTIONS BEFORE I TEMPLATED VECTOR AMD SOME SHAPES

	class SCollision {

	public:
		static bool IsPointInCircle(const FVector2f& point, const FCirclef& circle);
		static bool IsPointInCircle(const FVector2i& point, const FCirclei& circle);
		static bool IsPointInCircle(const FVector2d& point, const FCircled& circle);

		static bool IsPointInRect(const FVector2f& point, const FRectf& rect, bool isYInverted = true);
		static bool IsPointInRect(const FVector2f& point, const FRectd& rect, bool isYInverted = true);
		static bool IsPointInRect(const FVector2f& point, const FRecti& rect, bool isYInverted = true);

		static bool IsPointInLine(const FVector2f& point, const FLine2f& line, float permittedDistanceFromLine);

		//All Above are checked

		static bool IsPointInTriangle(const FVector2f& point, const FTriangle2f& triangle); 
	
		static bool IsOverlapping(const FLine2f& line1, const FLine2f& line2, FLineOverlapInfo& outOverlapInfo);
		
		static bool IsOverlapping(const FCirclef& circle1, const FCirclef& circle2);
		static bool IsOverlapping(const FCirclei& circle1, const FCirclei& circle2);
		static bool IsOverlapping(const FCircled& circle1, const FCircled& circle2);

		static bool IsOverlapping(
			const FCirclef& circle1, const FCirclef& circle2, 
			FOverlapInfo& outOverlapInfoRect1, FOverlapInfo& outOverlapInfoRect2);

		static bool IsOverlapping(const FRecti& rect1, const FRecti& rect2, bool isYInverted = true);
		static bool IsOverlapping(const FRectf& rect1, const FRectf& rect2, bool isYInverted = true);

		static bool IsOverlapping(const FRectf& rect1, const FRectf& rect2, FOverlapInfo& outOverlapInfoRect1, FOverlapInfo& outOverlapInfoRect2, bool isYInverted = true);

		static void SolveCollision(FRectf& rect, const FOverlapInfo& overlapInfo, float deltaSeconds, float translationLengthToSmoothTranslation = 3.f);
		static void SolveCollision(FVector2f& position, const FOverlapInfo& overlapInfo, float deltaSeconds, float translationLengthToSmoothTranslation = 3.f);

		static void SolveRectPhysicsCollision(FVector2f& velocity, const FOverlapInfo& overlapInfo);

		static void SolveIfIsOverlapping(
			FRectf& rect1, FRectf& rect2,
			FOverlapInfo& outOverlapInfo1, FOverlapInfo& outOverlapInfo2,
			float deltaSeconds, float translationLengthToSmoothTranslation = 3.f);

		static void SolveIfIsOverlapping(
			FRectf& rect1, FRectf& rect2,
			FVector2f& position1, FVector2f& position2,
			FVector2f& velocity1, FVector2f& velocity2,
			FOverlapInfo& outOverlapInfo1, FOverlapInfo& outOverlapInfo2,
			float deltaSeconds, float translationLengthToSmoothTranslation = 3.f);

		static void ApplySticky(float& velocityAxisToAffect, float stickyVelocityAxis);
	};
}

