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

	m_Camera.Position = m_Player.Body.WorldCircle.Center;
}

void Game::Cleanup( )
{
}

void Game::Update( float elapsedSec )
{
	// Check keyboard state
	//const Uint8 *pStates = SDL_GetKeyboardState( nullptr );
	//if ( pStates[SDL_SCANCODE_RIGHT] )
	//{
	//	std::cout << "Right arrow key is down\n";
	//}
	//if ( pStates[SDL_SCANCODE_LEFT] && pStates[SDL_SCANCODE_UP])
	//{
	//	std::cout << "Left and up arrow keys are down\n";
	//}

	FVector2f followingOffset{};

	for (size_t i = 0; i < m_Enemies.size(); i++)
	{
		if (!m_Enemies.at(i).Wounded)
		{
			followingOffset =
				SVectors::Scale(SVectors::NormalizeVector(
					SVectors::Subtract(m_Player.Body.WorldCircle.Center, m_Enemies.at(i).Body.WorldCircle.Center)),
					m_Enemies.at(i).FollowingSpeed * elapsedSec);

			m_Enemies.at(i).Velocity.X += followingOffset.X;
			m_Enemies.at(i).Velocity.Y += followingOffset.Y;
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

	m_Player.Body.WorldCircle.Center.X += m_Player.Velocity.X * elapsedSec;
	m_Player.Body.WorldCircle.Center.Y += m_Player.Velocity.Y * elapsedSec;

	m_Player.CurrentShotForce.X = 0;
	m_Player.CurrentShotForce.Y = 0;

	FVector2f hitForce{};

	for (size_t i = 0; i < m_Enemies.size(); i++)
	{
		if (SCollision::IsOverlapping(
			m_Enemies.at(i).Body.WorldCircle, m_Player.Body.WorldCircle,
			m_Enemies.at(i).OverlapInfo, m_Player.OverlapInfo))
		{
			//Solve for player
			m_Player.Body.WorldCircle.Center.X += m_Player.OverlapInfo.TranslationVector.X;
			m_Player.Body.WorldCircle.Center.Y += m_Player.OverlapInfo.TranslationVector.Y;

			m_Player.Velocity.X += m_Player.OverlapInfo.TranslationVector.X;
			m_Player.Velocity.Y += m_Player.OverlapInfo.TranslationVector.Y;

			//Solve for enemy
			m_Enemies.at(i).Body.WorldCircle.Center.X += m_Enemies.at(i).OverlapInfo.TranslationVector.X;
			m_Enemies.at(i).Body.WorldCircle.Center.Y += m_Enemies.at(i).OverlapInfo.TranslationVector.Y;

			hitForce = SVectors::Scale(
				SVectors::NormalizeVector(m_Enemies.at(i).OverlapInfo.TranslationVector), 
				m_Player.CollisionImpactForceMagnitude * elapsedSec);

			m_Enemies.at(i).Velocity.X += hitForce.X;
			m_Enemies.at(i).Velocity.Y += hitForce.Y;

			m_Enemies.at(i).Wounded = true;
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

	m_Player.Body.ScreenCircle.Center.X = m_Player.Body.WorldCircle.Center.X + cameraDelta.X;
	m_Player.Body.ScreenCircle.Center.Y = m_Player.Body.WorldCircle.Center.Y + cameraDelta.Y;


	for (size_t i = 0; i < m_Enemies.size(); i++)
	{
		m_Enemies.at(i).Body.ScreenCircle.Center.X = m_Enemies.at(i).Body.WorldCircle.Center.X + cameraDelta.X;
		m_Enemies.at(i).Body.ScreenCircle.Center.Y = m_Enemies.at(i).Body.WorldCircle.Center.Y + cameraDelta.Y;
	}

}

void Game::Draw( ) const
{
	ClearBackground( );

	for (size_t i = 0; i < m_Enemies.size(); i++)
	{
		m_Enemies.at(i).Body.ScreenCircle.Draw(FColor4i{ 255,0,0,255 }, true);
	}

	m_Player.Body.ScreenCircle.Draw(FColor4i{ 0,255,0,255 }, true);
	
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

	m_Player.CurrentShotForce =
		SVectors::Scale(
			SVectors::NormalizeVector(
				SVectors::Subtract(m_Player.Body.ScreenCircle.Center, FVector2f{ float(e.x), float(e.y) }))
			, m_Player.ShotForceMagnitude);
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
