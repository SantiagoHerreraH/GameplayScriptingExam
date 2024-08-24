
#include "../ExternalIncludes.h"

#include "../Draw/UTexture.h"
#include "utils.h"
#include <iostream>

namespace Pillar {

#pragma region Base Texture

	void UTexture::SetScreenPosition(const FVector2f& positionInScreen)
	{
		m_TexturePositionInScreen = positionInScreen;
	}
	void UTexture::SetScale(const FVector2f& scaleInScreen)
	{
		m_TextureScaleInScreen = scaleInScreen;
	}
	void UTexture::SetTextureView(const FRectf& textureView)
	{
		m_TextureView = textureView;
	}
	const FVector2f& UTexture::GetPositionInScreen() const
	{
		return m_TexturePositionInScreen;
	}
	const FVector2f& UTexture::GetScaleInScreen() const
	{
		return m_TextureScaleInScreen;
	}
	const FRectf& UTexture::GetTextureView() const
	{
		return m_TextureView;
	}
	
	float UTexture::GetTextureOriginalWidth()const
	{
		return m_TextureInfo.TextureWidth;
	}
	float UTexture::GetTextureOriginalHeight()const
	{
		return m_TextureInfo.TextureHeight;
	}

	float UTexture::GetTextureScaledWidth()const
	{
		return m_TextureScaleInScreen.X * m_TextureInfo.TextureWidth;
	}

	float UTexture::GetTextureScaledHeight()const
	{
		return m_TextureScaleInScreen.X * m_TextureInfo.TextureHeight;
	}

	void UTexture::Draw() const
	{
		const float epsilon{ 0.001f };
		if (!m_IsValid || s_AllDeleted)
		{
			DrawFilledRect(FRectf{ 0,0,100,100 });
			return;
		}

		// Determine texture coordinates using srcRect and default destination width and height
		float textLeft{};
		float textRight{};
		float textTop{};
		float textBottom{};

		float defaultDestWidth{};
		float defaultDestHeight{};
		if (!(m_TextureView.Width > epsilon && m_TextureView.Height > epsilon)) // No srcRect specified
		{
			// Use complete texture
			textLeft = 0.0f;
			textRight = 1.0f;
			textTop = 0.0f;
			textBottom = 1.0f;

			defaultDestHeight = m_TextureInfo.TextureHeight;
			defaultDestWidth = m_TextureInfo.TextureWidth;
		}
		else // srcRect specified
		{
			//CHANGED THIS -> TEST

			// Convert to the range [0.0, 1.0]
			textLeft = m_TextureView.Left / m_TextureInfo.TextureWidth;
			textRight = (m_TextureView.Left + m_TextureView.Width) / m_TextureInfo.TextureWidth;
			textBottom = ((m_TextureView.Top) / m_TextureInfo.TextureHeight); //this is because positive y goes downward
			textTop = ((m_TextureView.Top + m_TextureView.Height) / m_TextureInfo.TextureHeight); // this is because positive y goes downward

			defaultDestHeight = m_TextureView.Height;
			defaultDestWidth = m_TextureView.Width;
		}

		Point2f actualTextureSizeInScreen{ m_TextureScaleInScreen.X * m_TextureView.Width, m_TextureScaleInScreen.Y * m_TextureView.Height};
		// Determine vertex coordinates
		float vertexLeft{ m_TexturePositionInScreen.X };
		float vertexBottom{ m_TexturePositionInScreen.Y };
		float vertexRight{};
		float vertexTop{};
		if (!(actualTextureSizeInScreen.x > 0.001f && actualTextureSizeInScreen.y > 0.001f)) // If no size specified use default size
		{
			vertexRight = vertexLeft + defaultDestWidth;
			vertexTop = vertexBottom + defaultDestHeight;
		}
		else
		{
			vertexRight = vertexLeft + actualTextureSizeInScreen.x;
			vertexTop = vertexBottom + actualTextureSizeInScreen.y;

		}

		// Tell opengl which texture we will use
		glBindTexture(GL_TEXTURE_2D, m_TextureInfo.Id);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		// Draw
		glEnable(GL_TEXTURE_2D);
		{
			glBegin(GL_QUADS);
			{
				glTexCoord2f(textLeft, textBottom);
				glVertex2f(vertexLeft, vertexBottom);

				glTexCoord2f(textLeft, textTop);
				glVertex2f(vertexLeft, vertexTop);

				glTexCoord2f(textRight, textTop);
				glVertex2f(vertexRight, vertexTop);

				glTexCoord2f(textRight, textBottom);
				glVertex2f(vertexRight, vertexBottom);
			}
			glEnd();
		}
		glDisable(GL_TEXTURE_2D);
	}

