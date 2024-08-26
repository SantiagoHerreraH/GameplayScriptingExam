#include "pch.h"
#include "Game.h"
#include "utils.h"

Game::Game( const Window& window ) 
	:BaseGame{ window }
{
	Initialize();
}

Game::~Game( )
{
	Cleanup( );
}

void Game::Initialize( )
{
	LoadLevel(m_CurrentLevelIndex);
	//LoadLevel(3);
	m_MousePositionScreenIndicator.Radius = 5;

	m_Player.HealthBar().Size = FVector2f{ 200,50 };
	m_Player.HealthBar().Position = FVector2f{ 30, GetViewPort().height - m_Player.HealthBar().Size.Y - 10};
	
	m_Camera.Position = m_Player.Body().WorldCircle.Center;
}

void Game::Cleanup( )
{
	UTexture::DeleteAll();
}

void Game::Update( float deltaSeconds )
{
	// Check keyboard state
	
	const double elapsedSec{ STools::Clamp(deltaSeconds * m_Player.GetTimeMultiplier(), 0, 0.01)};

	FVector2f followingOffset{};

	for (size_t i = 0; i < m_Bullets.size(); i++)
	{
		if (m_Bullets.at(i).AlwaysLookTowardsPlayer)
		{
			m_Bullets.at(i).CustomMovementDirection = 
				SVectors::NormalizeVector(SVectors::Subtract(
					m_Player.Body().WorldCircle.Center,
					m_Bullets.at(i).m_Body.WorldCircle.Center));
		}

		if (m_Bullets.at(i).HaveTimedLife)
		{
			m_Bullets.at(i).CurrentLifeTime += elapsedSec;

			m_Bullets.at(i).CurrentLife = 
				m_Bullets.at(i).MaxLife * 
				(m_Bullets.at(i).CurrentLifeTime/ m_Bullets.at(i).MaxLifeTime);

			if (m_Bullets.at(i).CurrentLifeTime >= m_Bullets.at(i).MaxLifeTime)
			{
				m_Bullets.at(i).CurrentLifeTime = 0;
				m_Bullets.at(i).CurrentLife = 0;
			}
		}

		if ((m_Bullets.at(i).CurrentLife <= 0) && !m_Bullets.at(i).AlwaysWounded)
		{

			m_Bullets.at(i).CurrentDeathTime += elapsedSec;

			m_Bullets.at(i).m_Body.WorldCircle.Radius =
				STools::CalculateLerpValue(
					m_Bullets.at(i).MinHealthBodyRadius,
					m_Bullets.at(i).MinBodyRadius,
					m_Bullets.at(i).CurrentDeathTime / m_Bullets.at(i).MaxDeathTime);

			m_Bullets.at(i).m_Body.ScreenCircle.Radius = m_Bullets.at(i).m_Body.WorldCircle.Radius;

			if (m_Bullets.at(i).CurrentDeathTime >= m_Bullets.at(i).MaxDeathTime )
			{

				m_Bullets.at(i).IsSpawned = false;
				m_Bullets.at(i).Wounded = false;
				m_Bullets.at(i).CurrentDeathTime = 0;
				m_Bullets.at(i).CurrentLife = m_Bullets.at(i).MaxLife;
				m_Bullets.at(i).m_Body.WorldCircle.Radius = m_Bullets.at(i).MaxHealthBodyRadius;

				m_Bullets.at(i).m_Body.ScreenCircle = m_Bullets.at(i).m_Body.WorldCircle;
			}
		}

		if (!m_Bullets.at(i).Wounded && !m_Bullets.at(i).AlwaysWounded && !m_Bullets.at(i).IsStatic)
		{
			if (m_Bullets.at(i).BulletBehaviour == EBulletBehaviour::FollowPlayer)
			{
				if (!m_Bullets.at(i).SenseBound ||
					(m_Bullets.at(i).SenseBound && 
						SCollision::IsOverlapping(m_Bullets.at(i).Sense.WorldCircle, m_Player.Body().WorldCircle)))
				{

					FVector2f deltaToPlayer{ 
						SVectors::Subtract(
							m_Player.Body().WorldCircle.Center,
							m_Bullets.at(i).m_Body.WorldCircle.Center) };
					
					followingOffset =
						SVectors::Scale(SVectors::NormalizeVector(
							deltaToPlayer),
							SVectors::Length(deltaToPlayer) - m_Bullets.at(i).DistanceToKeepFromPlayer);

					m_Bullets.at(i).CurrentFollowingOffset =
						SVectors::Scale(SVectors::NormalizeVector(followingOffset),
							m_Bullets.at(i).FollowingSpeed);


					m_Bullets.at(i).m_Velocity.X = m_Bullets.at(i).CurrentFollowingOffset.X * elapsedSec;
					m_Bullets.at(i).m_Velocity.Y = m_Bullets.at(i).CurrentFollowingOffset.Y * elapsedSec;
				}
				

			}
			else if (m_Bullets.at(i).BulletBehaviour == EBulletBehaviour::MoveStraightInCustomMovement)
			{
				m_Bullets.at(i).m_Velocity.X = m_Bullets.at(i).CustomMovementDirection.X * m_Bullets.at(i).FollowingSpeed * elapsedSec;
				m_Bullets.at(i).m_Velocity.Y = m_Bullets.at(i).CustomMovementDirection.Y * m_Bullets.at(i).FollowingSpeed * elapsedSec;
			}
			else if (m_Bullets.at(i).BulletBehaviour == EBulletBehaviour::Patrol)
			{
				FVector2f currentPatrolPoint{
					m_Bullets.at(i).PatrolPositions.at(m_Bullets.at(i).CurrentIndexToPatrol) };

				followingOffset =
					SVectors::Scale(SVectors::NormalizeVector(
						SVectors::Subtract(currentPatrolPoint, m_Bullets.at(i).m_Body.WorldCircle.Center)),
						m_Bullets.at(i).FollowingSpeed);

				if (SCollision::IsPointInCircle(currentPatrolPoint, m_Bullets.at(i).m_Body.WorldCircle))
				{
					m_Bullets.at(i).CurrentTimeWaited += elapsedSec;

					if (m_Bullets.at(i).CurrentTimeWaited >= m_Bullets.at(i).MaxTimeToWaitAtEachPosition)
					{
						m_Bullets.at(i).CurrentTimeWaited = 0;

						m_Bullets.at(i).CurrentIndexToPatrol =
							m_Bullets.at(i).CurrentIndexToPatrol < (m_Bullets.at(i).PatrolPositions.size() - 1) ?
							m_Bullets.at(i).CurrentIndexToPatrol + 1 : 0;
					}
				}
				else
				{
					m_Bullets.at(i).m_Velocity.X = followingOffset.X * elapsedSec;
					m_Bullets.at(i).m_Velocity.Y = followingOffset.Y * elapsedSec;
				}
			}
			
		}
		else
		{
			m_Bullets.at(i).CurrentWoundedTime += elapsedSec;

			if (m_Bullets.at(i).CurrentWoundedTime >= m_Bullets.at(i).MaxWoundedTime)
			{
				m_Bullets.at(i).CurrentWoundedTime = 0;
				m_Bullets.at(i).Wounded = false;
			}
		}
		

		m_Bullets.at(i).m_Body.WorldCircle.Center.X += m_Bullets.at(i).m_Velocity.X * elapsedSec;
		m_Bullets.at(i).m_Body.WorldCircle.Center.Y += m_Bullets.at(i).m_Velocity.Y * elapsedSec;
	}

	m_Player.Update(elapsedSec);
	m_Player.DeactivateShieldCollision();

	FVector2f hitForce{};

	for (size_t i = 0; i < m_Bullets.size(); i++)
	{

		if (m_Player.IsShieldActivated())
		{
			if (m_Bullets.at(i).IsSpawned && 
				SCollision::IsOverlapping(
				m_Bullets.at(i).m_Body.WorldCircle, m_Player.Shield().WorldCircle,
				m_Bullets.at(i).m_BodyOverlapInfo, m_Player.ShieldOverlapInfo()))
			{

				m_Player.ActivateShieldCollision();

				if (!m_Bullets.at(i).IsStatic)
				{
					//m_Bullets.at(i).m_Body.WorldCircle.Center.X += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.X;
					//m_Bullets.at(i).m_Body.WorldCircle.Center.Y += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.Y;
					//
					//hitForce = SVectors::Scale(
					//	SVectors::NormalizeVector(SVectors::Subtract(m_CurrentMouseScreenPosition, m_Player.Body().ScreenCircle.Center)),
					//	m_Player.GetShieldForce());
					//
					//m_Bullets.at(i).m_Velocity.X = hitForce.X;// * elapsedSec;
					//m_Bullets.at(i).m_Velocity.Y = hitForce.Y;// * elapsedSec;

					m_Player.BulletsToControl().push_back(i);

					m_Bullets.at(i).Wounded = true;
					m_Bullets.at(i).CurrentWoundedTime = 0;
				}
			}
		}

		if (m_Bullets.at(i).IsSpawned && 
			SCollision::IsOverlapping(
			m_Bullets.at(i).m_Body.WorldCircle, m_Player.Body().WorldCircle,
			m_Bullets.at(i).m_BodyOverlapInfo, m_Player.BodyOverlapInfo()))
		{
			
			m_Player.SolveCollisionWithBullet(elapsedSec);

			if (!m_Bullets.at(i).IsStatic)
			{
				m_Bullets.at(i).m_Body.WorldCircle.Center.X += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.X;
				m_Bullets.at(i).m_Body.WorldCircle.Center.Y += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.Y;

				hitForce = SVectors::Scale(
					SVectors::NormalizeVector(m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector),
					m_Player.GetShieldForce());

				//m_Bullets.at(i).m_Velocity.X += hitForce.X * elapsedSec;
				//m_Bullets.at(i).m_Velocity.Y += hitForce.Y * elapsedSec;

				m_Bullets.at(i).m_Velocity.X = hitForce.X;// * elapsedSec;
				m_Bullets.at(i).m_Velocity.Y = hitForce.Y;// * elapsedSec;
			}
			
		}

		if (m_Bullets.at(i).IsSpawned)
		{
			for (size_t j = i + 1; j < m_Bullets.size(); j++)
			{
				if ((m_Bullets.at(i).Wounded || m_Bullets.at(i).AlwaysWounded) || (m_Bullets.at(j).Wounded || m_Bullets.at(j).AlwaysWounded))
				{
					if (SCollision::IsOverlapping(
						m_Bullets.at(i).m_Body.WorldCircle, m_Bullets.at(j).m_Body.WorldCircle,
						m_Bullets.at(i).m_BodyOverlapInfo, m_Bullets.at(j).m_BodyOverlapInfo))
					{
						if (!m_Bullets.at(i).IsStatic)
						{
							//Solve for enemy one
							m_Bullets.at(i).m_Body.WorldCircle.Center.X += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.X;
							m_Bullets.at(i).m_Body.WorldCircle.Center.Y += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.Y;

							hitForce = SVectors::Scale(
								SVectors::NormalizeVector(m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector),
								m_Player.GetShieldForce());

							//m_Bullets.at(i).m_Velocity.X += hitForce.X * elapsedSec;
							//m_Bullets.at(i).m_Velocity.Y += hitForce.Y * elapsedSec;

							m_Bullets.at(i).m_Velocity.X = hitForce.X;// * elapsedSec;
							m_Bullets.at(i).m_Velocity.Y = hitForce.Y;// * elapsedSec;
						}

						

						if ((!m_Bullets.at(i).Wounded || m_Bullets.at(j).IsStatic || m_Bullets.at(j).AlwaysWounded) && !m_Bullets.at(i).AlwaysWounded)
						{
							m_Bullets.at(i).Wounded = true;
							m_Bullets.at(i).CurrentWoundedTime = 0;
							m_Bullets.at(i).CurrentLife -= 1;
							m_Bullets.at(i).CurrentDeathTime = 0.f;
							m_Bullets.at(i).m_Body.WorldCircle.Radius =
								STools::CalculateLerpValue(
									m_Bullets.at(i).MaxHealthBodyRadius,
									m_Bullets.at(i).MinHealthBodyRadius,
									m_Bullets.at(i).CurrentLife / (float)m_Bullets.at(i).MaxLife);
							m_Bullets.at(i).m_Body.ScreenCircle.Radius = m_Bullets.at(i).m_Body.WorldCircle.Radius;
						}

						if (!m_Bullets.at(j).IsStatic) {

							//Solve for enemy two
							m_Bullets.at(j).m_Body.WorldCircle.Center.X += m_Bullets.at(j).m_BodyOverlapInfo.TranslationVector.X;
							m_Bullets.at(j).m_Body.WorldCircle.Center.Y += m_Bullets.at(j).m_BodyOverlapInfo.TranslationVector.Y;

							hitForce = SVectors::Scale(
								SVectors::NormalizeVector(m_Bullets.at(j).m_BodyOverlapInfo.TranslationVector),
								m_Player.GetShieldForce());

							//m_Bullets.at(i).m_Velocity.X += hitForce.X * elapsedSec;
							//m_Bullets.at(i).m_Velocity.Y += hitForce.Y * elapsedSec;

							m_Bullets.at(i).m_Velocity.X = hitForce.X;// * elapsedSec;
							m_Bullets.at(i).m_Velocity.Y = hitForce.Y;// * elapsedSec;
						}
						

						if ((!m_Bullets.at(j).Wounded || m_Bullets.at(i).IsStatic || m_Bullets.at(i).AlwaysWounded) && !m_Bullets.at(j).AlwaysWounded)
						{
							m_Bullets.at(j).Wounded = true;
							m_Bullets.at(j).CurrentWoundedTime = 0;
							m_Bullets.at(j).CurrentLife -= 1;
							m_Bullets.at(j).CurrentDeathTime = 0.f;
							m_Bullets.at(j).m_Body.WorldCircle.Radius =
								STools::CalculateLerpValue(
									m_Bullets.at(j).MaxHealthBodyRadius,
									m_Bullets.at(j).MinHealthBodyRadius,
									m_Bullets.at(j).CurrentLife / (float)m_Bullets.at(j).MaxLife);
							m_Bullets.at(j).m_Body.ScreenCircle.Radius = m_Bullets.at(j).m_Body.WorldCircle.Radius;
						}
					}
				}
			}

			if ((m_Bullets.at(i).CollideWithLevel || m_Bullets.at(i).Wounded)
				&& !m_Bullets.at(i).IsStatic)
			{
				for (size_t lvlCollisionIdx = 0; lvlCollisionIdx < m_CurrentLevel.size(); lvlCollisionIdx++)
				{
					if (SCollision::IsOverlapping(m_Bullets.at(i).m_Body.WorldCircle, m_CurrentLevel.at(lvlCollisionIdx).WorldRect, m_Bullets.at(i).m_BodyOverlapInfo, false))
					{
						m_Bullets.at(i).m_Body.WorldCircle.Center.X += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.X;
						m_Bullets.at(i).m_Body.WorldCircle.Center.Y += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.Y;

						m_Bullets.at(i).m_Velocity.X = m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.X;
						m_Bullets.at(i).m_Velocity.Y = m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.Y;
					}
				}
			}
			

		}
		
	}
	

	for (size_t i = 0; i < m_CurrentLevel.size(); i++)
	{
		m_Player.SolveCollisionWithRect(m_CurrentLevel.at(i).WorldRect);
	}
	
	for (size_t i = 0; i < m_SpawnPoints.size(); i++)
	{
		if (SCollision::IsOverlapping(m_Player.Body().WorldCircle, m_SpawnPoints.at(i).WorldRect, false))
		{
			FVector2f middlePos{
				m_SpawnPoints.at(i).WorldRect.Left + m_SpawnPoints.at(i).WorldRect.Width  / 2.f,
				m_SpawnPoints.at(i).WorldRect.Top  - m_SpawnPoints.at(i).WorldRect.Height / 2.f,
			};

			m_Player.SetSpawnPoint(middlePos);
		}
	}

	if (SCollision::IsOverlapping(m_Player.Body().WorldCircle, m_WinBounds.WorldRect, false))
	{
		NextLevel();
	}

	// --- CAMERA LOGIC --- 
	
	FVector2f cameraDirection{ 
			SVectors::Subtract(m_Player.Body().WorldCircle.Center, m_Camera.Position)};
	m_Camera.Position.X += cameraDirection.X * m_Camera.FollowingSpeed * elapsedSec;
	m_Camera.Position.Y += cameraDirection.Y * m_Camera.FollowingSpeed * elapsedSec;


	// --- OFFSET

	FVector2f cameraDelta{
	(GetViewPort().width / 2.f) - m_Camera.Position.X,
	(GetViewPort().height / 2.f) - m_Camera.Position.Y
	};

	//Player

	m_Player.SetShieldPosition(m_CurrentMouseScreenPosition.X, m_CurrentMouseScreenPosition.Y);
	m_Player.SetCameraDelta(cameraDelta);


	m_WinBounds.ScreenRect.Left = m_WinBounds.WorldRect.Left + cameraDelta.X;
	m_WinBounds.ScreenRect.Top  = m_WinBounds.WorldRect.Top + cameraDelta.Y;

	for (size_t i = 0; i < m_CurrentLevel.size(); i++)
	{
		m_CurrentLevel.at(i).ScreenRect.Left = m_CurrentLevel.at(i).WorldRect.Left + cameraDelta.X;
		m_CurrentLevel.at(i).ScreenRect.Top =  m_CurrentLevel.at(i).WorldRect.Top + cameraDelta.Y;
	}

	for (size_t i = 0; i < m_SpawnPoints.size(); i++)
	{
		m_SpawnPoints.at(i).ScreenRect.Left = m_SpawnPoints.at(i).WorldRect.Left + cameraDelta.X;
		m_SpawnPoints.at(i).ScreenRect.Top = m_SpawnPoints.at(i).WorldRect.Top + cameraDelta.Y;
	}

	for (size_t i = 0; i < m_Bullets.size(); i++)
	{
		if (m_Bullets.at(i).IsSpawned)
		{
			m_Bullets.at(i).m_Body.ScreenCircle.Center.X = m_Bullets.at(i).m_Body.WorldCircle.Center.X + cameraDelta.X;
			m_Bullets.at(i).m_Body.ScreenCircle.Center.Y = m_Bullets.at(i).m_Body.WorldCircle.Center.Y + cameraDelta.Y;

			if (m_Bullets.at(i).SenseBound)
			{
				m_Bullets.at(i).Sense.WorldCircle.Center.X	= m_Bullets.at(i).m_Body.WorldCircle.Center.X;
				m_Bullets.at(i).Sense.WorldCircle.Center.Y	= m_Bullets.at(i).m_Body.WorldCircle.Center.Y;

				m_Bullets.at(i).Sense.ScreenCircle.Center.X = m_Bullets.at(i).Sense.WorldCircle.Center.X + cameraDelta.X;
				m_Bullets.at(i).Sense.ScreenCircle.Center.Y = m_Bullets.at(i).Sense.WorldCircle.Center.Y + cameraDelta.Y;
			}
		}
	}

}

