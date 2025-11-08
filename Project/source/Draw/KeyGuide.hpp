#pragma once
#pragma warning(disable:4464)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#include "ImageDraw.hpp"

#include "../Util/Util.hpp"
#include "../Util/Key.hpp"

#pragma warning( push, 3 )
#include <array>
#include <vector>
#include <functional>
#pragma warning( pop )

// リサイズ
constexpr auto LineHeight = (18);

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// キーガイド
	// --------------------------------------------------------------------------------------------------
	class KeyGuide : public Util::SingletonBase<KeyGuide> {
	private:
		friend class Util::SingletonBase<KeyGuide>;
	private:
		// コンストラクタ
		KeyGuide(void) noexcept;
		// コピーしてはいけないので通常のコンストラクタ以外をすべてdelete
		KeyGuide(const KeyGuide&) = delete;
		KeyGuide(KeyGuide&&) = delete;
		KeyGuide& operator=(const KeyGuide&) = delete;
		KeyGuide& operator=(KeyGuide&&) = delete;
		virtual ~KeyGuide(void) noexcept {}
	private:
		class KeyGuideGraph {
			int xsize{ 0 }, ysize{ 0 };
			Draw::GraphHandle GuideImg;
		public:
			KeyGuideGraph(void) noexcept {}
			KeyGuideGraph(const KeyGuideGraph&) = delete;
			KeyGuideGraph(KeyGuideGraph&&) = delete;
			KeyGuideGraph& operator=(const KeyGuideGraph&) = delete;
			KeyGuideGraph& operator=(KeyGuideGraph&&) = delete;

			~KeyGuideGraph(void) noexcept {}
		public:
			void AddGuide(int x, int y, int xs, int ys, const Draw::GraphHandle& baseImage) noexcept {
				GuideImg.DerivationGraph(x, y, xs, ys, baseImage);
				xsize = xs;
				ysize = ys;
			}
			void Dispose(void) noexcept { GuideImg.Dispose(); }
			int GetDrawSize(void) const noexcept;
			void Draw(int x, int y) const noexcept;
		};
		class KeyGuideOnce {
			std::shared_ptr<KeyGuideGraph>	m_GuideGraph;
			std::string GuideString;
		public:
			KeyGuideOnce(void) noexcept {}
			KeyGuideOnce(const KeyGuideOnce&) = delete;
			KeyGuideOnce(KeyGuideOnce&&) = delete;
			KeyGuideOnce& operator=(const KeyGuideOnce&) = delete;
			KeyGuideOnce& operator=(KeyGuideOnce&&) = delete;

			~KeyGuideOnce(void) noexcept {}
		public:
			void AddGuide(const std::shared_ptr<KeyGuideGraph>& pGuide, const std::string& GuideStr) noexcept {
				this->m_GuideGraph = pGuide;
				GuideString = GuideStr;
			}
			void Dispose(void) noexcept {
				if (this->m_GuideGraph) {
					this->m_GuideGraph.reset();
				}
				GuideString = "";
			}
			int GetDrawSize(void) const noexcept;
			void Draw(int x, int y) const noexcept;
		};
	private:
		bool													m_IsFlipGuide{ true };				// ガイドの更新フラグ
		char		padding[7]{};
		Draw::GraphHandle										m_GuideBaseImage;					// 分割前の画像
		std::vector<std::shared_ptr<KeyGuideGraph>>				m_DerivationGuideImage;				// 分割後の画像
		std::vector<std::unique_ptr<KeyGuideOnce>>				m_Key;								// ガイド
	public:
		static int GetPADStoOffset(Util::EnumMenu PAD) noexcept {
			auto* KeyMngr = Util::KeyParam::Instance();
			return static_cast<int>(KeyMngr->GetKeyAssign(PAD, 0));
		}
		static int GetPADStoOffset(Util::EnumBattle PAD) noexcept {
			auto* KeyMngr = Util::KeyParam::Instance();
			return static_cast<int>(KeyMngr->GetKeyAssign(PAD, 0));
		}
	public:
		void SetGuideFlip(void) noexcept { this->m_IsFlipGuide = true; }
		void ChangeGuide(std::function<void()>Guide_Pad) noexcept;
		void AddGuide(int graphOffset, const std::string& GuideStr) noexcept {
			if (graphOffset == static_cast<int>(Util::EnumInput::Max)) { return; }
			this->m_Key.emplace_back(std::make_unique<KeyGuideOnce>());
			this->m_Key.back()->AddGuide((graphOffset != InvalidID) ? this->m_DerivationGuideImage[static_cast<size_t>(graphOffset)] : nullptr, GuideStr);
		}
		void Dispose(void) noexcept {
			for (auto& k : this->m_Key) {
				k->Dispose();
				k.reset();
			}
			this->m_Key.clear();
		}
	public:
		//ガイド表示の描画
		void Draw(void) const noexcept;
		//キー単体の描画
		void DrawButton(int x, int y, int graphOffset) const noexcept { this->m_DerivationGuideImage[static_cast<size_t>(graphOffset)]->Draw(x, y); }
		int GetDrawSize(int graphOffset) const noexcept { return this->m_DerivationGuideImage[static_cast<size_t>(graphOffset)]->GetDrawSize(); }
	};
}
