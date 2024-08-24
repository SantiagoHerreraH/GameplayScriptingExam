

#include "../ExternalIncludes.h"

#include "../Math/SMatrix.h"
#include "../Math/SVectors.h"
#include "../Math/SAngles.h"

namespace Pillar {

	SMatrix::SMatrix()
	{
	}

	void SMatrix::Translate(FMatrix4x4f& in, const FVector2f& translation, bool lastTransformation)
	{
		FMatrix4x4f translationMatrix{ };
		translationMatrix.Values[0][3] = translation.X;
		translationMatrix.Values[1][3] = translation.Y;

		if (lastTransformation)
		{
			Multiply(translationMatrix, in);
		}
		else
		{
			Multiply(in, translationMatrix);
			in = translationMatrix;
		}
		
	}

	void SMatrix::FlatRotateX(FMatrix4x4f& in, double rotation, bool lastTransformation)
	{
		FMatrix4x4f rotationMatrix{};
		double radians(SAngles::InRadians(rotation));
		rotationMatrix.Values[0][0] = cos(radians);

		if (lastTransformation)
		{
			Multiply(rotationMatrix, in);
		}
		else
		{
			Multiply(in, rotationMatrix);
			in = rotationMatrix;
		}
	}

	void SMatrix::FlatRotateY(FMatrix4x4f& in, double rotation, bool lastTransformation)
	{
		FMatrix4x4f rotationMatrix{};
		double radians(SAngles::InRadians(rotation));
		rotationMatrix.Values[1][1] = cos(radians);

		if (lastTransformation)
		{
			Multiply(rotationMatrix, in);
		}
		else
		{
			Multiply(in, rotationMatrix);
			in = rotationMatrix;
		}
	}

	void SMatrix::FlatRotateZ(FMatrix4x4f& in, double rotation, bool lastTransformation)
	{
		FMatrix4x4f rotationMatrix{};
		double radians(SAngles::InRadians(rotation));
		rotationMatrix.Values[2][2] = cos(radians);

		if (lastTransformation)
		{
			Multiply(rotationMatrix, in);
		}
		else
		{
			Multiply(in, rotationMatrix);
			in = rotationMatrix;
		}
	}

	void SMatrix::RotateX(FMatrix4x4f& in, double rotation, bool lastTransformation)
	{
		FMatrix4x4f rotationMatrix{};
		double radians(SAngles::InRadians(rotation));
		rotationMatrix.Values[1][1] =  cos(radians);
		rotationMatrix.Values[1][2] = -sin(radians);
		rotationMatrix.Values[2][1] =  sin(radians);
		rotationMatrix.Values[2][2] =  cos(radians);

		if (lastTransformation)
		{
			Multiply(rotationMatrix, in);
		}
		else
		{
			Multiply(in, rotationMatrix);
			in = rotationMatrix;
		}
	}

	void SMatrix::RotateY(FMatrix4x4f& in, double rotation, bool lastTransformation)
	{
		FMatrix4x4f rotationMatrix{};
		double radians(SAngles::InRadians(rotation));
		rotationMatrix.Values[0][0] =  cos(radians);
		rotationMatrix.Values[2][0] = -sin(radians);
		rotationMatrix.Values[0][2] =  sin(radians);
		rotationMatrix.Values[2][2] =  cos(radians);

		if (lastTransformation)
		{
			Multiply(rotationMatrix, in);
		}
		else
		{
			Multiply(in, rotationMatrix);
			in = rotationMatrix;
		}
	}

	void SMatrix::RotateZ(FMatrix4x4f& in, double rotation, bool lastTransformation)
	{
		FMatrix4x4f rotationMatrix{};
		double radians(SAngles::InRadians(rotation));
		rotationMatrix.Values[0][0] =  cos(radians);
		rotationMatrix.Values[0][1] = -sin(radians);
		rotationMatrix.Values[1][0] =  sin(radians);
		rotationMatrix.Values[1][1] =  cos(radians);

		if (lastTransformation)
		{
			Multiply(rotationMatrix, in);
		}
		else
		{
			Multiply(in, rotationMatrix);
			in = rotationMatrix;
		}
		
	}

