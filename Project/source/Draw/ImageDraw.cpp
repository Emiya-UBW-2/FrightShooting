#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#pragma warning(disable:5259)
#include "ImageDraw.hpp"

const Draw::GraphPool* Util::SingletonBase<Draw::GraphPool>::m_Singleton = nullptr;

namespace Draw {
	Graphhave::Graphhave(std::string_view FilePath) noexcept {
		this->m_FilePath = FilePath;
		this->m_Handle.Load(this->m_FilePath);
	}
}