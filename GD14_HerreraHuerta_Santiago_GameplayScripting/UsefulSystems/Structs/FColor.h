#pragma once

#include <vector>
#include "../Math/STools.h"

namespace Pillar{

	//range is 0 to 255
	struct FColor4i {
		FColor4i() {}
		explicit FColor4i(int r, int g, int b, int a)
		{
			SetR(r);
			SetG(g);
			SetB(b);
			SetA(a);
		}

		int GetR()const{return m_R;}
		int GetG()const{return m_G;}
		int GetB()const{return m_B;}
		int GetA()const{return m_A;}

		static const size_t NUMBER_OF_VARIABLES{ 4 };
		static const size_t MAXVALUE{ 255 };

		void SetR(size_t value) {  m_R = value > MAXVALUE ? MAXVALUE : value < 0 ? 0 :  value; }
		void SetG(size_t value) {  m_G = value > MAXVALUE ? MAXVALUE : value < 0 ? 0 :  value; }
		void SetB(size_t value) {  m_B = value > MAXVALUE ? MAXVALUE : value < 0 ? 0 :  value; }
		void SetA(size_t value) {  m_A = value > MAXVALUE ? MAXVALUE : value < 0 ? 0 :  value; }

		void MakeRandom(bool randomizeAlpha = false, int alphaValue = MAXVALUE) {
			m_R = STools::GetRandomBetweenRange(0, MAXVALUE);
			m_G = STools::GetRandomBetweenRange(0, MAXVALUE);
			m_B = STools::GetRandomBetweenRange(0, MAXVALUE);
			if (randomizeAlpha)
			{
				m_A = STools::GetRandomBetweenRange(0, MAXVALUE);
			}
			else
			{
				m_A = alphaValue;
			}
		}

	private:
		
		int m_R{0};
		int m_G{0};
		int m_B{0};
		int m_A{0};

	};

	inline bool operator==(const FColor4i& one, const FColor4i& two) {
		return 
			(one.GetR() == two.GetR()) &&
			(one.GetG() == two.GetG()) && 
			(one.GetB() == two.GetB()) && 
			(one.GetA() == two.GetA());
	}

}

template <>
struct std::hash<Pillar::FColor4i>
{
	std::size_t operator()(const Pillar::FColor4i& color) const
	{
		return color.GetR() + (color.GetG() * color.MAXVALUE) + (color.GetB() * color.MAXVALUE * 2) + (color.GetA() * color.MAXVALUE * 3);
	}
};



