#pragma warning(disable:4464)
#pragma warning(disable:5259)
#include "Util.hpp"


#include "../File/FileStream.hpp"

const Util::SaveData* Util::SingletonBase<Util::SaveData>::s_Singleton = nullptr;
namespace Util {
	bool SaveData::Load(void) noexcept {
		this->m_data.clear();
		if (!File::IsFileExist("Save/new.svf")) {
			return false;
		}
		File::InputFileStream FileStream("Save/new.svf");
		while (true) {
			if (FileStream.ComeEof()) { break; }
			std::string ALL = FileStream.SeekLineAndGetStr();
			if (ALL == "") {
				continue;
			}
			std::string LEFT = File::InputFileStream::getleft(ALL, "=");
			std::string RIGHT = File::InputFileStream::getright(ALL, "=");
			this->m_data.emplace_back(std::make_pair(LEFT, RIGHT));
		}
		return true;
	}
}