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
	//m_RectTest.WorldRect.Height = 100;
	//m_RectTest.WorldRect.Width = 1000;
	//m_RectTest.WorldRect.Left = -100;
	//m_RectTest.WorldRect.Top = -100;
	//m_RectTest.ScreenRect = m_RectTest.WorldRect;

	//m_Bullets.resize(m_NumOfBullets);
	//
	//
	//
	//for (size_t i = 0; i < m_Bullets.size(); i++)
	//{
	//	m_Bullets.at(i).m_Body.WorldCircle.Radius = m_Bullets.at(i).MaxHealthBodyRadius;
	//	m_Bullets.at(i).m_Body.WorldCircle.Center = 
	//		FVector2f{ 
	//		(float)STools::GetRandomBetweenRange(0,GetViewPort().width),
	//		(float)STools::GetRandomBetweenRange(0,GetViewPort().height) };
	//	m_Bullets.at(i).m_Body.ScreenCircle = m_Bullets.at(i).m_Body.WorldCircle;
	//}
	
	m_Bounds.WorldCircle.Radius = 1000;
	m_Bounds.ScreenCircle.Radius = m_Bounds.WorldCircle.Radius;

	CreatePatrolEnemy(3, true, FVector2f{0,0}, FVector2f{100,0 }, 1, CreateFollowingBullet(3));

	m_Player.HealthBar().Size = FVector2f{ 200,50 };
	m_Player.HealthBar().Position = FVector2f{ 30, GetViewPort().height - m_Player.HealthBar().Size.Y - 10};
	

	m_Camera.Position = m_Player.Body().WorldCircle.Center;
}

void Game::Cleanup( )
{
}

