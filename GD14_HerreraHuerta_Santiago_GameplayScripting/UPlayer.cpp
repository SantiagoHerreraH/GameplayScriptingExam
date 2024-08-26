#include "UPlayer.h"

namespace Pillar {
	UPlayer::UPlayer()
	{
		Body().WorldCircle.Radius = 10;
		Body().WorldCircle.Center = FVector2f{ 400,100 };
		Body().ScreenCircle = Body().WorldCircle;

		Shield().WorldCircle.Radius = m_MaxShieldRadius;
		Shield().ScreenCircle = Shield().WorldCircle;
	}
	void UPlayer::Update(float elapsedSec)
	{
		RespawnIfDied();
		UpdatePlayerInput(elapsedSec);
		UpdateGravity(elapsedSec);
		UpdateShieldState(elapsedSec);
		SetAndClampVelocity(elapsedSec);
		UpdateWoundedTime(elapsedSec);
	}
	void UPlayer::DrawLife() const
	{
		m_HealthBar.Draw(m_CurrentLife, m_MaxLife);
	}
	void UPlayer::Draw() const
	{

		if (m_Wounded)
		{
			m_Body.ScreenCircle.Draw(m_PlayerWoundedColor, true);
		}
		else
		{
			m_Body.ScreenCircle.Draw(m_PlayerNormalColor, true);
		}

		if (m_ShieldActivated)
		{
			m_Shield.ScreenCircle.Draw(FColor4i{ 255,255,255,255 }, true);
		}
		else
		{
			m_Shield.ScreenCircle.Draw(FColor4i{ 255,255,255,255 }, false, 5);
		}
		
	}
	void UPlayer::SetCameraDelta(const FVector2f& cameraDelta)
	{
		m_Body.ScreenCircle.Center.X = m_Body.WorldCircle.Center.X + cameraDelta.X;
		m_Body.ScreenCircle.Center.Y = m_Body.WorldCircle.Center.Y + cameraDelta.Y;

		m_Shield.ScreenCircle.Radius   = m_Shield.WorldCircle.Radius;
		m_Shield.ScreenCircle.Center.X = m_Shield.WorldCircle.Center.X + cameraDelta.X;
		m_Shield.ScreenCircle.Center.Y = m_Shield.WorldCircle.Center.Y + cameraDelta.Y;
	}
	bool UPlayer::IsShieldActivated() const
	{
		return m_ShieldActivated;
	}
	void UPlayer::GetDashDirection(float mousex, float mousey, bool invertIt)
	{
		if (!m_Wounded)
		{
			
		}

		FVector2f normalizedDirectionFromPlayerToMouse{};

		if (!invertIt)
		{
			normalizedDirectionFromPlayerToMouse = SVectors::NormalizeVector(
				SVectors::Subtract(FVector2f{ mousex, mousey }, m_Body.ScreenCircle.Center));
		}
		else
		{
			normalizedDirectionFromPlayerToMouse = SVectors::NormalizeVector(
				SVectors::Subtract(m_Body.ScreenCircle.Center, FVector2f{ mousex, mousey }));
		}

		m_CurrentShotForce =
			SVectors::Scale(normalizedDirectionFromPlayerToMouse, m_DashForceMagnitude);
	}
	void UPlayer::SetShieldPosition(float mouseScreenPosX, float mouseScreenPosY)
	{
		m_Shield.WorldCircle.Center =
			SVectors::Add(SVectors::Scale(
				SVectors::NormalizeVector(
					SVectors::Subtract(FVector2f{ mouseScreenPosX , mouseScreenPosY }, m_Body.ScreenCircle.Center)),
				m_ShieldDistanceFromBodyCenter), m_Body.WorldCircle.Center);
	}
	FCircleCollider& UPlayer::Shield()
	{
		return m_Shield;
	}
	FOverlapInfo& UPlayer::ShieldOverlapInfo()
	{
		return m_ShieldOverlapInfo;
	}
	std::vector<int>& UPlayer::BulletsToControl()
	{
		return m_BulletIndexesToControl;
	}
	void UPlayer::DeactivateShield()
	{
		m_ShieldActivated = false;
		m_ShieldReadyForDeactivation = true;
	}
	void UPlayer::ActivateShield()
	{
		m_ShieldActivated = true;
		m_ShieldReadyForDeactivation = false;
	}
	void UPlayer::ActivateShieldCollision()
	{
		m_ShieldCollided = true;
	}
	void UPlayer::DeactivateShieldCollision()
	{
		m_ShieldCollided = false;
	}
	float UPlayer::GetShieldForce() const
	{
		return m_CollisionImpactForceMagnitude;
	}
	float UPlayer::GetTimeMultiplier() const
	{
		return m_TimeMultiplier;
	}
	const FCircleCollider& UPlayer::ConstBody() const
	{
		return m_Body;
	}
	FCircleCollider& UPlayer::Body()
	{
		return m_Body;
	}
	FOverlapInfo& UPlayer::BodyOverlapInfo()
	{
		return m_BodyOverlapInfo;
	}
	const FVector2f& UPlayer::GetVelocity() const
	{
		return m_Velocity;
	}
	void UPlayer::SetVelocity(const FVector2f& velocity)
	{
		m_Velocity = velocity;
	}
	void UPlayer::SolveCollisionWithBullet(float elapsedSec)
	{
		//Solve for player
		m_Body.WorldCircle.Center.X += m_BodyOverlapInfo.TranslationVector.X;
		m_Body.WorldCircle.Center.Y += m_BodyOverlapInfo.TranslationVector.Y;

		FVector2f hitForce = SVectors::Scale(
			SVectors::NormalizeVector(m_BodyOverlapInfo.TranslationVector),
			m_CollisionImpactForceMagnitude * elapsedSec);

		m_Velocity.X += hitForce.X;
		m_Velocity.Y += hitForce.Y;

		if (!m_Wounded)
		{
			m_Wounded = true;
			m_CurrentWoundedTime = 0.f;
			m_CurrentLife -= 1;
		}
	}
	void UPlayer::SolveCollisionWithBounds(const FCirclef& bounds, float elapsedSec)
	{
		FVector2f boundCenterToPlayerCenter{
		SVectors::Subtract(
			bounds.Center,
			m_Body.WorldCircle.Center) };

		float totalLength{ SVectors::Length(boundCenterToPlayerCenter) + m_Body.WorldCircle.Radius };
		if (totalLength > bounds.Radius)
		{
			FVector2f deltaVectorNormalized{ SVectors::NormalizeVector(boundCenterToPlayerCenter) };
			FVector2f deltaVector{ SVectors::Scale(deltaVectorNormalized, totalLength - bounds.Radius) };
			m_Body.WorldCircle.Center.X += deltaVector.X;
			m_Body.WorldCircle.Center.Y += deltaVector.Y;

			FVector2f hitForce = SVectors::Scale(
				deltaVectorNormalized,
				SVectors::Length(m_Velocity) * elapsedSec * 10);

			m_Velocity.X += hitForce.X;
			m_Velocity.Y += hitForce.Y;
		}
	}
	void UPlayer::SolveCollisionWithRect(const FRectf& rect)
	{
		if (SCollision::IsOverlapping(m_Body.WorldCircle, rect, m_BodyOverlapInfo, false))
		{
			m_Body.WorldCircle.Center.X += m_BodyOverlapInfo.TranslationVector.X;
			m_Body.WorldCircle.Center.Y += m_BodyOverlapInfo.TranslationVector.Y;

			//m_Velocity.X += m_BodyOverlapInfo.TranslationVector.X;
			//m_Velocity.Y += m_BodyOverlapInfo.TranslationVector.Y;

			SCollision::SolveRectPhysicsCollision(m_Velocity, m_BodyOverlapInfo);
		}
	}
	FHealthBar& UPlayer::HealthBar()
	{
		return m_HealthBar;
	}
	void UPlayer::SetSpawnPoint(const FVector2f& position)
	{
		m_RespawnPoint = position;
	}
	void UPlayer::Respawn()
	{
		m_Body.WorldCircle.Center = m_RespawnPoint;
		m_Body.ScreenCircle.Center = m_RespawnPoint;
	}
	void UPlayer::UpdatePlayerInput(float elapsedSec)
	{
		const Uint8* pStates = SDL_GetKeyboardState(nullptr);
		if (!m_Wounded)
		{

			if (pStates[SDL_SCANCODE_D])
			{
				m_PressingRight = true;
				//if (m_Velocity.X < 0)
				//{
				//	m_Velocity.X = 0;
				//}
				m_Velocity.X += m_MovementSpeed * elapsedSec;
			}
			//else if (m_PressingRight)
			//{
			//	m_Velocity.X = 0;
			//	m_PressingRight = false;
			//}

			if (pStates[SDL_SCANCODE_A])
			{
				m_PressingLeft = true;

				//if (m_Velocity.X > 0)
				//{
				//	m_Velocity.X = 0;
				//}
				m_Velocity.X += -m_MovementSpeed * elapsedSec;
			}
			//else if (m_PressingLeft)
			//{
			//	m_Velocity.X = 0;
			//	m_PressingLeft = false;
			//}

			if (pStates[SDL_SCANCODE_W])// && !m_PressingUp)
			{
				m_PressingUp = true;

				//if (m_Velocity.Y < 0)
				//{
				//	m_Velocity.Y = 0;
				//}

				m_Velocity.Y += m_JumpSpeed * elapsedSec;
			}
			//else if (!pStates[SDL_SCANCODE_W] && m_PressingUp)
			//{
			//	m_Velocity.Y = 0;
			//	m_PressingUp = false;
			//}

			if (pStates[SDL_SCANCODE_S])// && !m_PressingDown)
			{
				m_PressingDown = true;
				//if (m_Velocity.Y > 0)
				//{
				//	m_Velocity.Y = 0;
				//}
				m_Velocity.Y += -m_JumpSpeed * elapsedSec;
			}
			//else if (!pStates[SDL_SCANCODE_S] && m_PressingDown)
			//{
			//	m_Velocity.Y = 0;
			//	m_PressingDown = false;
			//}
		}
	}

