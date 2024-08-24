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
	m_Enemies.resize(m_NumOfEnemies);

	m_Bounds.WorldCircle.Radius = 1000;
	m_Bounds.ScreenCircle.Radius = m_Bounds.WorldCircle.Radius;

	for (size_t i = 0; i < m_Enemies.size(); i++)
	{
		m_Enemies.at(i).Body.WorldCircle.Radius = 50;
		m_Enemies.at(i).Body.WorldCircle.Center = 
			FVector2f{ 
			(float)STools::GetRandomBetweenRange(0,GetViewPort().width),
			(float)STools::GetRandomBetweenRange(0,GetViewPort().height) };
		m_Enemies.at(i).Body.ScreenCircle = m_Enemies.at(i).Body.WorldCircle;
	}

	m_Player.Body.WorldCircle.Radius = 50;
	m_Player.Body.WorldCircle.Center = FVector2f{ 400,100 };
	m_Player.Body.ScreenCircle = m_Player.Body.WorldCircle;

	m_Player.Shield.WorldCircle.Radius = 50;
	m_Player.Shield.ScreenCircle = m_Player.Shield.WorldCircle;

	m_Camera.Position = m_Player.Body.WorldCircle.Center;
}

void Game::Cleanup( )
{
}

void Game::Update( float elapsedSec )
{
	// Check keyboard state
	const Uint8 *pStates = SDL_GetKeyboardState( nullptr );
	if (!m_Player.Wounded)
	{
		m_Player.Velocity = FVector2f{ 0,0 };

		if (pStates[SDL_SCANCODE_D])
		{
			m_Player.Velocity.X += m_Player.MovementSpeed * elapsedSec;
		}
		if (pStates[SDL_SCANCODE_A])
		{
			m_Player.Velocity.X -= m_Player.MovementSpeed * elapsedSec;
		}
		if (pStates[SDL_SCANCODE_W])
		{
			m_Player.Velocity.Y += m_Player.MovementSpeed * elapsedSec;
		}
		if (pStates[SDL_SCANCODE_S])
		{
			m_Player.Velocity.Y -= m_Player.MovementSpeed * elapsedSec;
		}
	}

	FVector2f followingOffset{};

	for (size_t i = 0; i < m_Enemies.size(); i++)
	{
		if (!m_Enemies.at(i).Wounded)
		{
			followingOffset =
				SVectors::Scale(SVectors::NormalizeVector(
					SVectors::Subtract(m_Player.Body.WorldCircle.Center, m_Enemies.at(i).Body.WorldCircle.Center)),
					m_Enemies.at(i).FollowingSpeed);

			m_Enemies.at(i).Velocity.X = followingOffset.X * elapsedSec;
			m_Enemies.at(i).Velocity.Y = followingOffset.Y * elapsedSec;
		}
		else
		{
			m_Enemies.at(i).CurrentWoundedTime += elapsedSec;

			if (m_Enemies.at(i).CurrentWoundedTime >= m_Enemies.at(i).MaxWoundedTime)
			{
				m_Enemies.at(i).CurrentWoundedTime = 0;
				m_Enemies.at(i).Wounded = false;
			}
		}
		

		m_Enemies.at(i).Body.WorldCircle.Center.X += m_Enemies.at(i).Velocity.X;
		m_Enemies.at(i).Body.WorldCircle.Center.Y += m_Enemies.at(i).Velocity.Y;
	}


	m_Player.Velocity.X += m_Player.CurrentShotForce.X * elapsedSec;
	m_Player.Velocity.Y += m_Player.CurrentShotForce.Y * elapsedSec;

	m_Player.CurrentShotForce.X = 0;
	m_Player.CurrentShotForce.Y = 0;

	FVector2f velocity{ m_Player.Velocity.X, m_Player.Velocity.Y };

	if (abs(velocity.X) <= m_Player.MaxPlayerVelocityPerSecond)
	{
		m_Player.Body.WorldCircle.Center.X += velocity.X * elapsedSec;
	}
	else
	{
		if (velocity.X > 0)
		{
			m_Player.Body.WorldCircle.Center.X += m_Player.MaxPlayerVelocityPerSecond * elapsedSec;
		}
		else
		{
			m_Player.Body.WorldCircle.Center.X += -m_Player.MaxPlayerVelocityPerSecond * elapsedSec;
		}
	}
	
	if (abs(velocity.Y) <= m_Player.MaxPlayerVelocityPerSecond)
	{
		m_Player.Body.WorldCircle.Center.Y += velocity.Y * elapsedSec;
	}
	else
	{
		if (velocity.Y > 0)
		{
			m_Player.Body.WorldCircle.Center.Y += m_Player.MaxPlayerVelocityPerSecond * elapsedSec;
		}
		else
		{
			m_Player.Body.WorldCircle.Center.Y += -m_Player.MaxPlayerVelocityPerSecond * elapsedSec;
		}
	}

	FVector2f hitForce{};

	if (m_Player.Wounded)
	{
		m_Player.CurrentWoundedTime += elapsedSec;

		if (m_Player.CurrentWoundedTime >= m_Player.MaxWoundedTime)
		{
			m_Player.CurrentWoundedTime = 0.f;
			m_Player.Wounded = false;
		}
	}

	for (size_t i = 0; i < m_Enemies.size(); i++)
	{

		if (SCollision::IsOverlapping(
			m_Enemies.at(i).Body.WorldCircle, m_Player.Shield.WorldCircle,
			m_Enemies.at(i).BodyOverlapInfo, m_Player.ShieldOverlapInfo))
		{
			m_Enemies.at(i).Body.WorldCircle.Center.X += m_Enemies.at(i).BodyOverlapInfo.TranslationVector.X;
			m_Enemies.at(i).Body.WorldCircle.Center.Y += m_Enemies.at(i).BodyOverlapInfo.TranslationVector.Y;

			hitForce = SVectors::Scale(
				SVectors::NormalizeVector(m_Enemies.at(i).BodyOverlapInfo.TranslationVector),
				m_Player.CollisionImpactForceMagnitude * elapsedSec);

			m_Enemies.at(i).Velocity.X += hitForce.X;
			m_Enemies.at(i).Velocity.Y += hitForce.Y;

			m_Enemies.at(i).Wounded = true;
			m_Enemies.at(i).CurrentWoundedTime = 0;
		}

		if (SCollision::IsOverlapping(
			m_Enemies.at(i).Body.WorldCircle, m_Player.Body.WorldCircle,
			m_Enemies.at(i).BodyOverlapInfo, m_Player.BodyOverlapInfo))
		{
			//Solve for player
			m_Player.Body.WorldCircle.Center.X += m_Player.BodyOverlapInfo.TranslationVector.X;
			m_Player.Body.WorldCircle.Center.Y += m_Player.BodyOverlapInfo.TranslationVector.Y;

			hitForce = SVectors::Scale(
				SVectors::NormalizeVector(m_Player.BodyOverlapInfo.TranslationVector),
				m_Player.CollisionImpactForceMagnitude * elapsedSec);

			m_Player.Velocity.X += hitForce.X;
			m_Player.Velocity.Y += hitForce.Y;

			if (!m_Player.Wounded)
			{
				m_Player.Wounded = true;
				m_Player.CurrentWoundedTime = 0.f;
			}

			//Solve for enemy
			m_Enemies.at(i).Body.WorldCircle.Center.X += m_Enemies.at(i).BodyOverlapInfo.TranslationVector.X;
			m_Enemies.at(i).Body.WorldCircle.Center.Y += m_Enemies.at(i).BodyOverlapInfo.TranslationVector.Y;

			hitForce = SVectors::Scale(
				SVectors::NormalizeVector(m_Enemies.at(i).BodyOverlapInfo.TranslationVector), 
				m_Player.CollisionImpactForceMagnitude * elapsedSec);

			m_Enemies.at(i).Velocity.X += hitForce.X;
			m_Enemies.at(i).Velocity.Y += hitForce.Y;
		}

		for (size_t j = i + 1; j < m_Enemies.size(); j++)
		{
			if (m_Enemies.at(i).Wounded || m_Enemies.at(j).Wounded)
			{
				if (SCollision::IsOverlapping(
					m_Enemies.at(i).Body.WorldCircle, m_Enemies.at(j).Body.WorldCircle,
					m_Enemies.at(i).BodyOverlapInfo, m_Enemies.at(j).BodyOverlapInfo))
				{

					//Solve for enemy one
					m_Enemies.at(i).Body.WorldCircle.Center.X += m_Enemies.at(i).BodyOverlapInfo.TranslationVector.X;
					m_Enemies.at(i).Body.WorldCircle.Center.Y += m_Enemies.at(i).BodyOverlapInfo.TranslationVector.Y;

					hitForce = SVectors::Scale(
						SVectors::NormalizeVector(m_Enemies.at(i).BodyOverlapInfo.TranslationVector),
						m_Player.CollisionImpactForceMagnitude * elapsedSec);

					m_Enemies.at(i).Velocity.X += hitForce.X;
					m_Enemies.at(i).Velocity.Y += hitForce.Y;

					if (!m_Enemies.at(i).Wounded)
					{
						m_Enemies.at(i).Wounded = true;
						m_Enemies.at(i).CurrentWoundedTime = 0;
					}

					//Solve for enemy two
					m_Enemies.at(j).Body.WorldCircle.Center.X += m_Enemies.at(j).BodyOverlapInfo.TranslationVector.X;
					m_Enemies.at(j).Body.WorldCircle.Center.Y += m_Enemies.at(j).BodyOverlapInfo.TranslationVector.Y;

					hitForce = SVectors::Scale(
						SVectors::NormalizeVector(m_Enemies.at(j).BodyOverlapInfo.TranslationVector),
						m_Player.CollisionImpactForceMagnitude * elapsedSec);

					m_Enemies.at(j).Velocity.X += hitForce.X;
					m_Enemies.at(j).Velocity.Y += hitForce.Y;

					if (!m_Enemies.at(j).Wounded)
					{
						m_Enemies.at(j).Wounded = true;
						m_Enemies.at(j).CurrentWoundedTime = 0;
					}
				}
			}
		}
	}
	

	// --- CAMERA LOGIC --- 
	
	FVector2f cameraDirection{ 
			SVectors::Subtract(m_Player.Body.WorldCircle.Center, m_Camera.Position) };
	m_Camera.Position.X += cameraDirection.X * m_Camera.FollowingSpeed * elapsedSec;
	m_Camera.Position.Y += cameraDirection.Y * m_Camera.FollowingSpeed * elapsedSec;

	//m_Camera.Position = m_Player.Body.WorldCircle.Center;

	// --- OFFSET

	FVector2f cameraDelta{
	(GetViewPort().width / 2.f) - m_Camera.Position.X,
	(GetViewPort().height / 2.f) - m_Camera.Position.Y
	};

	//Player

	m_Player.Body.ScreenCircle.Center.X = m_Player.Body.WorldCircle.Center.X + cameraDelta.X;
	m_Player.Body.ScreenCircle.Center.Y = m_Player.Body.WorldCircle.Center.Y + cameraDelta.Y;

	m_Player.Shield.WorldCircle.Center =
		SVectors::Add(SVectors::Scale(
			SVectors::NormalizeVector(
				SVectors::Subtract(m_CurrentMouseScreenPosition, m_Player.Body.ScreenCircle.Center)),
			m_Player.ShieldDistanceFromBodyCenter), m_Player.Body.WorldCircle.Center);
	
	m_Player.Shield.ScreenCircle.Center.X = m_Player.Shield.WorldCircle.Center.X + cameraDelta.X;
	m_Player.Shield.ScreenCircle.Center.Y = m_Player.Shield.WorldCircle.Center.Y + cameraDelta.Y;

	m_Bounds.ScreenCircle.Center.X = m_Bounds.WorldCircle.Center.X + cameraDelta.X;
	m_Bounds.ScreenCircle.Center.Y = m_Bounds.WorldCircle.Center.Y + cameraDelta.Y;

	for (size_t i = 0; i < m_Enemies.size(); i++)
	{
		m_Enemies.at(i).Body.ScreenCircle.Center.X = m_Enemies.at(i).Body.WorldCircle.Center.X + cameraDelta.X;
		m_Enemies.at(i).Body.ScreenCircle.Center.Y = m_Enemies.at(i).Body.WorldCircle.Center.Y + cameraDelta.Y;
	}

}

