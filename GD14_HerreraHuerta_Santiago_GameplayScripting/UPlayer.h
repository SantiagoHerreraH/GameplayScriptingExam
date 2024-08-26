#pragma once
#include "UsefulSystems/Includes.h"

namespace Pillar {

	struct FHealthBar {


		FColor4i FillColor{ 0, 255, 0, 255 };
		FColor4i CellColor{ 255, 255 , 255 , 255 };
		float CellLineThickness{ 5 };
		FVector2f Position{};
		FVector2f Size{};

		void Draw(int currentLife, int maxLife) const {

			if (currentLife < 0 || maxLife <= 0)
			{
				return;
			}

			float oneCellRatio{ Size.X / (float)maxLife };
			float healthRatio{ currentLife / (float)maxLife };
			FRectf oneHealth{ Position.X, Position.Y, oneCellRatio, Size.Y };
			FRectf currentHealth{ Position.X, Position.Y, Size.X * healthRatio, Size.Y };

			currentHealth.Draw(FillColor, true, false);

			for (int i = 0; i < maxLife; i++)
			{
				oneHealth.Draw(CellColor, false, false, CellLineThickness);
				oneHealth.Left += oneCellRatio;
			}
		}
	};

	class UPlayer
	{
	public:
		UPlayer();
		void Update(float elapsedSec);
		void DrawLife()const;
		void Draw()const;
		void SetCameraDelta(const FVector2f& cameraDelta);

		void SetShieldPosition(float mouseScreenPosX, float mouseScreenPosY);
		FCircleCollider& Shield();
		FOverlapInfo& ShieldOverlapInfo();

		void DeactivateShield();
		void ActivateShield();
		bool IsShieldActivated()const;

		void GetDashDirection(float mousex, float mousey, bool invertIt);

		void ActivateShieldCollision();
		void DeactivateShieldCollision();
		float GetShieldForce()const;

		float GetTimeMultiplier()const;

		FCircleCollider& Body();
		FOverlapInfo& BodyOverlapInfo();

		const FVector2f& GetVelocity()const;
		void SetVelocity(const FVector2f& velocity);

		void SolveCollisionWithBullet(float elapsedSec);
		void SolveCollisionWithBounds(const FCirclef& bounds, float elapsedSec);
		void SolveCollisionWithRect(const FRectf& rect);

		FHealthBar& HealthBar();

		void SetSpawnPoint(const FVector2f& position);
		void Respawn();

	private:
		void UpdatePlayerInput(float elapsedSec);
		void UpdateGravity(float elapsedSec);
		void UpdateShieldState(float elapsedSec);
		void SetAndClampVelocity(float elapsedSec);
		void UpdateWoundedTime(float elapsedSec);
		void RespawnIfDied();
		void SlowDownTime();
		float ConvertSlowModeTimeToNormalTime(float currentDeltaSeconds);
		void SetNormalTime();


		float m_TimeMultiplier{ 1.f };
		FHealthBar m_HealthBar;

		FColor4i m_PlayerWoundedColor{ 255,100,100,255 };
		FColor4i m_PlayerNormalColor{ 0,255,0,255 };

		FVector2f m_RespawnPoint{ 0,0 };

		bool m_PressingUp{ false };
		bool m_PressingDown{ false };
		bool m_PressingLeft{ false };
		bool m_PressingRight{ false };

		float m_MaxShieldRadius{ 40.f };
		float m_MinShieldRadius{ 0.f };
		FCircleCollider m_Shield;
		FOverlapInfo m_ShieldOverlapInfo;
		float m_ShieldDistanceFromBodyCenter{ 10 };
		bool  m_ShieldCollided{ false };
		bool  m_ShieldActivated{ false };
		bool  m_ShieldReadyForDeactivation{ true };
		float m_MaxShieldActivationTime{ 1.5f };
		float m_CurrentShieldActivationTime{ 0.f };

		FCircleCollider m_Body;
		FOverlapInfo m_BodyOverlapInfo;
		FVector2f m_Velocity;
		FVector2f m_CurrentShotForce; 
		FCircleCollider m_CurrentShotForceCircle;

		float m_MaxGravity{ 1000 };
		float m_MinGravity{0};
		float m_Gravity{ m_MaxGravity };
		float m_MovementSpeed{ 40000 };
		float m_JumpSpeed{ 70000 };

		float m_MaxPlayerVelocityPerSecond{ 1000 };
		float m_DashForceMagnitude{ 100000 };
		float m_CollisionImpactForceMagnitude{ 500 };

		bool  m_Wounded{ false };
		float m_CurrentWoundedTime{};
		float m_MaxWoundedTime{ 1.f };

		int m_MaxLife{ 3 };
		int m_CurrentLife{ 3 };
	};
}



