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
	std::shared_ptr<MyPlane>	m_Plane;

	std::vector<std::shared_ptr<Enemy>>	m_Enemy;
private:
	PlayerManager(void) noexcept {}
	PlayerManager(const PlayerManager&) = delete;
	PlayerManager(PlayerManager&&) = delete;
	PlayerManager& operator=(const PlayerManager&) = delete;
	PlayerManager& operator=(PlayerManager&&) = delete;
	virtual ~PlayerManager(void) noexcept { Dispose(); }
public:
	void Load(void) noexcept {
		ObjectManager::Instance()->LoadModel("data/model/Plane/");
		this->m_Enemy.reserve(2000);
	}
	void Init(void) noexcept {
		this->m_Plane = std::make_shared<MyPlane>();
		ObjectManager::Instance()->InitObject(this->m_Plane, this->m_Plane, "data/model/Plane/");
	}
	void Dispose(void) noexcept {
		this->m_Plane.reset();
		this->m_Enemy.clear();
	}
public:
	auto& SetPlane(void) noexcept { return this->m_Plane; }
	auto& SetEnemy(void) noexcept { return this->m_Enemy; }
};