void Game::Draw( ) const
{
	ClearBackground( );


	for (size_t i = 0; i < m_SpawnPoints.size(); i++)
	{
		m_SpawnPoints.at(i).ScreenRect.Draw(FColor4i{ 0,255,0,255 }, true, false);
	}

	m_WinBounds.ScreenRect.Draw(FColor4i{ 255,255,0,255 }, true, false);

	m_Player.DrawLife();

	m_MousePositionScreenIndicator.Draw(FColor4i{ 255,255,255,255 }, false, 5.f);

	for (size_t i = 0; i < m_Bullets.size(); i++)
	{
		if (m_Bullets.at(i).IsSpawned)
		{
			if (m_Bullets.at(i).AlwaysWounded)
			{
				m_Bullets.at(i).m_Body.ScreenCircle.Draw(m_Bullets.at(i).BulletParriedColor, false, 7.f);
			}
			if (m_Bullets.at(i).Wounded)
			{
				m_Bullets.at(i).m_Body.ScreenCircle.Draw(m_Bullets.at(i).BulletParriedColor, true);
			}
			else
			{
				m_Bullets.at(i).m_Body.ScreenCircle.Draw(m_Bullets.at(i).BulletNormalColor, true);
			}

			if (m_Bullets.at(i).SenseBound)
			{
				m_Bullets.at(i).Sense.ScreenCircle.Draw(FColor4i{255, 255, 255, 255 }, false, 5.f);
			}
		}
		
	}


	for (size_t i = 0; i < m_CurrentLevel.size(); i++)
	{
		m_CurrentLevel.at(i).ScreenRect.Draw(FColor4i{ 255,255,255,255 }, true, false);
	}

	if (m_Player.IsShieldActivated())
	{
		utils::DrawLine(
			m_MousePositionScreenIndicator.Center.X,
			m_MousePositionScreenIndicator.Center.Y,
			m_Player.ConstBody().ScreenCircle.Center.X,
			m_Player.ConstBody().ScreenCircle.Center.Y,
			3.f);
	}

	m_Player.Draw();
	
	
	
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	switch ( e.keysym.sym )
	{
	case SDLK_r:
		m_Player.Respawn();
		//std::cout << "Left arrow key released\n";
		break;
	case SDLK_RIGHT:
		//std::cout << "`Right arrow key released\n";
		break;
	case SDLK_1:
	case SDLK_KP_1:
		//std::cout << "Key 1 released\n";
		break;
	}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;

	m_CurrentMouseScreenPosition.X = e.x;
	m_CurrentMouseScreenPosition.Y = e.y;
	m_MousePositionScreenIndicator.Center = m_CurrentMouseScreenPosition;
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}

	if (e.button == SDL_BUTTON_LEFT)
	{
		m_Player.ActivateShield();
	}
	if (e.button == SDL_BUTTON_RIGHT)
	{
		m_Player.ActivateShield();
	}
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
	//if (e.button == SDL_BUTTON_LEFT)
	//{
	//	m_Player.GetDashDirection(e.x, e.y, false);
	//}
	//if (e.button == SDL_BUTTON_RIGHT)
	//{
	//	m_Player.GetDashDirection(e.x, e.y, true);
	//}
	if (e.button != SDL_BUTTON_MIDDLE)
	{
		m_Player.DeactivateShield();

		FVector2f impulseDirection{};
		FBullet* currentBullet{ nullptr };

		for (size_t i = 0; i < m_Player.BulletsToControl().size(); i++)
		{
			currentBullet = &m_Bullets.at(m_Player.BulletsToControl().at(i));
			impulseDirection = SVectors::Subtract(FVector2f{ (float)e.x, (float)e.y }, currentBullet->m_Body.ScreenCircle.Center);

			impulseDirection = SVectors::Scale(
				SVectors::NormalizeVector(impulseDirection),
				m_Player.GetShieldForce());

			currentBullet->m_Velocity = impulseDirection;
		}

		m_Player.BulletsToControl().clear();
	}
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.3f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::ResetLevel()
{
	LoadLevel(m_CurrentLevelIndex);
}

