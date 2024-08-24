#pragma once

#include "../Structs/FVector.h"
#include "../Structs/FCollision.h"
#include "../Structs/FTransform.h"
#include "../Structs/FMatrix.h"

namespace Pillar {

	class SMatrix {

	public:
		SMatrix();

		static void Translate(FMatrix4x4f& in, const FVector2f& translation, bool lastTransformation = true);
		static void FlatRotateX(FMatrix4x4f& in, double rotation, bool lastTransformation = true);//This is like Reflection
		static void FlatRotateY(FMatrix4x4f& in, double rotation, bool lastTransformation = true);//This is like Reflection
		static void FlatRotateZ(FMatrix4x4f& in, double rotation, bool lastTransformation = true);//This is like Reflection
		static void RotateX(FMatrix4x4f& in, double rotation, bool lastTransformation = true);
		static void RotateY(FMatrix4x4f& in, double rotation, bool lastTransformation = true);
		static void RotateZ(FMatrix4x4f& in, double rotation, bool lastTransformation = true);
		static void MakeRotationMatrix(DMatrix2x2f& in, double degrees);
		static void Scale(FMatrix4x4f& in, const FVector2f& scale, bool lastTransformation = true);

		//Operations
		static void Inverse(const DMatrix2x2f& in, DMatrix2x2f& out);
		static void Inverse(DMatrix2x2f& matrixToInverse);

		static void MakeIdentity(FMatrix4x4f& matrix);

		static void Multiply(DMatrix2x2f& matrix, const float value);
		static void Multiply(DMatrix2x2f& first, const FVector2f& second, FVector2f& out);

		static void Multiply(const FMatrix4x4f& first, FMatrix4x4f& outSecond);

	};

}