void Game::Draw( ) const
{
	ClearBackground( );

	m_Bounds.ScreenCircle.Draw(FColor4i{0,0,255,255}, true);

	for (size_t i = 0; i < m_Enemies.size(); i++)
	{
		if (m_Enemies.at(i).Wounded)
		{
			m_Enemies.at(i).Body.ScreenCircle.Draw(m_EnemyWoundedColor, true);
		}
		else
		{
			m_Enemies.at(i).Body.ScreenCircle.Draw(m_EnemyNormalColor, true);
		}
		
	}

	if (m_Player.Wounded)
	{
		m_Player.Body.ScreenCircle.Draw(m_PlayerWoundedColor, true);
	}
	else
	{
		m_Player.Body.ScreenCircle.Draw(m_PlayerNormalColor, true);
	}

	m_Player.Shield.ScreenCircle.Draw(FColor4i{255,255,255,255}, true);
	
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

	FVector2f normalizedDirectionFromMousePosToPlayer{
		SVectors::NormalizeVector(
					SVectors::Subtract(m_Player.Body.ScreenCircle.Center, FVector2f{ float(e.x), float(e.y)})) };

	if (!m_Player.Wounded)
	{
		m_Player.CurrentShotForce =
			SVectors::Scale(normalizedDirectionFromMousePosToPlayer, m_Player.ShotForceMagnitude);
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
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.3f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}