void Game::NextLevel()
{
	m_CurrentLevelIndex = m_CurrentLevelIndex < (m_MaxLevelNum - 1) ? m_CurrentLevelIndex + 1 : 0;
	LoadLevel(m_CurrentLevelIndex);
}

void Game::LoadLevel(int levelIndex)
{
	UTextureData currentLevel{UTexture(m_GeneralLevelString + std::to_string(levelIndex) + ".png")};

	std::vector<FRectf> rects{ SGeometry::Simplify(currentLevel.MakeRectsFromPixelsOfColor(m_LevelCollisionColor, m_LevelScale.X,m_LevelScale.Y) )};
	m_CurrentLevel.clear();
	m_CurrentLevel.reserve(rects.size());

	FRectCollider currentRect{};

	for (size_t i = 0; i < rects.size(); i++)
	{
		currentRect.ScreenRect = rects.at(i);
		currentRect.WorldRect  = rects.at(i);

		m_CurrentLevel.push_back(currentRect);
	}

	//rects = currentLevel.MakeRectsFromPixelsOfColor(m_StaticShooterColor, m_LevelScale.X,m_LevelScale.Y);
	//m_Bullets.clear();
	//m_CurrentLevel.reserve(rects.size());
	//
	//for (size_t i = 0; i < rects.size(); i++)
	//{
	//	CreateStaticShooterEnemy();
	//}

	rects = SGeometry::Simplify(currentLevel.MakeRectsFromPixelsOfColor(m_PatrolEnemyColor, m_LevelScale.X,m_LevelScale.Y));
	m_Bullets.clear();
	
	FVector2f currentPosition{};
	FVector2f otherPosition{};

	for (size_t i = 0; i < rects.size(); i++)
	{
		currentPosition.X = rects.at(i).Left;
		currentPosition.Y = rects.at(i).Top;

		otherPosition.X = rects.at(i).Width < rects.at(i).Height ? rects.at(i).Left : rects.at(i).Left + rects.at(i).Width;
		otherPosition.Y = rects.at(i).Width < rects.at(i).Height ? rects.at(i).Top - rects.at(i).Height : rects.at(i).Top;;

		CreatePatrolEnemy(3, true, currentPosition, otherPosition);
	}

	rects = currentLevel.MakeRectsFromPixelsOfColor(m_FollowPlayerEnemyColor, m_LevelScale.X, m_LevelScale.Y);

	for (size_t i = 0; i < rects.size(); i++)
	{
		currentPosition.X = rects.at(i).Left + (rects.at(i).Width/2.f );
		currentPosition.Y = rects.at(i).Top  - (rects.at(i).Height/2.f);

		CreateFollowingEnemy(3, currentPosition);
	}
	
	rects = currentLevel.MakeRectsFromPixelsOfColor(m_StaticBulletColor, m_LevelScale.X, m_LevelScale.Y);
	m_Bullets.reserve(m_Bullets.size() + rects.size());

	for (size_t i = 0; i < rects.size(); i++)
	{
		currentPosition.X = rects.at(i).Left + (rects.at(i).Width / 2.f);
		currentPosition.Y = rects.at(i).Top - (rects.at(i).Height / 2.f);

		m_Bullets.push_back(CreateStaticBullet(3, currentPosition));
	}

	rects = currentLevel.MakeRectsFromPixelsOfColor(m_MovableBulletColor, m_LevelScale.X, m_LevelScale.Y);
	m_Bullets.reserve(m_Bullets.size() + rects.size());

	for (size_t i = 0; i < rects.size(); i++)
	{
		currentPosition.X = rects.at(i).Left + (rects.at(i).Width / 2.f);
		currentPosition.Y = rects.at(i).Top - (rects.at(i).Height / 2.f);

		m_Bullets.push_back(CreateMovableBullet(3, currentPosition));
	}
	
	rects = SGeometry::Simplify(currentLevel.MakeRectsFromPixelsOfColor(m_SpawnPointColor, m_LevelScale.X, m_LevelScale.Y));
	m_SpawnPoints.clear();
	FVector2f leftestSpawnPoint{INFINITY, INFINITY};
	float currentCenter{};

	for (size_t i = 0; i < rects.size(); i++)
	{
		currentCenter = (rects.at(i).Left + rects.at(i).Width / 2.f);
		if (currentCenter < leftestSpawnPoint.X )
		{
			leftestSpawnPoint.X = currentCenter;
			leftestSpawnPoint.Y = rects.at(i).Top - (rects.at(i).Height / 2.f);
		}
		CreateSpawnPoint(rects.at(i));
	}

	m_Player.SetSpawnPoint(leftestSpawnPoint);
	m_Player.Respawn();
	m_Player.SetVelocity(FVector2f{ 0,0 });

	rects = SGeometry::Simplify(currentLevel.MakeRectsFromPixelsOfColor(m_WinPointColor, m_LevelScale.X, m_LevelScale.Y));
	m_WinBounds.ScreenRect = rects.at(0);
	m_WinBounds.WorldRect = rects.at(0);
}

