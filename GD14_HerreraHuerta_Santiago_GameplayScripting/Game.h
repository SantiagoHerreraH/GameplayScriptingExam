#pragma once
#include "BaseGame.h"
#include "UsefulSystems/Includes.h"

using namespace Pillar;

struct FCircleCollider {

	FCirclef WorldCircle;
	FCirclef ScreenCircle;
};

struct FPlayer {

	FCircleCollider Shield;
	FCircleCollider Body;
	FOverlapInfo OverlapInfo;
	FVector2f Velocity;
	FVector2f CurrentShotForce;
	float ShotForceMagnitude{ 60000 };
};

struct FCamera {

	float FollowingSpeed{10};
	FVector2f Position;
};

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

private:

	FCamera m_Camera{};
	FPlayer m_Player{};

	FCircleCollider Body;
	FOverlapInfo OverlapInfo;

	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;
};