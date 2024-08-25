#pragma once
#include "BaseGame.h"
#include "UsefulSystems/Includes.h"

using namespace Pillar;

struct FCircleCollider {

	FCirclef WorldCircle;
	FCirclef ScreenCircle;
};

struct FPlayer {

	bool PressingUp{ false };
	bool PressingDown{ false };
	bool PressingLeft{ false };
	bool PressingRight{ false };

	float MaxShieldRadius{20.f};
	float MinShieldRadius{0.f};
	FCircleCollider Shield;
	FOverlapInfo ShieldOverlapInfo;
	float ShieldDistanceFromBodyCenter{ 10 };
	bool ShieldCollided{false};
	bool ShieldActivated{false};
	bool ShieldReadyForDeactivation{ true };
	float MaxShieldActivationTime{0.5f};
	float CurrentShieldActivationTime{0.f};

	FCircleCollider Body;
	FOverlapInfo BodyOverlapInfo;
	FVector2f Velocity;
	FVector2f CurrentShotForce;

	float Gravity{1000};
	float MovementSpeed{ 40000 };
	float JumpSpeed{ 70000 };

	float MaxPlayerVelocityPerSecond{ 1000 };
	float DashForceMagnitude{ 60000 };
	float CollisionImpactForceMagnitude{ 500 };

	bool Wounded{ false };
	float CurrentWoundedTime{};
	float MaxWoundedTime{1.f};

	int MaxLife{ 3 };
	int CurrentLife{ 3 };


};

struct FBullet {
	FCircleCollider Body;
	FVector2f Velocity;
	bool Spawned{ false };
	float CurrentBulletTimeSpawned{ 0.f };
};

struct FCamera {

	float FollowingSpeed{10};
	FVector2f Position;
};

struct FEnemy {
	
	int CurrentLife{3};
	int MaxLife{3};
	float CurrentDeathTime{ 0.f };
	float MaxDeathTime{1.f};


	float MaxBodyRadius{50};
	float MinBodyRadius{0};
	FCircleCollider Body;
	FOverlapInfo BodyOverlapInfo;
	FVector2f Velocity;
	float FollowingSpeed{500};

	bool Wounded{ false };
	float MaxWoundedTime{1.f};
	float CurrentWoundedTime{0.f};
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
	FVector2f m_CurrentMouseScreenPosition{};

	FColor4i m_PlayerWoundedColor{ 255,100,100,255 };
	FColor4i m_PlayerNormalColor{ 0,255,0,255 };
	FPlayer m_Player{};

	int m_NumOfEnemies{10};
	FColor4i m_EnemyWoundedColor{255,0,255,255};
	FColor4i m_EnemyNormalColor{ 255,0,0,255 };
	std::vector<FEnemy> m_Enemies;

	FCircleCollider m_Bounds;

	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;
};