	void SMatrix::MakeRotationMatrix(DMatrix2x2f& in, double degrees)
	{
		double radians(SAngles::InRadians(degrees));
		DMatrix2x2f rotationMatrix{};
		rotationMatrix[0][0] = cos(radians);
		rotationMatrix[0][1] = -sin(radians);
		rotationMatrix[1][0] = sin(radians);
		rotationMatrix[1][1] = cos(radians);
	}

	void SMatrix::Scale(FMatrix4x4f& in, const FVector2f& scale, bool lastTransformation)
	{
		FMatrix4x4f scaleMatrix{};
		scaleMatrix.Values[0][0] = scale.X;
		scaleMatrix.Values[1][1] = scale.Y;

		if (lastTransformation)
		{
			Multiply(scaleMatrix, in);
		}
		else
		{
			Multiply(in, scaleMatrix);
			in = scaleMatrix;
		}
		
	}

	void SMatrix::Inverse(const DMatrix2x2f& in, DMatrix2x2f& out)
	{
		FVector2f rightVector{ in[0][0],in[1][0] };
		FVector2f upVector{ in[0][1],in[1][1] };
		float determinant{ SVectors::Determinant(rightVector, upVector) };
		float scalar = determinant == 0 ? 0 : 1.0f / determinant;
		
		out[0][0] =  in[1][1];
		out[0][1] = -in[0][1];
		out[1][0] = -in[1][0];
		out[1][1] =  in[0][0];

		Multiply(out, scalar);
	}

	void SMatrix::Inverse(DMatrix2x2f& matrixToInverse)
	{
		FVector2f rightVector{ matrixToInverse[0][0],matrixToInverse[1][0] };
		FVector2f upVector	{ matrixToInverse[0][1],matrixToInverse[1][1]};
		float determinant{ SVectors::Determinant(rightVector, upVector) };
		float scalar = determinant == 0 ? 0 : 1.0f / determinant;

		float temp{ matrixToInverse[0][0] };
		

		matrixToInverse[0][0] = matrixToInverse[1][1];
		matrixToInverse[0][1] *= -1;
		matrixToInverse[1][0] *= -1;
		matrixToInverse[1][1] = temp;

		Multiply(matrixToInverse, scalar);
		
	}

	void SMatrix::MakeIdentity(FMatrix4x4f& matrix)
	{
		for (size_t rowIdx = 0; rowIdx < 4; rowIdx++)
		{
			for (size_t columnIdx = 0; columnIdx < 4; columnIdx++)
			{
				if (rowIdx == columnIdx)
				{
					matrix.Values[rowIdx][columnIdx] = 1;
				}
				else
				{
					matrix.Values[rowIdx][columnIdx] = 0;
				}
			}
		}
	}

	void SMatrix::Multiply(DMatrix2x2f& matrix, const float value)
	{
		//for each row
		for (size_t currentRowIndex = 0; currentRowIndex < matrix.size(); currentRowIndex++)
		{
			//for each column
			for (size_t currentColumnIndex = 0; currentColumnIndex < matrix[currentRowIndex].size(); currentColumnIndex++)
			{
				matrix[currentRowIndex][currentColumnIndex] *= value;
			}
		}
	}

	void SMatrix::Multiply(DMatrix2x2f& first, const FVector2f& second, FVector2f& out)
	{
		out.X = second.X * first[0][0] + second.Y * first[0][1];
		out.Y = second.X * first[1][0] + second.Y * first[1][1];
	}

	void SMatrix::Multiply(const FMatrix4x4f& first, FMatrix4x4f& outSecond)
	{
		FMatrix4x4f result{};
		for (size_t resultColumnIdx = 0; resultColumnIdx < 4; resultColumnIdx++)
		{
			for (size_t resultRowIdx = 0; resultRowIdx < 4; resultRowIdx++)
			{

				result.Values[resultRowIdx][resultColumnIdx] = 0;

				for (size_t currentMultiplication = 0; currentMultiplication < 4; currentMultiplication++)
				{
					result.Values[resultRowIdx][resultColumnIdx] +=
						first.Values[resultRowIdx][currentMultiplication] *
						outSecond.Values[currentMultiplication][resultColumnIdx];
				}
				
			}
		}

		outSecond.Values.swap(result.Values);
	}

}