#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:5259)
#include "Util.hpp"
#include "../File/FileStream.hpp"

#pragma warning( push, 3 )
#include <array>
#pragma warning( pop )

namespace Util {
	struct VRAnim {
		int ID{};
		Util::Matrix3x3	m_RightRot;
		Util::Matrix3x3	m_LeftRot;
		Util::VECTOR3D	m_LeftHandPos;
		Util::VECTOR3D	m_RightHandPos;
	public:
		static VRAnim LerpAnim(const VRAnim& A, const VRAnim& B, float Per) noexcept {
			VRAnim Answer;
			Answer.m_RightRot = Util::Lerp(A.m_RightRot, B.m_RightRot, Per);
			Answer.m_LeftRot = Util::Lerp(A.m_LeftRot, B.m_LeftRot, Per);
			Answer.m_LeftHandPos = Util::Lerp(A.m_LeftHandPos, B.m_LeftHandPos, Per);
			Answer.m_RightHandPos = Util::Lerp(A.m_RightHandPos, B.m_RightHandPos, Per);
			return Answer;
		}
	};

	class HandAnimData {
		std::vector<VRAnim>	m_VRAnims{};
		std::string			m_Path{};
		float				m_AnimTime{};
		float				m_Time{};
		float				m_Speed{};
		char		padding[4]{};
	public:
		const std::string& GetPath(void) const noexcept { return this->m_Path; }
		const VRAnim& GetAnim(void) const noexcept { return this->m_VRAnims.at(std::clamp<size_t>(static_cast<size_t>(this->m_Time), 0, static_cast<size_t>(this->m_VRAnims.size() - 1))); }

		void			SetSpeed(float Speed) noexcept { this->m_Speed = Speed; }
		void			GoTimeStart(void) noexcept { this->m_Time = 0.f; }
	public:
		void			Init(const char* Path) noexcept {
			this->m_Path = Path;
			this->m_VRAnims.resize(static_cast<size_t>(60 * 30));
			{
				File::InputFileStream FileStream(Path);
				int NowSearch = 0;
				while (true) {
					if (FileStream.ComeEof()) { break; }
					auto ALL = FileStream.SeekLineAndGetStr();
					if (ALL == "") { continue; }
					auto LEFT = File::InputFileStream::getleft(ALL, "=");
					auto RIGHT = File::InputFileStream::getright(ALL, "=");

					std::array<float, 3> Args{};
					int now = 0;
					while (true) {
						auto div = RIGHT.find(",");
						if (div != std::string::npos) {
							Args[static_cast<size_t>(now)] = std::stof(RIGHT.substr(0, div));
							++now;
							RIGHT = RIGHT.substr(div + 1);
						}
						else {
							Args[static_cast<size_t>(now)] = std::stof(RIGHT);
							++now;
							break;
						}
					}

					if (LEFT == "Frame") {
						NowSearch = std::stoi(RIGHT);
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).ID = std::stoi(RIGHT);
					}
					else if (LEFT == "RightRot0") {
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_RightRot.get().m[0][0] = Args[0];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_RightRot.get().m[0][1] = Args[1];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_RightRot.get().m[0][2] = Args[2];
					}
					else if (LEFT == "RightRot1") {
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_RightRot.get().m[1][0] = Args[0];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_RightRot.get().m[1][1] = Args[1];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_RightRot.get().m[1][2] = Args[2];
					}
					else if (LEFT == "RightRot2") {
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_RightRot.get().m[2][0] = Args[0];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_RightRot.get().m[2][1] = Args[1];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_RightRot.get().m[2][2] = Args[2];
					}
					else if (LEFT == "RightPos") {
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_RightHandPos.x = Args[0];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_RightHandPos.y = Args[1];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_RightHandPos.z = Args[2];
					}
					else if (LEFT == "LeftRot0") {
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_LeftRot.get().m[0][0] = Args[0];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_LeftRot.get().m[0][1] = Args[1];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_LeftRot.get().m[0][2] = Args[2];
					}
					else if (LEFT == "LeftRot1") {
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_LeftRot.get().m[1][0] = Args[0];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_LeftRot.get().m[1][1] = Args[1];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_LeftRot.get().m[1][2] = Args[2];
					}
					else if (LEFT == "LeftRot2") {
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_LeftRot.get().m[2][0] = Args[0];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_LeftRot.get().m[2][1] = Args[1];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_LeftRot.get().m[2][2] = Args[2];
					}
					else if (LEFT == "LeftPos") {
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_LeftHandPos.x = Args[0];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_LeftHandPos.y = Args[1];
						this->m_VRAnims.at(static_cast<size_t>(NowSearch)).m_LeftHandPos.z = Args[2];
					}
				}
				this->m_AnimTime = static_cast<float>(NowSearch);
			}
			this->m_Speed = 1.f;
		}
		void			Update(void) noexcept {
			this->m_Time += 30.f * DeltaTime * this->m_Speed;
			if (this->m_Time >= this->m_AnimTime) {
				this->m_Time -= this->m_AnimTime;
			}
		}
		void			Dispose(void) noexcept {
			this->m_VRAnims.clear();
			this->m_VRAnims.shrink_to_fit();
		}
	};

	typedef int64_t HandAnimID;

	class HandAnimPool : public SingletonBase<HandAnimPool> {
	private:
		friend class SingletonBase<HandAnimPool>;
	private:
		std::vector<HandAnimData>	m_Anim{};
	private:
		HandAnimPool(void) noexcept { Init(); }
		HandAnimPool(const HandAnimPool&) = delete;
		HandAnimPool(HandAnimPool&&) = delete;
		HandAnimPool& operator=(const HandAnimPool&) = delete;
		HandAnimPool& operator=(HandAnimPool&&) = delete;
		virtual ~HandAnimPool(void) noexcept { Dispose(); }
	public:
		void	SetAnimSpeed(HandAnimID ID, float Speed) noexcept {
			this->m_Anim.at(static_cast<size_t>(ID)).SetSpeed(Speed);
		}
		void			GoTimeStart(HandAnimID ID) noexcept {
			this->m_Anim.at(static_cast<size_t>(ID)).GoTimeStart();
		}

		const HandAnimData& GetAnim(HandAnimID ID) const noexcept { return this->m_Anim.at(static_cast<size_t>(ID)); }
		HandAnimID				Add(const char* Path) noexcept {
			auto ID = GetID(Path);
			if (ID != -1) {
				return ID;
			}
			this->m_Anim.emplace_back();
			this->m_Anim.back().Init(Path);
			return static_cast<HandAnimID>(this->m_Anim.size() - 1);
		}
		HandAnimID				GetID(const char* Path) noexcept {
			for (auto& a : this->m_Anim) {
				if (a.GetPath() == Path) {
					return static_cast<HandAnimID>(&a - &m_Anim.front());
				}
			}
			return -1;
		}
	public:
		void				Init(void) noexcept {
			this->m_Anim.reserve(10);
		}
		void				Update(void) noexcept {
			for (auto& a : this->m_Anim) {
				a.Update();
			}
		}
		void				Dispose(void) noexcept {
			for (auto& a : this->m_Anim) {
				a.Dispose();
			}
		}
	};
}
