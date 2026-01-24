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
	}
	void Init(void) noexcept {
		this->m_Plane.at(0) = std::make_shared<PlaneCommon>();
		ObjectManager::Instance()->InitObject(this->m_Plane.at(0), this->m_Plane.at(0), "data/model/Sopwith/");
		this->m_Plane.at(0)->SetPlayerID(0);
		//this->m_Plane.at(0)->SetPos(BackGround::Instance()->GetWorldPos(m.m_pos));
	}
	void Dispose(void) noexcept {
		for (auto& m : this->m_Plane) {
			m.reset();
		}
		this->m_Plane.clear();
	}
public:
	const auto& GetPlane(void) const noexcept { return this->m_Plane; }
	auto& SetPlane(void) noexcept { return this->m_Plane; }
};
