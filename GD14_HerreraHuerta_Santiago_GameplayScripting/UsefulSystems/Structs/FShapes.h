#pragma once

#include "../ExternalIncludes.h"

#include "../Structs/FVector.h"
#include "../Structs/FColor.h"


namespace Pillar {

#pragma region Shape

	using DShape2Df = std::vector<FVector2f>;

	struct FShape2Df {

		DShape2Df Shape;
		FVector2f Center;
	};

	struct FBaseShape2Df {

		DShape2Df Shape;
		FVector2f Center;
	};

#pragma endregion

#pragma region Triangle

	struct FTriangle2Di {
		FVector2i Vertices[3]{};
	};

	struct FTriangle2D {
		FVector2d Vertices[3]{};
	};

	struct FTriangle2f {
		FVector2f Vertices[3]{};
	};

#pragma endregion

#pragma region Line

	struct FLine2f {
		FVector2f Start;
		FVector2f End;
	};

	struct FLine2i {
		FVector2i Start;
		FVector2i End;
	};

	struct FLine2 {
		FVector2d Start;
		FVector2d End;
	};

#pragma endregion

#pragma region Circle

	template <typename Type>
	struct FCircle {

		FVector2<Type> Center;

		Type Radius;

		void Draw(const FColor4i color, bool fill, float lineWidth = 1.f) const{
			
			glColor4f(
				color.GetR() / (float)color.MAXVALUE,
				color.GetG() / (float)color.MAXVALUE,
				color.GetB() / (float)color.MAXVALUE,
				color.GetA() / (float)color.MAXVALUE);

			float dAngle{ float(M_PI / Radius) };

			if (fill)
			{

				glBegin(GL_POLYGON);
				{
					for (float angle = 0.0; angle < float(2 * M_PI); angle += dAngle)
					{
						glVertex2f(Center.X + Radius * float(cos(angle)), Center.Y + Radius * float(sin(angle)));
					}
				}
				glEnd();
			}
			else
			{
				glLineWidth(lineWidth);
				glBegin(GL_LINE_LOOP);
				{
					for (float angle = 0.0; angle < float(2 * M_PI); angle += dAngle)
					{
						glVertex2f(Center.X + Radius * float(cos(angle)), Center.Y + Radius * float(sin(angle)));
					}
				}
				glEnd();
			}
		}

	};

	using FCirclef = FCircle<float>;
	using FCircled = FCircle<double>;
	using FCirclei = FCircle<int>;

#pragma endregion

#pragma region Rectangle
	//For this engine it is a bit counter intutive, but bottom bigger than top becasue positive y goes down

	template<typename Type>
	struct FRect {

		FRect() {};
		explicit FRect(Type left, Type top, Type width, Type height) : Left(left), Top(top), Width(width), Height(height) {};
		Type Left;
		Type Top;
		Type Width;
		Type Height;

		void Draw(const FColor4i color, bool fill, bool isYInverted, float lineWidth = 1)const {
			
			glColor4f(
				color.GetR() / (float)color.MAXVALUE,
				color.GetG() / (float)color.MAXVALUE,
				color.GetB() / (float)color.MAXVALUE,
				color.GetA() / (float)color.MAXVALUE);

			if (fill)
			{

				glBegin(GL_POLYGON);
				{
					glVertex2f(Left, Top);
					glVertex2f(Left + Width, Top);
					if (isYInverted)
					{
						glVertex2f(Left + Width, Top + Height);
					}
					else
					{
						glVertex2f(Left + Width, Top - Height);
						glVertex2f(Left, Top - Height);
					}
				}
				glEnd();
			}
			else
			{
				glLineWidth(lineWidth);
				glBegin(GL_LINE_LOOP);
				{
					glVertex2f(Left, Top);
					glVertex2f(Left + Width, Top);
					if (isYInverted)
					{
						glVertex2f(Left + Width, Top + Height);
						glVertex2f(Left, Top + Height);
					}
					else
					{
						glVertex2f(Left + Width, Top - Height);
						glVertex2f(Left, Top - Height);
					}
				}
				glEnd();
			}
			
		}

	};

	using FRectf = FRect<float>;
	using FRecti = FRect<int>;
	using FRectd = FRect<double>;

#pragma endregion

}