void Game::CreateSpawnPoint(const FRectf& rect)
{
	FRectCollider rectColl{};
	rectColl.ScreenRect = rect;
	rectColl.WorldRect = rect;
	m_SpawnPoints.push_back(rectColl);
}



void Game::CreateStaticShooterEnemy(int maxLife, FVector2f shootDirection, const FVector2f& position)
{
	FBullet enemy{};

	enemy.BulletNormalColor = FColor4i{ 50,50,10, 255 };

	enemy.IsSpawned = true;
	enemy.IsStatic = true;

	enemy.AlwaysLookTowardsPlayer = false;
	enemy.CustomMovementDirection = shootDirection;
	enemy.StartingPos = position;
	enemy.CollideWithLevel = true;

	enemy.CurrentLife = maxLife;
	enemy.MaxLife = maxLife;
	enemy.CurrentDeathTime = 0.f;
	enemy.MaxDeathTime = 1.f;

	enemy.HaveTimedLife = false;

	enemy.MaxHealthBodyRadius = 35;
	enemy.MinHealthBodyRadius = 10;
	enemy.MinBodyRadius = 0;

	enemy.m_Body.WorldCircle.Center = position;
	enemy.m_Body.WorldCircle.Radius = enemy.MaxHealthBodyRadius;
	enemy.m_Body.ScreenCircle = enemy.m_Body.WorldCircle;

	enemy.SenseBound = false;

	enemy.Wounded = false;
	enemy.MaxWoundedTime = 1.f;
	enemy.CurrentWoundedTime = 0.f;

	m_Bullets.push_back(enemy);
}

