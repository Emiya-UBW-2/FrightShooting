#pragma once
#include "Util.hpp"
#pragma warning( push, 3 )
#include <string>
#pragma warning( pop )

namespace Util {
	class LocalizePool : public SingletonBase<LocalizePool> {
	private:
		friend class SingletonBase<LocalizePool>;
	public:
		typedef int LocalizeID;
	private:
		struct LocalizeStr {
			LocalizeID	m_ID{ 0 };
			char		padding[4]{};
			std::string	m_Str{};
		public:
			LocalizeStr(void) noexcept {}
			LocalizeStr(LocalizeID ID, std::string_view Str) noexcept {
				this->m_ID = ID;
				this->m_Str = Str;
			}
		};

		std::vector<LocalizeStr> havehandle;
	private:
		LocalizePool(void) noexcept;
		LocalizePool(const LocalizePool&) = delete;
		LocalizePool(LocalizePool&&) = delete;
		LocalizePool& operator=(const LocalizePool&) = delete;
		LocalizePool& operator=(LocalizePool&&) = delete;
	public:
		void Load(void) noexcept;
		void Dispose(void) noexcept {
			this->havehandle.clear();
			this->havehandle.shrink_to_fit();
		}
	public:
		std::string Get(LocalizeID type) noexcept {
			for (auto& s : this->havehandle) {
				if (s.m_ID == type) {
					return s.m_Str;
				}
			}
			return "";
		}
	};
}
