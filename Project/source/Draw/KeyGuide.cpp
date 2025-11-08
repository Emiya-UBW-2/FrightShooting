#pragma warning(disable:4464)
#pragma warning(disable:5259)
#include "KeyGuide.hpp"

#include "FontDraw.hpp"
#include "MainDraw.hpp"

#include "../File/FileStream.hpp"

const DXLibRef::KeyGuide* Util::SingletonBase<DXLibRef::KeyGuide>::m_Singleton = nullptr;

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	int KeyGuide::KeyGuideGraph::GetDrawSize(void) const noexcept { return (xsize * 24 / ysize) + 3; }
	void KeyGuide::KeyGuideGraph::Draw(int x, int y) const noexcept {
		GuideImg.DrawExtendGraph(x, y, x + (xsize * 24 / ysize), y + 24, true);
	}
	// 
	int KeyGuide::KeyGuideOnce::GetDrawSize(void) const noexcept {
		int ofs = (this->m_GuideGraph) ? this->m_GuideGraph->GetDrawSize() : 0;
		if (GuideString != "") {
			ofs += Draw::FontPool::Instance()->Get(Draw::FontType::MS_Gothic, LineHeight, 3)->GetStringWidth(Util::SjistoUTF8(GuideString)) + 12;
		}
		return ofs;
	}
	void KeyGuide::KeyGuideOnce::Draw(int x, int y) const noexcept {
		int ofs = 0;
		if (this->m_GuideGraph) {
			this->m_GuideGraph->Draw(x, y);
			ofs = this->m_GuideGraph->GetDrawSize();
		}
		if (GuideString != "") {
			Draw::FontPool::Instance()->Get(Draw::FontType::MS_Gothic, LineHeight, 3)->DrawString(
				Draw::FontXCenter::LEFT, Draw::FontYCenter::MIDDLE,
				x + ofs, y + 24 / 2,
				ColorPalette::White, ColorPalette::Black, Util::SjistoUTF8(GuideString));
		}
	}
	// 
	KeyGuide::KeyGuide(void) noexcept {
		SetGuideFlip();
		this->m_GuideBaseImage.Load("CommonData/key/OutputFont.png");
		{
			int count = 0;
			File::InputFileStream FileStream("CommonData/key/OutputFont.psf");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				auto ALL = FileStream.SeekLineAndGetStr();
				if (ALL == "") { continue; }
				//=の右側の文字をカンマ区切りとして識別する
				auto RIGHT = File::InputFileStream::getright(ALL, "=");
				std::array<int, 4> Args{};
				int now = 0;
				while (true) {
					auto div = RIGHT.find(",");
					if (div != std::string::npos) {
						Args[static_cast<size_t>(now)] = std::stoi(RIGHT.substr(0, div));
						++now;
						RIGHT = RIGHT.substr(div + 1);
					}
					else {
						Args[static_cast<size_t>(now)] = std::stoi(RIGHT);
						++now;
						break;
					}
				}
				//得た情報をもとに分割した画像を得る
				this->m_DerivationGuideImage.emplace_back(std::make_shared<KeyGuideGraph>());
				this->m_DerivationGuideImage.back()->AddGuide(Args[0], Args[1], Args[2], Args[3], this->m_GuideBaseImage);
				++count;
			}
		}
	}
	void KeyGuide::ChangeGuide(std::function<void()>Guide_Pad) noexcept {
		if (this->m_IsFlipGuide) {
			this->m_IsFlipGuide = false;
			Dispose();
			//AddGuide(GetPADStoOffset(Util::EnumBattle::W), "");
			//AddGuide(GetPADStoOffset(Util::EnumBattle::A), "aaaa");
			// ガイド
			Guide_Pad();
		}
	}
	void KeyGuide::Draw(void) const noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		int x = 32;
		int y = DrawerMngr->GetDispHeight() - (21 + 16);
		for (const auto& k : this->m_Key) {
			k->Draw(x, y);
			x += k->GetDrawSize();
			//次の行へ移行
			if (x > DrawerMngr->GetDispWidth() / 2) {
				x = 32;
				y -= 28;
			}
		}
	}
};