	bool UTexture::IsValid() const
	{
		return m_IsValid;
	}

	void UTexture::DeleteAll()
	{
		for (auto It = s_TexturePath_To_TextureInfo.begin(); It != s_TexturePath_To_TextureInfo.end(); ++It)
		{
			glDeleteTextures(1, &It->second.Id);
		}

		s_TexturePath_To_TextureInfo.clear();
		s_AllDeleted = true;
	}

	void UTexture::DrawFilledRect(const FRectf& rect) const
	{
		glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
		glBegin(GL_POLYGON);
		{
			glVertex2f(rect.Left, rect.Top - rect.Height);
			glVertex2f(rect.Left + rect.Width, rect.Top - rect.Height);
			glVertex2f(rect.Left + rect.Width, rect.Top);
			glVertex2f(rect.Left, rect.Top);
		}
		glEnd();
	}

#pragma endregion

#pragma region Texture

	std::unordered_map<std::string, FTextureInfo> UTexture::s_TexturePath_To_TextureInfo;
	bool UTexture::s_AllDeleted{false};

	UTexture::UTexture(const std::string& imagePath)
	{
		if (IsTextureLoaded(imagePath))
		{
			CopyFromLoadedTexture(imagePath);
		}
		else
		{
			CreateFromImage(imagePath);
		}

		m_TextureView.Width = m_TextureInfo.TextureWidth;
		m_TextureView.Height = m_TextureInfo.TextureHeight;
	}
	
	UTexture::UTexture(const std::string& text, const std::string& fontPath, int ptSize, const Color4f& textColor)
	{
		if (IsTextureLoaded(fontPath))
		{
			CopyFromLoadedTexture(fontPath);
		}
		else
		{
			CreateFromString(text, fontPath, ptSize, textColor);
		}

		m_TextureView.Width = m_TextureInfo.TextureWidth;
		m_TextureView.Height = m_TextureInfo.TextureHeight;
	}

	bool UTexture::IsTextureLoaded(const std::string& texturePath)
	{
		return !(s_TexturePath_To_TextureInfo.end() == s_TexturePath_To_TextureInfo.find(texturePath));
	}

	void UTexture::CopyFromLoadedTexture(const std::string& texturePath)
	{
		m_TextureInfo = s_TexturePath_To_TextureInfo.at(texturePath);
	}

	void UTexture::CreateFromImage(const std::string& path)
	{
		m_IsValid = true;

		// Load image at specified path
		SDL_Surface* pLoadedSurface = IMG_Load(("../Resources/" + path).c_str());
		if (pLoadedSurface == nullptr)
		{
			std::cerr << "Texture::CreateFromImage, error when calling IMG_Load: " << SDL_GetError() << std::endl;
			m_IsValid = false;
			return;
		}
		CreateFromSurface(pLoadedSurface);

		// Free loaded surface
		SDL_FreeSurface(pLoadedSurface);

	}

	void UTexture::CreateFromString(const std::string& text, const std::string& fontPath, int ptSize, const Color4f& textColor)
	{
		m_IsValid = true;

		// Create font
		TTF_Font* pFont{};
		pFont = TTF_OpenFont(("../Resources/" + fontPath).c_str(), ptSize);
		if (pFont == nullptr)
		{
			std::cerr << "Texture::CreateFromString, error when calling TTF_OpenFont: " << TTF_GetError() << std::endl;
			m_IsValid = false;
			return;
		}

		// Create texture using this font and close font afterwards
		CreateFromString(text, pFont, textColor);
		TTF_CloseFont(pFont);
	}

	void UTexture::CreateFromString(const std::string& text, TTF_Font* pFont, const Color4f& color)
	{
		m_IsValid = true;
		if (pFont == nullptr)
		{
			std::cerr << "Texture::CreateFromString, invalid TTF_Font pointer\n";
			m_IsValid = false;
			return;
		}

		// Render text surface
		SDL_Color textColor{};
		textColor.r = Uint8(color.r * 255);
		textColor.g = Uint8(color.g * 255);
		textColor.b = Uint8(color.b * 255);
		textColor.a = Uint8(color.a * 255);

		SDL_Surface* pLoadedSurface = TTF_RenderText_Blended(pFont, text.c_str(), textColor);
		if (pLoadedSurface == nullptr)
		{
			std::cerr << "Texture::CreateFromString, error when calling TTF_RenderText_Blended: " << TTF_GetError() << std::endl;
			m_IsValid = false;
			return;
		}

		// Copy to video memory
		CreateFromSurface(pLoadedSurface);

		// Free loaded surface
		SDL_FreeSurface(pLoadedSurface);
	}

