#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../Util/Util.hpp"
#include "Character.hpp"

class PlayerManager : public Util::SingletonBase<PlayerManager> {
private:
	friend class Util::SingletonBase<PlayerManager>;
private:
	std::vector<std::shared_ptr<PlaneCommon>>	m_Plane;
	std::vector<std::shared_ptr<Target>>	m_Target;

	int										m_Score{ 0 };
	float									m_Timer{ 0.f };
private:
	PlayerManager(void) noexcept {}
	PlayerManager(const PlayerManager&) = delete;
	PlayerManager(PlayerManager&&) = delete;
	PlayerManager& operator=(const PlayerManager&) = delete;
	PlayerManager& operator=(PlayerManager&&) = delete;
	virtual ~PlayerManager(void) noexcept { Dispose(); }
public:
	void Load(void) noexcept {
		ObjectManager::Instance()->LoadModel("data/model/Sopwith/");
		this->m_Plane.resize(1);

		ObjectManager::Instance()->LoadModel("data/model/Target/");
		this->m_Target.resize(10);
	}
	void Init(void) noexcept {
		this->m_Plane.at(0) = std::make_shared<Plane>();
		ObjectManager::Instance()->InitObject(this->m_Plane.at(0), this->m_Plane.at(0), "data/model/Sopwith/");
		this->m_Plane.at(0)->SetPlayerID(0);
		//this->m_Plane.at(0)->SetPos(BackGround::Instance()->GetWorldPos(m.m_pos));

		for (size_t loop = 1; loop < this->m_Plane.size(); ++loop) {
			this->m_Plane.at(loop) = std::make_shared<EnemyPlane>();
			ObjectManager::Instance()->InitObject(this->m_Plane.at(loop), this->m_Plane.at(loop), "data/model/Sopwith/");
			this->m_Plane.at(loop)->SetPlayerID(static_cast<int>(loop));
		}

		for (auto& m : this->m_Target) {
			m = std::make_shared<Target>();
			ObjectManager::Instance()->InitObject(m, m, "data/model/Target/");
			m->SetMatrix(Util::Matrix4x4::Mtrans(Util::VECTOR3D::vget(
				GetRandf(1500.f * Scale3DRate),
				GetRandf(500.f * Scale3DRate), 
				GetRandf(1500.f * Scale3DRate)
			)));
		}
		m_Score = 0;
	}
	void Dispose(void) noexcept {
		for (auto& m : this->m_Plane) {
			m.reset();
		}
		this->m_Plane.clear();

		for (auto& m : this->m_Target) {
			m.reset();
		}
		this->m_Target.clear();
	}
public:
	const auto& GetPlane(void) const noexcept { return this->m_Plane; }
	auto& SetPlane(void) noexcept { return this->m_Plane; }
	const auto& GetTarget(void) const noexcept { return this->m_Target; }
	auto& SetTarget(void) noexcept { return this->m_Target; }

	const auto& GetScore(void) const noexcept { return this->m_Score; }
	void AddScore(void) noexcept { ++this->m_Score; }

	const auto& GetTime(void) const noexcept { return this->m_Timer; }
	auto& SetTime(void) noexcept { return this->m_Timer; }
};