	void UPlayer::UpdateGravity(float elapsedSec)
	{
		m_Velocity.Y -= m_Gravity * elapsedSec;
	}

	void UPlayer::UpdateShieldState(float elapsedSec)
	{

		if (TimeIsSlowed)
		{
			elapsedSec = ConvertSlowModeTimeToNormalTime(elapsedSec);
		}

		if (m_ShieldActivated)// || (m_ShieldReadyForDeactivation && m_CurrentShieldActivationTime > 0)))
		{
			

			m_CurrentShieldActivationTime += elapsedSec;
			m_Shield.WorldCircle.Radius =
				STools::CalculateLerpValue(
					m_MaxShieldRadius,
					m_MinShieldRadius,
					m_CurrentShieldActivationTime / m_MaxShieldActivationTime);

			if (m_CurrentShieldActivationTime >= m_MaxShieldActivationTime)
			{
				m_CurrentShieldActivationTime = 0;
				m_Shield.WorldCircle.Radius = m_MaxShieldRadius;
				m_ShieldActivated = false;
			}

			if (m_ShieldCollided)
			{
				SlowDownTime();
			}
		}
		else
		{
			SetNormalTime();

			if (m_CurrentShieldActivationTime > 0)
			{
				m_CurrentShieldActivationTime -= elapsedSec;
				m_Shield.WorldCircle.Radius =
					STools::CalculateLerpValue(
						m_MaxShieldRadius,
						m_MinShieldRadius,
						m_CurrentShieldActivationTime / m_MaxShieldActivationTime);
			}
			else
			{
				m_CurrentShieldActivationTime = 0;
			}
			
		}

	}