	void UTexture::CreateFromSurface(SDL_Surface* pSurface)
	{
		m_IsValid = true;

		//Get image dimensions
		m_TextureInfo.TextureWidth = float(pSurface->w);

		m_TextureInfo.TextureHeight = float(pSurface->h);

		// Get pixel format information and translate to OpenGl format
		GLenum pixelFormat{ GL_RGB };
		switch (pSurface->format->BytesPerPixel)
		{
		case 3:
			if (pSurface->format->Rmask == 0x000000ff)
			{
				pixelFormat = GL_RGB;
			}
			else
			{
				pixelFormat = GL_BGR;
			}
			break;
		case 4:
			if (pSurface->format->Rmask == 0x000000ff)
			{
				pixelFormat = GL_RGBA;
			}
			else
			{
				pixelFormat = GL_BGRA;
			}
			break;
		default:
			std::cerr << "Texture::CreateFromSurface, unknow pixel format, BytesPerPixel: " << pSurface->format->BytesPerPixel << "\nUse 32 bit or 24 bit images.\n";
			m_IsValid = false;
			return;
		}

		//Generate an array of textures.  We only want one texture (one element array), so trick
		//it by treating "texture" as array of length one.
		glGenTextures(1, &m_TextureInfo.Id);

		//Select (bind) the texture we just generated as the current 2D texture OpenGL is using/modifying.
		//All subsequent changes to OpenGL's texturing state for 2D textures will affect this texture.
		glBindTexture(GL_TEXTURE_2D, m_TextureInfo.Id);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, pSurface->pitch / pSurface->format->BytesPerPixel);
		// check for errors. Can happen if a texture is created while a static pointer is being initialized, even before the call to the main function.
		GLenum e = glGetError();
		if (e != GL_NO_ERROR)
		{
			std::cerr << "Texture::CreateFromSurface, error binding textures, Error id = " << e << '\n';
			std::cerr << "Can happen if a texture is created before performing the initialization code (e.g. a static Texture object).\n";
			std::cerr << "There might be a white rectangle instead of the image.\n";
		}

		// Specify the texture's data.  
		// This function is a bit tricky, and it's hard to find helpful documentation. 
		// A summary:
		//    GL_TEXTURE_2D:    The currently bound 2D texture (i.e. the one we just made)
		//                0:    The mipmap level.  0, since we want to update the base level mipmap image (i.e., the image itself,
		//                         not cached smaller copies)
		//          GL_RGBA:    Specifies the number of color components in the texture.
		//                     This is how OpenGL will store the texture internally (kinda)--
		//                     It's essentially the texture's type.
		//       surface->w:    The width of the texture
		//       surface->h:    The height of the texture
		//                0:    The border.  Don't worry about this if you're just starting.
		//      pixelFormat:    The format that the *data* is in--NOT the texture! 
		// GL_UNSIGNED_BYTE:    The type the data is in.  In SDL, the data is stored as an array of bytes, with each channel
		//                         getting one byte.  This is fairly typical--it means that the image can store, for each channel,
		//                         any value that fits in one byte (so 0 through 255).  These values are to be interpreted as
		//                         *unsigned* values (since 0x00 should be dark and 0xFF should be bright).
		//  surface->pixels:    The actual data.  As above, SDL's array of bytes.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pSurface->w, pSurface->h, 0, pixelFormat, GL_UNSIGNED_BYTE, pSurface->pixels);

		// Set the minification and magnification filters.  In this case, when the texture is minified (i.e., the texture's pixels (texels) are
		// *smaller* than the screen pixels you're seeing them on, linearly filter them (i.e. blend them together).  This blends four texels for
		// each sample--which is not very much.  Mipmapping can give better results.  Find a texturing tutorial that discusses these issues
		// further.  Conversely, when the texture is magnified (i.e., the texture's texels are *larger* than the screen pixels you're seeing
		// them on), linearly filter them.  Qualitatively, this causes "blown up" (overmagnified) textures to look blurry instead of blocky.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

#pragma endregion

#pragma region Sprite Sheet

