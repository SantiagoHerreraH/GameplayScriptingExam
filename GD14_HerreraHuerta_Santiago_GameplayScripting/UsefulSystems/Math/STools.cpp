

#include "../ExternalIncludes.h"

#include "../Math/STools.h"
#include "../Math/SVectors.h"

namespace Pillar {
	const int STools::GetRandomBetweenRange(int min, int max)
	{
		//if you don't add the + 1, the range can never reach max
		return (std::rand() % (max - min + 1)) + min;
	}

	const FVector2i STools::CalculateLerpPoint(const FVector2i& first, const FVector2i& second, const double percentageFromZeroToOne)
	{

		int X = (second.X - first.X) * Clamp(percentageFromZeroToOne, 0, 1);
		int Y = (second.Y - first.Y) * Clamp(percentageFromZeroToOne, 0, 1);

		return { first.X + X, first.Y + Y };
	}

	const FVector2d STools::CalculateLerpPoint(const FVector2d& start, const FVector2d& end, const double percentageFromZeroToOne)
	{
		double X = (end.X - start.X) * Clamp(percentageFromZeroToOne, 0, 1);
		double Y = (end.Y - start.Y) * Clamp(percentageFromZeroToOne, 0, 1);

		return { start.X + X, start.Y + Y };
	}

	const FVector2f STools::CalculateLerpPoint(const FVector2f& start, const FVector2f& end, const double percentageFromZeroToOne)
	{
		return SVectors::CastToFloatPoint(CalculateLerpPoint(SVectors::CastToDoublePoint(start), SVectors::CastToDoublePoint(end), percentageFromZeroToOne));
	}
}