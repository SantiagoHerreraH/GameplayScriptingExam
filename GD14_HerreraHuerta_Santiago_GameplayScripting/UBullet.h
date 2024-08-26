#pragma once

#include "UsefulSystems/Includes.h"

namespace Pillar {

	enum class EBulletBehaviour {
		FollowPlayer,
		MoveStraightInCustomMovement,
		Patrol
	};

	enum class EBulletRank {
		Commander,
		Soldier
	};
	//Soldiers don't spawn anymore if Commanders are dead

	struct FBullet
	{
		FColor4i BulletParriedColor{ 255,0,255,255 };
		FColor4i BulletNormalColor{ 255,0,0,255 };

		bool AlwaysWounded{ false };
		bool IsStatic{false};
		bool IsSpawned{ true };

		FVector2f StartingPos{};

		EBulletBehaviour BulletBehaviour;
		FVector2f CustomMovementDirection{};
		bool AlwaysLookTowardsPlayer{ true };

		float DistanceToKeepFromPlayer{ 0.f };
		float FollowingSpeed{ 50000 };

		bool CollideWithLevel{ true };

		int CurrentLife{ 3 };
		int MaxLife{ 3 };
		float CurrentDeathTime{ 0.f };
		float MaxDeathTime{ 1.f };

		bool HaveTimedLife{ false };
		float MaxLifeTime{ 5.f };
		float CurrentLifeTime{ 0.f };

		float MaxHealthBodyRadius{ 20 };
		float MinHealthBodyRadius{ 10 };
		float MinBodyRadius{ 0 };

		FCircleCollider m_Body;
		FOverlapInfo m_BodyOverlapInfo;
		FVector2f m_Velocity;

		FVector2f CurrentFollowingOffset;
		bool SenseBound{ false };
		FCircleCollider Sense;

		bool Wounded{ false };
		float MaxWoundedTime{ 1.f };
		float CurrentWoundedTime{ 0.f };

		std::vector<FVector2f> PatrolPositions;
		int CurrentIndexToPatrol{};
		float MaxTimeToWaitAtEachPosition;
		float CurrentTimeWaited;
	};
}