	USpriteSheet::USpriteSheet(const UTexture& texture, unsigned int numberOfRows, unsigned int numberOfColumns, unsigned int numberOfEmptyPixeBetweenColumns, unsigned int numberOfEmptyPixeBetweenRows,  const FVector2f& scaleInScreen) :
		m_Texture(texture),
		m_NumberOfRows(numberOfRows),
		m_NumberOfColumns(numberOfColumns),
		m_NumberOfEmptyPixelsBetweenColumns(numberOfEmptyPixeBetweenColumns),
		m_NumberOfEmptyPixelsBetweenRows(numberOfEmptyPixeBetweenRows)
	{
		m_Texture.SetScreenPosition(FVector2f{0,0});
		m_Texture.SetScale(scaleInScreen);
		m_CellWidth = m_Texture.GetTextureOriginalWidth() / m_NumberOfColumns;
		m_CellHeight = m_Texture.GetTextureOriginalHeight() / m_NumberOfRows ;
	}
	
	
	UTexture& USpriteSheet::GetFrame(size_t rowIndex, size_t columnIndex)
	{
		FrameTexture(rowIndex, columnIndex);
		return m_Texture;
	}
	void USpriteSheet::SetFrame(size_t rowIndex, size_t columnIndex) {
		FrameTexture(rowIndex, columnIndex);
	}

	const UTexture& USpriteSheet::GetConstFrame()const
	{
		return m_Texture;
	}
	
	void USpriteSheet::SetScreenPosition(const FVector2f& position)
	{
		m_Texture.SetScreenPosition(position);
	}
	
	void USpriteSheet::SetScale(const FVector2f& scaleInScreen)
	{
		m_Texture.SetScale(scaleInScreen);
	}

	const FVector2f& USpriteSheet::GetPositionInScreen()const
	{
		return m_Texture.GetPositionInScreen();
	}

	const FVector2f& USpriteSheet::GetScaleInScreen() const
	{
		return m_Texture.GetScaleInScreen();
	}

	int USpriteSheet::GetOriginalCellWidth() const
	{
		return m_CellWidth - m_NumberOfEmptyPixelsBetweenColumns;
	}

	int USpriteSheet::GetOriginalCellHeight() const
	{
		return m_CellHeight - m_NumberOfEmptyPixelsBetweenRows;
	}

	float USpriteSheet::GetScaledCellWidth() const
	{
		return (m_CellWidth - m_NumberOfEmptyPixelsBetweenColumns) * m_Texture.GetScaleInScreen().X;
	}

	float USpriteSheet::GetScaledCellHeight() const
	{
		return (m_CellHeight - m_NumberOfEmptyPixelsBetweenRows) * m_Texture.GetScaleInScreen().Y;
	}
	
	bool USpriteSheet::IsValid() const
	{
		return m_Texture.IsValid();
	}

	void USpriteSheet::FrameTexture(size_t rowIndex, size_t columnIndex)
	{
		//++rowIndex;//because without the increment it would be top instead of bottom
		m_Texture.SetTextureView(FRectf{
			(columnIndex * m_CellWidth) + (m_NumberOfEmptyPixelsBetweenColumns / 2.f) ,
			(rowIndex * m_CellHeight) + (m_NumberOfEmptyPixelsBetweenRows / 2.f),
			float(m_CellWidth - m_NumberOfEmptyPixelsBetweenColumns),
			float(m_CellHeight - m_NumberOfEmptyPixelsBetweenRows) });
	}
	
#pragma endregion

#pragma region TextureData