	void UPlayer::SetAndClampVelocity(float elapsedSec)
	{
		FVector2f velocity{ m_Velocity.X, m_Velocity.Y };

		if (abs(velocity.X) <= m_MaxPlayerVelocityPerSecond)
		{
			m_Body.WorldCircle.Center.X += velocity.X * elapsedSec;
		}
		else
		{
			if (velocity.X > 0)
			{
				m_Body.WorldCircle.Center.X += m_MaxPlayerVelocityPerSecond * elapsedSec;
			}
			else
			{
				m_Body.WorldCircle.Center.X += -m_MaxPlayerVelocityPerSecond * elapsedSec;
			}
		}

		if (abs(velocity.Y) <= m_MaxPlayerVelocityPerSecond)
		{
			m_Body.WorldCircle.Center.Y += velocity.Y * elapsedSec;
		}
		else
		{
			if (velocity.Y > 0)
			{
				m_Body.WorldCircle.Center.Y += m_MaxPlayerVelocityPerSecond * elapsedSec;
			}
			else
			{
				m_Body.WorldCircle.Center.Y += -m_MaxPlayerVelocityPerSecond * elapsedSec;
			}
		}
	}

	void UPlayer::UpdateWoundedTime(float elapsedSec)
	{
		if (m_Wounded)
		{
			m_CurrentWoundedTime += elapsedSec;

			if (m_CurrentWoundedTime >= m_MaxWoundedTime)
			{
				m_CurrentWoundedTime = 0.f;
				m_Wounded = false;
			}
		}
	}

	void UPlayer::RespawnIfDied()
	{
		if (m_CurrentLife <= 0)
		{
			m_Body.WorldCircle.Center = m_RespawnPoint;
			m_CurrentLife = m_MaxLife;
		}
	}

	void UPlayer::SlowDownTime()
	{
		m_TimeMultiplier = 0.1f;
		TimeIsSlowed = true;
	}

	float UPlayer::ConvertSlowModeTimeToNormalTime(float currentDeltaSeconds)
	{
		return currentDeltaSeconds * 10;
	}

	void UPlayer::SetNormalTime()
	{
		m_TimeMultiplier = 1.f;
		TimeIsSlowed = false;
	}

}

