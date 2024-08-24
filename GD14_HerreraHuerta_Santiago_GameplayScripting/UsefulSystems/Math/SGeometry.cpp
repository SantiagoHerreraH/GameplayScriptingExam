
#include "../ExternalIncludes.h"

#include "../Math/SGeometry.h"
#include "../Math/STrigonometry.h"
#include "../Math/SVectors.h"

namespace Pillar {

	const FVector2f SGeometry::Center(FShape2Df& shape)
	{

		FVector2f finalCenter{};
		for (size_t i = 0; i < shape.Shape.size(); i++)
		{
			finalCenter.X += shape.Shape[i].X;
			finalCenter.Y += shape.Shape[i].Y;
		}

		finalCenter.X /= shape.Shape.size();
		finalCenter.Y /= shape.Shape.size();

		return finalCenter;
	}

	void SGeometry::MakeRect(float width, float height, const FVector2f& center, FShape2Df& shape)
	{
		shape.Center = center;

		shape.Shape.resize(4);
		shape.Shape.at(0) = FVector2f{center.X - width / 2.f, center.Y - height / 2.f};
		shape.Shape.at(1) = FVector2f{center.X + width / 2.f, center.Y - height / 2.f};
		shape.Shape.at(2) = FVector2f{center.X + width / 2.f, center.Y + height / 2.f};
		shape.Shape.at(3) = FVector2f{center.X - width / 2.f, center.Y + height / 2.f};
	}

	std::vector<FRectf> SGeometry::Simplify(const std::vector<FRectf>& rects, ERectSimplificationMode rectSimplificationMode)
	{
		if (rects.empty())
		{
			return rects;
		}
		bool gotFused{ false };



		std::vector<FRectf> firstFusedRects;
		firstFusedRects.reserve(rects.size());
		firstFusedRects.push_back(rects.at(0));

		for (size_t rectsIdx = 1; rectsIdx < rects.size(); rectsIdx++)
		{
			for (size_t vertRectsIndex = 0; vertRectsIndex < firstFusedRects.size(); vertRectsIndex++)
			{
				switch (rectSimplificationMode)
				{
				case Pillar::ERectSimplificationMode::PrioritizeVerticalRect:

					if (CanBeOneVertical(rects[rectsIdx], firstFusedRects[vertRectsIndex], 1, 2, 1))
					{
						firstFusedRects[vertRectsIndex] = Fuse(rects[rectsIdx], firstFusedRects[vertRectsIndex]);
						gotFused = true;
						
					}

					break;
				case Pillar::ERectSimplificationMode::PrioritizeHorizontalRects:

					if (CanBeOneHorizontal(rects[rectsIdx], firstFusedRects[vertRectsIndex], 1, 2, 1))
					{
						firstFusedRects[vertRectsIndex] = Fuse(rects[rectsIdx], firstFusedRects[vertRectsIndex]);
						gotFused = true;
						
					}

					break;
				}

				if (gotFused)
				{
					break;
				}
				
			}

			if (!gotFused)
			{
				firstFusedRects.push_back(rects[rectsIdx]);
			}

			gotFused = false;
		}

		std::vector<FRectf> secondFusedRects;
		secondFusedRects.reserve(firstFusedRects.size());
		secondFusedRects.push_back(firstFusedRects.at(0));

		

		for (size_t vertRectIdx = 1; vertRectIdx < firstFusedRects.size(); vertRectIdx++)
		{
			for (size_t horizontalRectsIdx = 0; horizontalRectsIdx < secondFusedRects.size(); horizontalRectsIdx++)
			{
				switch (rectSimplificationMode)
				{
				case Pillar::ERectSimplificationMode::PrioritizeVerticalRect:

					if (CanBeOneHorizontal(firstFusedRects[vertRectIdx], secondFusedRects[horizontalRectsIdx], 1, 2, 1))
					{
						secondFusedRects[horizontalRectsIdx] = Fuse(firstFusedRects[vertRectIdx], secondFusedRects[horizontalRectsIdx]);
						gotFused = true;
						break;
					}

					break;
				case Pillar::ERectSimplificationMode::PrioritizeHorizontalRects:

					if (CanBeOneVertical(firstFusedRects[vertRectIdx], secondFusedRects[horizontalRectsIdx], 1, 2, 1))
					{
						secondFusedRects[horizontalRectsIdx] = Fuse(firstFusedRects[vertRectIdx], secondFusedRects[horizontalRectsIdx]);
						gotFused = true;
						break;
					}

					break;
				}
				
			}

			if (!gotFused)
			{

				secondFusedRects.push_back(firstFusedRects[vertRectIdx]);
			}

			gotFused = false;
		}

		

		return secondFusedRects;
	}

	bool SGeometry::CanBeOneHorizontal(const FRectf& rect1, const FRectf& rect2, float maxAcceptableYDistance, float maxAcceptableXDistance, float maxAcceptableHeightDelta)
	{
		float deltaBottom{ abs((rect1.Top -  rect1.Height) - (rect2.Top - rect2.Height)) };

		if (rect2.Left > (rect1.Left + rect1.Width + maxAcceptableXDistance)) return false;
		if (rect1.Left > (rect2.Left + rect2.Width + maxAcceptableXDistance)) return false;
		if (maxAcceptableHeightDelta > 0 && abs(rect1.Height - rect2.Height) > maxAcceptableHeightDelta) return false;

		return (deltaBottom <= maxAcceptableYDistance);
	}

	bool SGeometry::CanBeOneVertical(const FRectf& one, const FRectf& two, float maxAcceptableYDistance, float maxAcceptableXDistance, float maxAcceptableWidthDelta)
	{
		float deltaLeft{ abs(one.Left - two.Left) };
		float bottomOne{ one.Top - one.Height };
		float bottomTwo{ two.Top - two.Height };

		if (bottomOne > (two.Top + maxAcceptableYDistance)) return false;
		if (bottomTwo > (one.Top + maxAcceptableYDistance)) return false;

		if (maxAcceptableWidthDelta > 0 && abs(one.Width - two.Width) > maxAcceptableWidthDelta) return false;

		return deltaLeft <= maxAcceptableXDistance;
	}

	FRectf SGeometry::Fuse(const FRectf& one, const FRectf& two)
	{
		float bottomOne{ one.Top - one.Height};
		float bottomTwo{ two.Top - two.Height};
		float rightOne{ one.Left + one.Width };
		float rightTwo{ two.Left + two.Width };

		float lowestBottom = bottomOne < bottomTwo ? bottomOne : bottomTwo;
		float lowestLeft = one.Left < two.Left ? one.Left : two.Left;
		float highestTop = one.Top > two.Top ? one.Top : two.Top;
		float highestRight = rightOne > rightTwo ? rightOne : rightTwo;

		return FRectf(
			lowestLeft,
			lowestBottom + (highestTop - lowestBottom),
			highestRight - lowestLeft,
			highestTop - lowestBottom);
	}

	FVector2f SGeometry::EvaluateLine(const FLine2f& line, float percentage)
	{
		return SVectors::Add(line.Start, SVectors::Scale(SVectors::Subtract(line.End, line.Start), percentage));
	}
}