	UTextureData::UTextureData(const UTexture& texture) :
		m_TextureWidth(texture.m_TextureInfo.TextureWidth),
		m_TextureHeight(texture.m_TextureInfo.TextureHeight)
	{
		if (texture.IsValid())
		{
			m_IsValid = true;
			// Bind the texture
			glBindTexture(GL_TEXTURE_2D, texture.m_TextureInfo.Id);

			// Allocate memory to store the pixel data
			m_Pixels = new GLubyte[m_TextureWidth * m_TextureHeight * m_RGBA_Num]; // Assuming RGBA format

			// Retrieve the pixel data from the texture
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Pixels);
		}

	}
	UTextureData::~UTextureData()
	{
		delete[] m_Pixels;
	}
	UTextureData::UTextureData(const UTextureData& other) :
		m_TextureWidth(other.m_TextureWidth),
		m_TextureHeight(other.m_TextureHeight),
		m_IsValid(other.m_IsValid)
	{
		size_t totalArraySize{ other.m_TextureWidth * other.m_TextureHeight * m_RGBA_Num };// Assuming RGBA format

		if (totalArraySize > 0)
		{
			m_Pixels = new GLubyte[totalArraySize]; 

			for (size_t i = 0; i < totalArraySize; i++)
			{
				m_Pixels[i] = other.m_Pixels[i];
			}
		}

	}
	UTextureData::UTextureData(UTextureData&& other) noexcept :
		m_Pixels(other.m_Pixels),
		m_TextureWidth(other.m_TextureWidth),
		m_TextureHeight(other.m_TextureHeight),
		m_IsValid(other.m_IsValid)
	{
		other.m_Pixels = nullptr;
	}
	UTextureData& UTextureData::operator=(const UTextureData& other)
	{

		size_t totalArraySize{ other.m_TextureWidth * other.m_TextureHeight * m_RGBA_Num };// Assuming RGBA format

		if ((this != &other) && totalArraySize > 0)
		{
			delete[] m_Pixels;
			m_Pixels = new GLubyte[totalArraySize];

			for (size_t i = 0; i < totalArraySize; i++)
			{
				m_Pixels[i] = other.m_Pixels[i];
			}
			m_TextureWidth = other.m_TextureWidth;
			m_TextureHeight = other.m_TextureHeight;
			m_IsValid = other.m_IsValid;
		}

		return *this;
	}
	UTextureData& UTextureData::operator=(UTextureData&& other) noexcept
	{
		if ((this != &other) && 
			other.m_TextureWidth > 0 && 
			other.m_TextureHeight > 0) 
		{

			delete[] m_Pixels;
			m_Pixels = other.m_Pixels;
			m_TextureWidth = other.m_TextureWidth;
			m_TextureHeight = other.m_TextureHeight;
			m_IsValid = other.m_IsValid;

			other.m_Pixels = nullptr;
		}

		return *this;
	}
	size_t UTextureData::NumberOfRows() const
	{
		return m_TextureHeight;
	}
	size_t UTextureData::NumberOfColumns() const
	{
		return m_TextureWidth;
	}
	size_t UTextureData::NumberOfPixels() const
	{
		return m_TextureHeight * m_TextureWidth;
	}
	FColor4i UTextureData::PixelAt(size_t column, size_t row)const
	{
		FColor4i color{};
		color.SetR(m_Pixels[(((m_TextureWidth * row) + column) * m_RGBA_Num)]	 );
		color.SetG(m_Pixels[(((m_TextureWidth * row) + column) * m_RGBA_Num) + 1]);
		color.SetB(m_Pixels[(((m_TextureWidth * row) + column) * m_RGBA_Num) + 2]);
		color.SetA(m_Pixels[(((m_TextureWidth * row) + column) * m_RGBA_Num) + 3]);

		return color;
	}
	FColor4i UTextureData::PixelAt(size_t pixelIndex)const
	{
		FColor4i color{};
		color.SetR(m_Pixels[(pixelIndex * m_RGBA_Num)]	  );
		color.SetG(m_Pixels[(pixelIndex * m_RGBA_Num) + 1]);
		color.SetB(m_Pixels[(pixelIndex * m_RGBA_Num) + 2]);
		color.SetA(m_Pixels[(pixelIndex * m_RGBA_Num) + 3]);

		return color;
	}
	bool UTextureData::PixelIsInBounds(int x, int y) const
	{
		return x > 0 && x < m_TextureWidth && y > 0 && y < m_TextureHeight;
	}
	std::vector<FRectf> UTextureData::MakeRectsFromPixelsOfColor(const FColor4i& color, float xSize, float ySize)const
	{
		std::vector<FRectf> rects{};
		rects.reserve(NumberOfPixels()/4);//aprox

		FRectf rect{};
		rect.Height = ySize;
		rect.Width = xSize;

		for (size_t rowIdx = 0; rowIdx < NumberOfRows(); rowIdx++)
		{
			for (size_t colIdx = 0; colIdx < NumberOfColumns(); colIdx++)
			{
				if (PixelAt(colIdx, rowIdx) == color)
				{
					rect.Top = rowIdx * ySize;
					rect.Left = colIdx * xSize;
					
					rects.push_back(rect);
				}
			}
		}

		return rects;
	}
	bool UTextureData::IsValid() const
	{
		return m_IsValid;
	}

#pragma endregion

}