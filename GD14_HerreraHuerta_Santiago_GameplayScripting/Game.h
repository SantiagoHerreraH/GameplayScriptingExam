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
	std::vector<FBullet> m_Bullets;

	const FColor4i m_LevelCollisionColor	{0,		0,     0, 255 };
	const FColor4i m_StaticShooterColor		{255,	0,     0, 255 };
	const FColor4i m_PatrolEnemyColor		{255,	142,   0, 255 };
	const FColor4i m_FollowPlayerEnemyColor	{113,	0,   255, 255 };
	const FColor4i m_StaticBulletColor		{255,	0,   175, 255 };
	const FColor4i m_MovableBulletColor		{0,		0,   255, 255 };
	const FColor4i m_SpawnPointColor		{125,  255,    0, 255 };
	const FColor4i m_WinPointColor			{ 251, 242,   54, 255 };

	const FVector2f m_LevelScale{ 70,70 };
	const std::string m_GeneralLevelString{ "Level_" };
	const int m_MaxLevelNum{2};
	int m_CurrentLevelIndex{0};
	std::vector<FRectCollider> m_CurrentLevel;

	std::vector<FRectCollider> m_SpawnPoints;
	FRectCollider m_WinBounds{};

	void ResetLevel();
	void NextLevel();
	void LoadLevel(int levelIndex);

	void CreateSpawnPoint(const FRectf& rect);

	void CreateStaticShooterEnemy(int maxLife, FVector2f shootDirection, const FVector2f& position, int bulletNumber, const FBullet& bulletType);
	void CreatePatrolEnemy(int maxLife, bool alwaysLookTowardsPlayer, FVector2f beginPatrol, FVector2f endPatrol, int bulletNumber, const FBullet& bulletType);
	void CreateFreePatrolEnemy(int maxLife, const std::vector<FVector2f>& patrolPoints, int bulletNumber, const FBullet& bulletType);
	void CreateFollowingEnemy(int maxLife, int bulletNumber, const FVector2f& position, const FBullet& bulletType);

	FBullet CreatePerishableBullet(int maxLife);
	FBullet CreateUniDirectionalBullet(int MaxLife);
	FBullet CreateFollowingBullet(int maxLife);
	FBullet CreateStaticBullet(int maxLife, const FVector2f& position);//if this is bond by life, it will spawn in index num 0
	FBullet CreateMovableBullet(int maxLife, const FVector2f& position);//if this is bond by life, it will spawn in index num 0


	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;
};