void Game::CreatePatrolEnemy(int maxLife, bool alwaysLookTowardsPlayer, FVector2f beginPatrol, FVector2f endPatrol)
{

	FBullet enemy{};

	enemy.BulletNormalColor = FColor4i{255,10,10, 255};
	enemy.StartingPos = beginPatrol;
	enemy.IsSpawned = true;

	enemy.BulletBehaviour = EBulletBehaviour::Patrol;
	enemy.AlwaysLookTowardsPlayer = alwaysLookTowardsPlayer;
	enemy.CustomMovementDirection = SVectors::OrthogonalVector(SVectors::Subtract(endPatrol, beginPatrol));

	//enemy.FollowingSpeed = 200 ;

	enemy.CollideWithLevel = true ;

	enemy.CurrentLife = maxLife;
	enemy.MaxLife = maxLife;
	enemy.CurrentDeathTime = 0.f;
	enemy.MaxDeathTime = 1.f;

	enemy.HaveTimedLife = false;
	//enemy.MaxLifeTime{ 5.f };
	//enemy.CurrentLifeTime{ 0.f };

	enemy.MaxHealthBodyRadius = 40;
	enemy.MinHealthBodyRadius = 10;
	enemy.MinBodyRadius = 0;

	enemy.m_Body.WorldCircle.Center = beginPatrol;
	enemy.m_Body.WorldCircle.Radius = enemy.MaxHealthBodyRadius;
	enemy.m_Body.ScreenCircle = enemy.m_Body.WorldCircle;
	//enemy.m_BodyOverlapInfo;
	//enemy.m_Velocity;

	enemy.SenseBound = false;
	//enemy.Sense;

	enemy.Wounded = false;
	enemy.MaxWoundedTime = 1.f ;
	enemy.CurrentWoundedTime = 0.f;

	enemy.PatrolPositions.reserve(2);
	enemy.PatrolPositions.push_back(beginPatrol);
	enemy.PatrolPositions.push_back(endPatrol);
	enemy.CurrentIndexToPatrol = 0;
	enemy.MaxTimeToWaitAtEachPosition = 1.f;
	enemy.CurrentTimeWaited = 0;

	m_Bullets.push_back(enemy);
}

