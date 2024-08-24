#include "pch.h"
#include "Game.h"

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
	Body.Radius = 50;
	Body.Center = FVector2f{ 100,100 };

	Player.Body.Radius = 50;
	Player.Body.Center = FVector2f{ 400,100 };
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

	Player.Velocity.X += Player.CurrentShotForce.X * elapsedSec;
	Player.Velocity.Y += Player.CurrentShotForce.Y * elapsedSec;

	Player.Body.Center.X += Player.Velocity.X * elapsedSec;
	Player.Body.Center.Y += Player.Velocity.Y * elapsedSec;

	Player.CurrentShotForce.X = 0;
	Player.CurrentShotForce.Y = 0;

	if (SCollision::IsOverlapping(
		Body, Player.Body,
		OverlapInfo, Player.OverlapInfo))
	{
		Player.Body.Center.X += Player.OverlapInfo.TranslationVector.X;
		Player.Body.Center.Y += Player.OverlapInfo.TranslationVector.Y;

		Player.Velocity.X += Player.OverlapInfo.TranslationVector.X;
		Player.Velocity.Y += Player.OverlapInfo.TranslationVector.Y;
	}
}

void Game::Draw( ) const
{
	ClearBackground( );
	Body.Draw(FColor4i{ 255,0,0,255 }, true);
	Player.Body.Draw(FColor4i{ 0,255,0,255 }, true);
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

	Player.CurrentShotForce =
		SVectors::Scale(
			SVectors::NormalizeVector(
				SVectors::Subtract(Player.Body.Center, FVector2f{ float(e.x), float(e.y) }))
			, Player.ShotForceMagnitude);
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