void Game::Update( float elapsedSec )
{
	// Check keyboard state
	


	FVector2f followingOffset{};

	for (size_t i = 0; i < m_Bullets.size(); i++)
	{
		if (m_Bullets.at(i).AlwaysLookTowardsPlayer && 
			m_Bullets.at(i).BulletRank == EBulletRank::Commander)
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

		if (m_Bullets.at(i).CurrentLife <= 0 && !m_Bullets.at(i).AlwaysWounded)
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

				if (m_Bullets.at(i).BulletRank == EBulletRank::Soldier && 
					m_Bullets.at(m_Bullets.at(i).IndexOfCommander).IsSpawned)
				{
					m_Bullets.at(i).IsSpawned = true;
					m_Bullets.at(i).m_Body.WorldCircle.Center = m_Bullets.at(m_Bullets.at(i).IndexOfCommander).m_Body.WorldCircle.Center;
				}

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

					followingOffset =
						SVectors::Scale(SVectors::NormalizeVector(followingOffset),
							m_Bullets.at(i).FollowingSpeed);

					m_Bullets.at(i).m_Velocity.X = followingOffset.X * elapsedSec;
					m_Bullets.at(i).m_Velocity.Y = followingOffset.Y * elapsedSec;
				}
				
			}
			else if (m_Bullets.at(i).BulletBehaviour == EBulletBehaviour::MoveStraightInCustomMovement)
			{
				m_Bullets.at(i).m_Velocity.X = m_Bullets.at(i).CustomMovementDirection.X * elapsedSec;
				m_Bullets.at(i).m_Velocity.Y = m_Bullets.at(i).CustomMovementDirection.Y * elapsedSec;
			}
			else if (m_Bullets.at(i).BulletBehaviour == EBulletBehaviour::Patrol)
			{
				FVector2f currentPatrolPoint{
					m_Bullets.at(i).PatrolPositions.at(m_Bullets.at(i).CurrentIndexToPatrol) };

				followingOffset =
					SVectors::Scale(SVectors::NormalizeVector(
						SVectors::Subtract(currentPatrolPoint, m_Bullets.at(i).m_Body.WorldCircle.Center)),
						m_Bullets.at(i).FollowingSpeed);

				m_Bullets.at(i).m_Velocity.X = followingOffset.X * elapsedSec;
				m_Bullets.at(i).m_Velocity.Y = followingOffset.Y * elapsedSec;

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
		

		m_Bullets.at(i).m_Body.WorldCircle.Center.X += m_Bullets.at(i).m_Velocity.X;
		m_Bullets.at(i).m_Body.WorldCircle.Center.Y += m_Bullets.at(i).m_Velocity.Y;
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
					m_Bullets.at(i).m_Body.WorldCircle.Center.X += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.X;
					m_Bullets.at(i).m_Body.WorldCircle.Center.Y += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.Y;

					hitForce = SVectors::Scale(
						SVectors::NormalizeVector(SVectors::Subtract(m_CurrentMouseScreenPosition, m_Player.Body().ScreenCircle.Center)),
						m_Player.GetShieldForce() * elapsedSec);

					m_Bullets.at(i).m_Velocity.X = hitForce.X;
					m_Bullets.at(i).m_Velocity.Y = hitForce.Y;

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
					m_Player.GetShieldForce() * elapsedSec);

				m_Bullets.at(i).m_Velocity.X += hitForce.X;
				m_Bullets.at(i).m_Velocity.Y += hitForce.Y;
			}
			
		}

		if (m_Bullets.at(i).IsSpawned)
		{
			for (size_t j = i + 1; j < m_Bullets.size(); j++)
			{
				if (m_Bullets.at(i).Wounded || m_Bullets.at(j).Wounded)
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
								m_Player.GetShieldForce() * elapsedSec);

							m_Bullets.at(i).m_Velocity.X += hitForce.X;
							m_Bullets.at(i).m_Velocity.Y += hitForce.Y;
						}

						

						if (!m_Bullets.at(i).Wounded && !m_Bullets.at(i).AlwaysWounded)
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
								m_Player.GetShieldForce() * elapsedSec);

							m_Bullets.at(j).m_Velocity.X += hitForce.X;
							m_Bullets.at(j).m_Velocity.Y += hitForce.Y;
						}
						

						if (!m_Bullets.at(j).Wounded && !m_Bullets.at(j).AlwaysWounded)
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

			if (m_Bullets.at(i).CollideWithLevel && !m_Bullets.at(i).IsStatic)
			{
				for (size_t i = 0; i < m_CurrentLevel.size(); i++)
				{
					if (SCollision::IsOverlapping(m_Bullets.at(i).m_Body.WorldCircle, m_CurrentLevel.at(i).WorldRect, m_Bullets.at(i).m_BodyOverlapInfo, false))
					{
						m_Bullets.at(i).m_Body.WorldCircle.Center.X += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.X;
						m_Bullets.at(i).m_Body.WorldCircle.Center.Y += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.Y;

						m_Bullets.at(i).m_Velocity.X += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.X;
						m_Bullets.at(i).m_Velocity.Y += m_Bullets.at(i).m_BodyOverlapInfo.TranslationVector.Y;
					}
				}
			}
			

		}
		
	}
	
	m_Player.SolveCollisionWithBounds(m_Bounds.WorldCircle, elapsedSec);

	for (size_t i = 0; i < m_CurrentLevel.size(); i++)
	{
		m_Player.SolveCollisionWithRect(m_CurrentLevel.at(i).WorldRect);
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

	m_Bounds.ScreenCircle.Center.X = m_Bounds.WorldCircle.Center.X + cameraDelta.X;
	m_Bounds.ScreenCircle.Center.Y = m_Bounds.WorldCircle.Center.Y + cameraDelta.Y;

	for (size_t i = 0; i < m_CurrentLevel.size(); i++)
	{
		m_CurrentLevel.at(i).ScreenRect.Left = m_CurrentLevel.at(i).WorldRect.Left + cameraDelta.X;
		m_CurrentLevel.at(i).ScreenRect.Top =  m_CurrentLevel.at(i).WorldRect.Top + cameraDelta.Y;
	}

	for (size_t i = 0; i < m_Bullets.size(); i++)
	{
		if (m_Bullets.at(i).IsSpawned)
		{
			m_Bullets.at(i).m_Body.ScreenCircle.Center.X = m_Bullets.at(i).m_Body.WorldCircle.Center.X + cameraDelta.X;
			m_Bullets.at(i).m_Body.ScreenCircle.Center.Y = m_Bullets.at(i).m_Body.WorldCircle.Center.Y + cameraDelta.Y;

		}
	}

}