void Game::CreateFreePatrolEnemy(int maxLife, const std::vector<FVector2f>& patrolPoints )
{
	FBullet enemy{};

	enemy.BulletNormalColor = FColor4i{ 255,255,10, 255 };

	enemy.IsSpawned = true;

	enemy.BulletBehaviour = EBulletBehaviour::Patrol;
	enemy.AlwaysLookTowardsPlayer = true;

	//enemy.FollowingSpeed = 200;

	enemy.CollideWithLevel = true;

	enemy.CurrentLife = maxLife;
	enemy.MaxLife = maxLife;
	enemy.CurrentDeathTime = 0.f;
	enemy.MaxDeathTime = 1.f;

	enemy.HaveTimedLife = false;

	enemy.MaxHealthBodyRadius = 40;
	enemy.MinHealthBodyRadius = 10;
	enemy.MinBodyRadius = 0;

	enemy.m_Body.WorldCircle.Center = patrolPoints.at(0);
	enemy.m_Body.WorldCircle.Radius = enemy.MaxHealthBodyRadius;
	enemy.m_Body.ScreenCircle = enemy.m_Body.WorldCircle;

	enemy.SenseBound = false;
	//enemy.Sense;

	enemy.Wounded = false;
	enemy.MaxWoundedTime = 1.f;
	enemy.CurrentWoundedTime = 0.f;

	enemy.StartingPos = patrolPoints.at(0);
	
	enemy.PatrolPositions = patrolPoints;
	enemy.CurrentIndexToPatrol = 0;
	enemy.MaxTimeToWaitAtEachPosition = 1.f;
	enemy.CurrentTimeWaited = 0;

	m_Bullets.push_back(enemy);
}

