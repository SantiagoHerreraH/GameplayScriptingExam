#pragma once
#include "BaseGame.h"
#include "UsefulSystems/Includes.h"
#include "UPlayer.h"
#include "UBullet.h"

using namespace Pillar;


struct FCamera {

	float FollowingSpeed{10};
	FVector2f Position;
};



struct FLevelManager {
	
	std::string GeneralLevelString{};
	int NumberOfLevels{};
	int CurrentLevel{};
	std::vector<FRectCollider> LevelCollisions;

};

//TO DOS
/*
- implement all bullet behaviour
- implement enemy behaviour
- implement pick ups

Bullets
Two types of bullets
Movement
Ones that go towards you
Others that go in a straight line
Ones that are static.
Ones that are only in the wounded state.

*/

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
	FVector2f m_CurrentMouseScreenPosition{};

	UPlayer m_Player{};

	int m_NumOfBullets{0};
	
	std::vector<FBullet> m_Bullets;

	//FRectCollider m_RectTest{};
	std::vector<FRectCollider> m_CurrentLevel;

	FCircleCollider m_Bounds;

	void CreatePatrolEnemy(int maxLife, bool alwaysLookTowardsPlayer, FVector2f beginPatrol, FVector2f endPatrol, int bulletNumber, const FBullet& bulletType);
	void CreateFreePatrolEnemy(int maxLife, const std::vector<FVector2f>& patrolPoints, int bulletNumber, const FBullet& bulletType);
	void CreateFollowingEnemy(int maxLife, int bulletNumber, const FBullet& bulletType);

	FBullet CreateUniDirectionalBullet(int MaxLife);
	FBullet CreateFollowingBullet(int maxLife);
	FBullet CreateStaticBullet(int maxLife, const FVector2f& position, bool alwaysWounded );
	FBullet CreateMovableBullet(int maxLife, const FVector2f& position);


	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;
};