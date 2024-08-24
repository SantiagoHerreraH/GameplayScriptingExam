#pragma once

#include "../Draw/UTexture.h"

namespace Pillar {

	struct FAnimationFrameInfo {
		UTexture Texture;
		unsigned int NumberOfFrames;
		float TimeToGoToTheNextFrame;
	};

	class UAnimation final{

	public:

		void Draw()const;
		void Update(float deltaSeconds);

		void CanLoop(bool canLoop); //default is true
		void SetFramesPerSecond(unsigned int numberOfFrames);
		void AddFrame(const UTexture& frame, unsigned int numberOfFrames);
		void ClearFrames();

		int LoopNumber();

		void Reset();
		void Play();
		void Pause();

	private:
		int m_NumberOfLoops{ 0 };
		bool m_CanLoop{ true };
		bool m_IsPlaying{ true };
		double m_CurrentTime{0};
		size_t m_CurrentFrameIndex{ 0 };
		float m_SecondsPerFrame{ 1/12.f };

		std::vector<FAnimationFrameInfo> m_Frames;
	};
}