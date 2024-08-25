#pragma once

#include <unordered_set>

#include "../Structs/FShapes.h"

namespace Pillar {

	//FParams are parameters you set yourself

	enum class ECollisionBodyType {
		Solid,
		OverlapAll,
		OverlapOnlyMovable,
		IgnoreOnlyMovable,
		IgnoreAll
	};

	struct FRectCollider {
		FRectCollider() {}
		explicit FRectCollider(float left, float top, float width, float height) :
			WorldRect(left, top, width, height),
			ScreenRect(left, top, width, height) {}
		FRectf WorldRect{};
		FRectf ScreenRect{};
	};

	struct FLineOverlapInfo {

		bool isOverlapping{ false };
		float Line1Scalar;
		float Line2Scalar;
		FVector2f IntersectionPoint;

	};

	struct FOverlapInfo {
		FVector2f TranslationVector;
		FVector2f IntersectionPoint;
		FVector2i DirectionOfCollided;
		bool isOverlapping{ false };
		bool OtherIsOptimized{ true };
	};


	/*
		NOTES
	*/

	/*
	
	Other way of abstracting this
	For Shapes Physics affects
	- translation
	- Rotation
	- Size

	Other way of abstracting this
	Physics affects
	- Points in shape
	- Shape as a whole

	For each triangle

	POINT BEHAVIOUR ON COLLISION
	Point collision is based on line collision
	Point physics just determine how a point moves when its line is hit

	TRIANGLE COLLISION
	Depending on the property of the triangle's material
	- There may be no individual point movement 
	- The other points will also move in accordance to the collision force
	- 

	MESH COLLISION
	Lets Keep the mesh structure

	- Pass 1 -> GetExternal forces 
	- Pass 2 -> Apply External Force -> thats is going to be a n^2 operation -> for each triangle, apply the force to every other triangle


	How to solve normal collisions and Torque collisions

	- In little prince notebook
	 
	
	*/

	//friction comes in when hit
	//remember there are two types of collision -> elastic and inelastic

	//OpenGL has a vector of matrices -> every time you draw -> all of the matrices are applied starting from the last pushed
	//OpenGl starts drawing on the bottom left corner -> keep that in mind for transformations
}

