#pragma warning(disable:4464)
#pragma warning(disable:5259)

#include "Localize.hpp"

#include "Enum.hpp"
#include "../File/FileStream.hpp"
#include "Option.hpp"


const Util::LocalizePool* Util::SingletonBase<Util::LocalizePool>::s_Singleton = nullptr;

namespace Util {
	LocalizePool::LocalizePool(void) noexcept {
		Load();
	}
	void LocalizePool::Load(void) noexcept {
		Dispose();
		auto* pOption = Util::OptionParam::Instance();
		std::string Path = "data/Localize/";
		Path += LanguageStr[pOption->GetParam(pOption->GetOptionType(Util::OptionType::Language))->GetSelect()];
		Path += ".txt";
		this->havehandle.reserve(256);
		File::InputFileStream FileStream(Path.c_str());
		while (true) {
			if (FileStream.ComeEof()) { break; }
			std::string ALL = FileStream.SeekLineAndGetStr();
			if (ALL == "") { continue; }
			//=の右側の文字を区切りとして識別する
			std::string LEFT = File::InputFileStream::getleft(ALL, "=");
			std::string RIGHT = File::InputFileStream::getright(ALL, "=");
			LocalizeID ID = (LocalizeID)(std::stoi(LEFT));
			bool IsHit = false;
			for (auto& h : this->havehandle) {
				if (h.m_ID == ID) {
					// 改行して同じテキストとする
					h.m_Str += "\n";
					h.m_Str += RIGHT;
					IsHit = true;
					break;
				}
			}
			if (!IsHit) {
				this->havehandle.emplace_back(ID, RIGHT);
			}
		}
	}
}