void Game::CreateFollowingEnemy(int maxLife, const FVector2f& position)
{
	FBullet enemy{};

	enemy.BulletNormalColor = FColor4i{ 255,10,10, 255 };

	enemy.IsSpawned = true;


	enemy.StartingPos = position;

	enemy.BulletBehaviour = EBulletBehaviour::FollowPlayer;
	enemy.AlwaysLookTowardsPlayer = true;

	//enemy.FollowingSpeed = 5000;

	enemy.CollideWithLevel = true;

	enemy.CurrentLife = maxLife;
	enemy.MaxLife = maxLife;
	enemy.CurrentDeathTime = 0.f;
	enemy.MaxDeathTime = 1.f;

	enemy.HaveTimedLife = false;

	enemy.MaxHealthBodyRadius = 40;
	enemy.MinHealthBodyRadius = 10;
	enemy.MinBodyRadius = 0;

	enemy.m_Body.WorldCircle.Center = position;
	enemy.m_Body.WorldCircle.Radius = enemy.MaxHealthBodyRadius;
	enemy.m_Body.ScreenCircle = enemy.m_Body.WorldCircle;

	enemy.SenseBound = true;//set sense into the center of the body
	enemy.Sense.WorldCircle.Center = position;
	enemy.Sense.WorldCircle.Radius = 500;
	enemy.Sense.ScreenCircle = enemy.Sense.WorldCircle;

	enemy.Wounded = false;
	enemy.MaxWoundedTime = 1.f;
	enemy.CurrentWoundedTime = 0.f;

	m_Bullets.push_back(enemy);
	
}

FBullet Game::CreatePerishableBullet(int maxLife)
{
	FBullet bullet{};


	//bullet.StartingPos = position;

	bullet.BulletNormalColor = FColor4i{ 30,70,255, 255 };
	bullet.IsSpawned = true;

	bullet.BulletBehaviour = EBulletBehaviour::MoveStraightInCustomMovement;
	bullet.AlwaysLookTowardsPlayer = false;

	bullet.FollowingSpeed = 50;

	bullet.CollideWithLevel = true;

	bullet.CurrentLife = 1;
	bullet.MaxLife = maxLife;
	bullet.CurrentDeathTime = 0.f;
	bullet.MaxDeathTime = 1.f;

	bullet.HaveTimedLife = true;
	bullet.MaxLifeTime = 5.f;
	bullet.CurrentLifeTime = 0.f;

	bullet.MaxHealthBodyRadius = 30;
	bullet.MinHealthBodyRadius = 10;
	bullet.MinBodyRadius = 0;

	bullet.m_Body.WorldCircle.Radius = bullet.MaxHealthBodyRadius;
	bullet.m_Body.ScreenCircle = bullet.m_Body.WorldCircle;

	bullet.SenseBound = false;

	bullet.Wounded = false;
	bullet.MaxWoundedTime = 1.f;
	bullet.CurrentWoundedTime = 0.f;

	return bullet;
}

