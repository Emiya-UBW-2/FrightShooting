#pragma warning(disable:4464)
#include "Util.hpp"


#include "../File/FileStream.hpp"

const Util::SaveData* Util::SingletonBase<Util::SaveData>::m_Singleton = nullptr;
namespace Util {
	bool SaveData::Load(void) noexcept {
		this->m_data.clear();
		if (!File::IsFileExist("Save/new.svf")) {
			return false;
		}
		File::InputFileStream FileStream("Save/new.svf");
		while (true) {
			if (FileStream.ComeEof()) { break; }
			auto ALL = FileStream.SeekLineAndGetStr();
			if (ALL == "") {
				continue;
			}
			auto LEFT = File::InputFileStream::getleft(ALL, "=");
			auto RIGHT = File::InputFileStream::getright(ALL, "=");
			this->m_data.emplace_back(std::make_pair(LEFT, std::stoi(RIGHT)));
		}
		return true;
	}
}