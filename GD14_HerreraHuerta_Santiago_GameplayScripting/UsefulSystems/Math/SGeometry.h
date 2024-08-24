#pragma once


#include "../Structs/FShapes.h"

namespace Pillar {

	enum class ERectSimplificationMode {
		PrioritizeVerticalRect,
		PrioritizeHorizontalRects
	};

	class SGeometry {

	public:
		static const FVector2f Center(FShape2Df& shape);

		static void MakeRect(float width, float height, const FVector2f& center, FShape2Df& shape);

		static std::vector<FRectf> Simplify(const std::vector<FRectf>& rects, ERectSimplificationMode rectSimplificationMode = ERectSimplificationMode::PrioritizeVerticalRect);
		static bool CanBeOneHorizontal(const FRectf& one, const FRectf& two, float maxAcceptableYDistance, float maxAcceptableXDistance, float maxAcceptableHeightDelta);
		static bool CanBeOneVertical(const FRectf& one, const FRectf& two, float maxAcceptableYDistance, float maxAcceptableXDistance, float maxAcceptableWidthDelta);
		static FRectf Fuse(const FRectf& one, const FRectf& two);

		static FVector2f EvaluateLine(const FLine2f& line, float percentage);
	};
}