FBullet Game::CreateUniDirectionalBullet(int maxLife)
{
	FBullet bullet{};

	bullet.BulletNormalColor = FColor4i{ 30,0,255, 255 };
	bullet.IsSpawned = true;

	bullet.BulletBehaviour = EBulletBehaviour::MoveStraightInCustomMovement;
	bullet.AlwaysLookTowardsPlayer = false;
	//bullet.CustomMovementDirection = directionToLookAt;

	//enemy.DistanceToKeepFromPlayer;
	//bullet.FollowingSpeed = 2000;

	//bullet.StartingPos = position;

	bullet.CollideWithLevel = true;

	bullet.CurrentLife = maxLife;
	bullet.MaxLife = maxLife;
	bullet.CurrentDeathTime = 0.f;
	bullet.MaxDeathTime = 1.f;

	bullet.HaveTimedLife = false;
	//enemy.MaxLifeTime{ 5.f };
	//enemy.CurrentLifeTime{ 0.f };

	bullet.MaxHealthBodyRadius = 20;
	bullet.MinHealthBodyRadius = 10;
	bullet.MinBodyRadius = 0;

	//bullet.m_Body.WorldCircle.Center;
	bullet.m_Body.WorldCircle.Radius = bullet.MaxHealthBodyRadius;
	bullet.m_Body.ScreenCircle = bullet.m_Body.WorldCircle;
	//enemy.m_BodyOverlapInfo;
	//enemy.m_Velocity;

	bullet.SenseBound = false;
	//enemy.Sense;

	bullet.Wounded = false;
	bullet.MaxWoundedTime = 1.f;
	bullet.CurrentWoundedTime = 0.f;

	return bullet;
}

FBullet Game::CreateFollowingBullet(int maxLife)
{
	FBullet bullet{};

	bullet.BulletNormalColor = FColor4i{ 70,30,255, 255 };

	//bullet.StartingPos = position;

	bullet.IsSpawned = true;
	bullet.IsStatic = false;
	bullet.AlwaysWounded = false;

	bullet.BulletBehaviour = EBulletBehaviour::FollowPlayer;

	bullet.AlwaysLookTowardsPlayer = false;
	//bullet.FollowingSpeed = 5000;

	bullet.CollideWithLevel = true;

	bullet.CurrentLife = maxLife;
	bullet.MaxLife = maxLife;
	bullet.CurrentDeathTime = 0.f;
	bullet.MaxDeathTime = 1.f;

	bullet.HaveTimedLife = false;

	bullet.MaxHealthBodyRadius = 30;
	bullet.MinHealthBodyRadius = 10;
	bullet.MinBodyRadius = 0;

	bullet.m_Body.WorldCircle.Radius = bullet.MaxHealthBodyRadius;
	bullet.m_Body.ScreenCircle = bullet.m_Body.WorldCircle;

	bullet.SenseBound = true;
	bullet.Sense.WorldCircle.Radius = 500;
	bullet.Sense.ScreenCircle = bullet.Sense.WorldCircle;

	bullet.Wounded = false;
	bullet.MaxWoundedTime = 1.f;
	bullet.CurrentWoundedTime = 0.f;

	return bullet;
}

FBullet Game::CreateStaticBullet(int maxLife, const FVector2f& position)
{
	FBullet bullet{};


	bullet.StartingPos = position;
	bullet.BulletNormalColor = FColor4i{ 0,0,255, 255 };

	bullet.IsSpawned = true;
	bullet.IsStatic = true;
	bullet.AlwaysWounded = true;

	bullet.AlwaysLookTowardsPlayer = false;

	bullet.CollideWithLevel = false;

	bullet.CurrentLife = maxLife;
	bullet.MaxLife = maxLife;
	bullet.CurrentDeathTime = 0.f;
	bullet.MaxDeathTime = 1.f;

	bullet.HaveTimedLife = false;

	bullet.MaxHealthBodyRadius = 20;
	bullet.MinHealthBodyRadius = 10;
	bullet.MinBodyRadius = 0;

	bullet.m_Body.WorldCircle.Center = position;
	bullet.m_Body.WorldCircle.Radius = bullet.MaxHealthBodyRadius;
	bullet.m_Body.ScreenCircle = bullet.m_Body.WorldCircle;

	bullet.SenseBound = false;

	bullet.Wounded = false;
	bullet.MaxWoundedTime = 1.f;
	bullet.CurrentWoundedTime = 0.f;

	return bullet;
}

FBullet Game::CreateMovableBullet(int maxLife, const FVector2f& position)
{
	FBullet bullet{};


	bullet.StartingPos = position;
	bullet.BulletNormalColor = FColor4i{ 0,255,255, 255 };

	bullet.IsSpawned = true;
	bullet.IsStatic = false;
	bullet.AlwaysWounded = true;

	bullet.AlwaysLookTowardsPlayer = false;
	//bullet.FollowingSpeed = 500;

	bullet.CollideWithLevel = true;

	bullet.CurrentLife = maxLife;
	bullet.MaxLife = maxLife;
	bullet.CurrentDeathTime = 0.f;
	bullet.MaxDeathTime = 1.f;

	bullet.HaveTimedLife = false;

	bullet.MaxHealthBodyRadius = 20;
	bullet.MinHealthBodyRadius = 10;
	bullet.MinBodyRadius = 0;

	bullet.m_Body.WorldCircle.Center = position;
	bullet.m_Body.WorldCircle.Radius = bullet.MaxHealthBodyRadius;
	bullet.m_Body.ScreenCircle = bullet.m_Body.WorldCircle;

	bullet.SenseBound = false;

	bullet.Wounded = false;
	bullet.MaxWoundedTime = 1.f;
	bullet.CurrentWoundedTime = 0.f;

	return bullet;
}
