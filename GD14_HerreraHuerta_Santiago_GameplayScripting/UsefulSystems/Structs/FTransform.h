#pragma once
#include "base.h"
#include "../Structs/FVector.h"
#include "../Structs/FCollision.h"
#include "../Math/SAngles.h"

namespace Pillar {

	struct FTransform
	{
	public:
		void ApplyTextureTransformation() const
		{
			glPushMatrix();
			glTranslatef(Position.X, Position.Y, 0);
			glRotatef(RotationZ, 0, 0, 1);
			glRotatef(RotationY, 0, 1, 0);
			glRotatef(RotationX, 1, 0, 0);
			glScalef(Scale.X, Scale.Y, 1);
		}

		void ResetTextureTransformation() const
		{
			glPopMatrix();
		}

		void ApplyRectTransformation(FRectCollider& rectCollider) {

			float rotatedHeight{ rectCollider.WorldRect.Height * cosf(SAngles::InRadians(RotationX)) };
			float rotatedWidth{ rectCollider.WorldRect.Width * cosf(SAngles::InRadians(RotationY)) };

			rectCollider.ScreenRect.Top = std::round((rectCollider.WorldRect.Top * Scale.Y) + Position.Y);
			rectCollider.ScreenRect.Left = std::round((rectCollider.WorldRect.Left * Scale.X) + Position.X);

			rectCollider.ScreenRect.Height = std::round(abs(rotatedHeight) * Scale.Y);
			rectCollider.ScreenRect.Width = std::round(abs(rotatedWidth) * Scale.X);
		}

		FVector2f Position{};
		float RotationX{};
		float RotationY{};
		float RotationZ{};
		FVector2f Scale{ 1.f, 1.f };
	};
}