void Game::Draw( ) const
{
	ClearBackground( );

	m_Bounds.ScreenCircle.Draw(FColor4i{0,0,255,255}, false, 10.f);
	m_Player.DrawLife();

	for (size_t i = 0; i < m_Bullets.size(); i++)
	{
		if (m_Bullets.at(i).IsSpawned)
		{
			if (m_Bullets.at(i).Wounded || m_Bullets.at(i).AlwaysWounded)
			{
				m_Bullets.at(i).m_Body.ScreenCircle.Draw(m_Bullets.at(i).BulletParriedColor, true);
			}
			else
			{
				m_Bullets.at(i).m_Body.ScreenCircle.Draw(m_Bullets.at(i).BulletNormalColor, true);
			}
		}
		
	}


	for (size_t i = 0; i < m_CurrentLevel.size(); i++)
	{
		m_CurrentLevel.at(i).ScreenRect.Draw(FColor4i{ 255,255,255,255 }, true, false);
	}

	m_Player.Draw();
	
	
	//utils::DrawLine(
	//	m_Player.Body.ScreenCircle.Center.X,
	//	m_Player.Body.ScreenCircle.Center.Y,
	//	m_Player.Body.ScreenCircle.Center.X + m_Player.Velocity.X,
	//	m_Player.Body.ScreenCircle.Center.Y + m_Player.Velocity.Y,
	//	5);
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	//switch ( e.keysym.sym )
	//{
	//case SDLK_LEFT:
	//	//std::cout << "Left arrow key released\n";
	//	break;
	//case SDLK_RIGHT:
	//	//std::cout << "`Right arrow key released\n";
	//	break;
	//case SDLK_1:
	//case SDLK_KP_1:
	//	//std::cout << "Key 1 released\n";
	//	break;
	//}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;

	m_CurrentMouseScreenPosition.X = e.x;
	m_CurrentMouseScreenPosition.Y = e.y;
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
		//m_Player.GetDashDirection(e.x, e.y);
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

	if (e.button == SDL_BUTTON_LEFT)
	{
		m_Player.DeactivateShield();
	}
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.3f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::CreatePatrolEnemy(int maxLife, bool alwaysLookTowardsPlayer, FVector2f beginPatrol, FVector2f endPatrol, int bulletNumber, const FBullet& bulletType)
{

	FBullet enemy{};

	enemy.BulletNormalColor = FColor4i{255,10,10, 255};

	enemy.IsSpawned = true;

	enemy.BulletRank = EBulletRank::Commander;
	enemy.IndexOfCommander = m_Bullets.size(); // own commander

	enemy.BulletBehaviour = EBulletBehaviour::Patrol;
	enemy.AlwaysLookTowardsPlayer = alwaysLookTowardsPlayer;
	enemy.CustomMovementDirection = SVectors::OrthogonalVector(SVectors::Subtract(endPatrol, beginPatrol));

	//enemy.DistanceToKeepFromPlayer;
	enemy.FollowingSpeed = 200 ;

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
	FBullet currentBullet{ bulletType };
	m_Bullets.reserve(m_Bullets.size() + bulletNumber);

	for (int i = 0; i < bulletNumber; i++)
	{
		currentBullet.BulletRank;
		currentBullet.IndexOfCommander = enemy.IndexOfCommander;
		currentBullet.CustomMovementDirection = enemy.CustomMovementDirection;
		currentBullet.m_Body.WorldCircle.Center = enemy.m_Body.WorldCircle.Center;
		currentBullet.m_Body.ScreenCircle.Center = enemy.m_Body.ScreenCircle.Center;
		m_Bullets.push_back(currentBullet);
	}
}

FBullet Game::CreateUniDirectionalBullet(int maxLife)
{
	FBullet bullet{};

	bullet.BulletNormalColor = FColor4i{ 30,0,255, 255 };
	bullet.IsSpawned = true;

	bullet.BulletRank = EBulletRank::Soldier;
	//bullet.IndexOfCommander = commanderIndex;

	bullet.BulletBehaviour = EBulletBehaviour::MoveStraightInCustomMovement;
	bullet.AlwaysLookTowardsPlayer = false;
	//bullet.CustomMovementDirection = directionToLookAt;

	//enemy.DistanceToKeepFromPlayer;
	bullet.FollowingSpeed = 500;

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

	//bullet.PatrolPositions.reserve(2);
	//bullet.PatrolPositions.push_back(beginPatrol);
	//bullet.PatrolPositions.push_back(endPatrol);
	//bullet.CurrentIndexToPatrol = 0;
	//bullet.MaxTimeToWaitAtEachPosition = 1.f;
	//bullet.CurrentTimeWaited = 0;

	return bullet;
}

FBullet Game::CreateFollowingBullet(int maxLife)
{
	FBullet bullet{};

	bullet.BulletNormalColor = FColor4i{ 70,30,255, 255 };

	bullet.IsSpawned = true;
	bullet.IsStatic = false;
	bullet.AlwaysWounded = false;

	bullet.BulletRank = EBulletRank::Soldier;

	bullet.BulletBehaviour = EBulletBehaviour::FollowPlayer;

	bullet.AlwaysLookTowardsPlayer = false;
	bullet.FollowingSpeed = 500;

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

	bullet.SenseBound = false;

	bullet.Wounded = false;
	bullet.MaxWoundedTime = 1.f;
	bullet.CurrentWoundedTime = 0.f;

	return bullet;
}

FBullet Game::CreateStaticBullet(int maxLife, const FVector2f& position, bool alwaysWounded)
{
	FBullet bullet{};


	bullet.BulletNormalColor = FColor4i{ 0,0,255, 255 };

	bullet.IsSpawned = true;
	bullet.IsStatic = true;
	bullet.AlwaysWounded = alwaysWounded;

	bullet.BulletRank = EBulletRank::Soldier;

	bullet.AlwaysLookTowardsPlayer = false;
	bullet.FollowingSpeed = 500;

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


	bullet.BulletNormalColor = FColor4i{ 0,255,255, 255 };

	bullet.IsSpawned = true;
	bullet.IsStatic = false;
	bullet.AlwaysWounded = true;

	bullet.BulletRank = EBulletRank::Soldier;

	bullet.AlwaysLookTowardsPlayer = false;
	bullet.FollowingSpeed = 500;

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
