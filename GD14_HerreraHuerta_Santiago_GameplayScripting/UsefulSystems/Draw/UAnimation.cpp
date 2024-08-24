

#include "../ExternalIncludes.h"

#include "../Draw/UAnimation.h"
#include "../Math/STools.h"
#include <cassert>

namespace Pillar {

#pragma region Animation

	void UAnimation::Play()
	{
		m_IsPlaying = true;
	}
	void UAnimation::Pause()
	{
		m_IsPlaying = false;
	}
	void UAnimation::CanLoop(bool canLoop)
	{
		m_CanLoop = canLoop;
	}
	void UAnimation::SetFramesPerSecond(unsigned int numberOfFrames)
	{
		m_SecondsPerFrame = 1.f / float(numberOfFrames);

		float sumedTime{ 0 };
		for (size_t i = 0; i < m_Frames.size(); i++)
		{
			m_Frames.at(i).TimeToGoToTheNextFrame = sumedTime + (m_Frames.at(i).NumberOfFrames * m_SecondsPerFrame);
			sumedTime = m_Frames.at(i).TimeToGoToTheNextFrame;
		}

		Reset();
	}
	void UAnimation::AddFrame(const UTexture& frame, unsigned int numberOfFrames)
	{
		float sumedTime = m_Frames.empty() ? 0 : m_Frames.at(m_Frames.size() - 1).TimeToGoToTheNextFrame;

		m_Frames.emplace_back(FAnimationFrameInfo{ frame, numberOfFrames , 0});

		m_Frames.at(m_Frames.size() - 1).TimeToGoToTheNextFrame = sumedTime + (m_Frames.at(m_Frames.size() - 1).NumberOfFrames * m_SecondsPerFrame);
	}

	void UAnimation::ClearFrames()
	{
		m_NumberOfLoops = 0;
		m_IsPlaying = false;
		m_CurrentTime = 0 ;
		m_CurrentFrameIndex = 0;
		m_SecondsPerFrame = 1;
		m_Frames.clear();
	}

	int UAnimation::LoopNumber()
	{
		return m_NumberOfLoops;
	}

	void UAnimation::Reset()
	{
		m_CurrentTime = 0;
		m_NumberOfLoops = 0;
		m_CurrentFrameIndex = 0;
	}

	void UAnimation::Draw() const
	{
		m_Frames.at(m_CurrentFrameIndex).Texture.Draw();
	}
	void UAnimation::Update(float deltaSeconds)
	{
		if (m_IsPlaying)
		{
			m_CurrentTime += deltaSeconds;
			//desired time to change to next frame

			if (m_CurrentTime >= m_Frames.at(m_CurrentFrameIndex).TimeToGoToTheNextFrame)
			{
				
				if (m_CanLoop)
				{
					m_NumberOfLoops = m_CurrentFrameIndex >= m_Frames.size() - 1 ? m_NumberOfLoops + 1 : m_NumberOfLoops;

					m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_Frames.size();
				}
				else if(!m_CanLoop && !m_Frames.empty())
				{

					m_NumberOfLoops = (m_CurrentFrameIndex >= m_Frames.size() - 1) && m_NumberOfLoops < 1 ? m_NumberOfLoops + 1 : m_NumberOfLoops;

					m_CurrentFrameIndex = STools::Clamp((m_CurrentFrameIndex + 1), 0, m_Frames.size() - 1);
				}

				m_CurrentTime = m_CurrentFrameIndex == 0 ? 0 : m_CurrentTime;

			}
		}
	}

#pragma endregion



}

