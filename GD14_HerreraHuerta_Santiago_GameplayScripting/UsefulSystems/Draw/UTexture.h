#pragma once

#include <unordered_set>
#include <unordered_map>

#include "base.h"

#include "../Structs/FColor.h"
#include "../Structs/FShapes.h"

namespace Pillar {

	using DTexturePixelData = std::vector<std::vector<std::array<GLubyte, 4>>>;

	class UTexture;

	class UTextureData {

	public:
		UTextureData() {}
		explicit UTextureData(const UTexture& texture);
		~UTextureData();
		UTextureData(const UTextureData& other);
		UTextureData(UTextureData&& other)noexcept;

		UTextureData& operator=(const UTextureData& other);
		UTextureData& operator=(UTextureData&& other)noexcept;

		size_t NumberOfRows()const;
		size_t NumberOfColumns()const;
		size_t NumberOfPixels()const;

		FColor4i PixelAt(size_t column, size_t row)const;
		FColor4i PixelAt(size_t pixelIndex)const;//count increases horizontally

		bool PixelIsInBounds(int x, int y)const;

		std::vector<FRectf> MakeRectsFromPixelsOfColor(const FColor4i& color, float xSize = 1.f,  float ySize = 1.f)const;

		bool IsValid()const;

	private:
		bool m_IsValid{ false };
		const int m_RGBA_Num{ 4 };
		GLubyte* m_Pixels{nullptr};
		size_t m_TextureWidth{0};
		size_t m_TextureHeight{0};

	};

	struct FTextureInfo {
		GLuint Id{ 0 };
		float TextureWidth{ 0 };
		float TextureHeight{ 0 };
	};

	class UTexture {

		//Just remember 0,0 is top left

	public:
		UTexture(const std::string& imagePath);
		UTexture(const std::string& text, const std::string& fontPath, int ptSize, const Color4f& textColor);

		void SetScreenPosition(const FVector2f& positionInScreen);
		void SetScale(const FVector2f& scaleInScreen);
		void SetTextureView(const FRectf& textureView);

		const FVector2f& GetPositionInScreen()const;
		const FVector2f& GetScaleInScreen()const;
		const FRectf& GetTextureView()const;

		float GetTextureOriginalWidth()const;
		float GetTextureOriginalHeight()const;

		float GetTextureScaledWidth()const;
		float GetTextureScaledHeight()const;

		void Draw()const;

		bool IsValid()const;

		static void DeleteAll();//for now, prevent draw after this

	private:

		friend class UTextureData;

		static std::unordered_map<std::string, FTextureInfo> s_TexturePath_To_TextureInfo;
		static bool s_AllDeleted;

		bool IsTextureLoaded(const std::string& texturePath);
		void CopyFromLoadedTexture(const std::string& texturePath);
		
		void CreateFromImage(const std::string& path);
		void CreateFromString(const std::string& text, TTF_Font* pFont, const Color4f& textColor);
		void CreateFromString(const std::string& text, const std::string& fontPath, int ptSize, const Color4f& textColor);
		void CreateFromSurface(SDL_Surface* pSurface);

		void DrawFilledRect(const FRectf& rect) const;

		FTextureInfo m_TextureInfo{};

		//DependentPerInstance
		FVector2f m_TexturePositionInScreen{};
		FVector2f m_TextureScaleInScreen{1,1};
		FRectf m_TextureView = FRectf(0, 0, 10, 10);
		bool m_IsValid{ false };
	};

	class USpriteSheet final{
	public:
		USpriteSheet(const UTexture& texture, 
			unsigned int numberOfRows, unsigned int numberOfColumns,
			unsigned int numberOfEmptyPixelsHorizontal, unsigned int numberOfEmptyPixelsVertical,
			const FVector2f& scaleInScreen = FVector2f{1,1});
	
		UTexture& GetFrame(size_t rowIndex, size_t columnIndex);

		void SetFrame(size_t rowIndex, size_t columnIndex);
		const UTexture& GetConstFrame()const;

		void SetScreenPosition(const FVector2f& position);
		void SetScale(const FVector2f& scaleInScreen);
		
		const FVector2f& GetPositionInScreen()const;
		const FVector2f& GetScaleInScreen()const;

		int GetOriginalCellWidth()const;
		int GetOriginalCellHeight()const;

		float GetScaledCellWidth()const;
		float GetScaledCellHeight()const;
		
		bool IsValid()const;
	
	private:

		void FrameTexture(size_t rowIndex, size_t columnIndex);
		UTexture m_Texture;
		size_t m_NumberOfRows; 
		size_t m_NumberOfColumns; 
		size_t m_NumberOfEmptyPixelsBetweenColumns;
		size_t m_NumberOfEmptyPixelsBetweenRows;
		size_t m_CellWidth{};
		size_t m_CellHeight{};
		
	};


}