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

			float rotatedHeight{ rectCollider.OriginalRect.Height * cosf(SAngles::InRadians(RotationX)) };
			float rotatedWidth{ rectCollider.OriginalRect.Width * cosf(SAngles::InRadians(RotationY)) };

			rectCollider.CurrentRect.Top = std::round((rectCollider.OriginalRect.Top * Scale.Y) + Position.Y);
			rectCollider.CurrentRect.Left = std::round((rectCollider.OriginalRect.Left * Scale.X) + Position.X);

			rectCollider.CurrentRect.Height = std::round(abs(rotatedHeight) * Scale.Y);
			rectCollider.CurrentRect.Width = std::round(abs(rotatedWidth) * Scale.X);
		}

		FVector2f Position{};
		float RotationX{};
		float RotationY{};
		float RotationZ{};
		FVector2f Scale{ 1.f, 1.f };
	};
}

