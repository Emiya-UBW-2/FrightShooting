#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../Util/Util.hpp"
#include "Character.hpp"
#include "EarlyCharacter.hpp"

class PlayerManager : public Util::SingletonBase<PlayerManager> {
private:
	friend class Util::SingletonBase<PlayerManager>;
private:
	std::vector<std::shared_ptr<CharacterCommon>>	m_Character;
private:
	PlayerManager(void) noexcept {}
	PlayerManager(const PlayerManager&) = delete;
	PlayerManager(PlayerManager&&) = delete;
	PlayerManager& operator=(const PlayerManager&) = delete;
	PlayerManager& operator=(PlayerManager&&) = delete;
	virtual ~PlayerManager(void) noexcept { Dispose(); }
public:
	void Load(void) noexcept {
		ObjectManager::Instance()->LoadModel("data/model/Soldier/");
		ObjectManager::Instance()->LoadModel("data/model/Early/");
		this->m_Character.resize(6);
	}
	void Init(void) noexcept {
		size_t loop = 0;

		this->m_Character.at(loop) = std::make_shared<Character>();
		ObjectManager::Instance()->InitObject(this->m_Character.at(loop), this->m_Character.at(loop), "data/model/Soldier/");
		//this->m_Character.at(loop)->SetPos(BackGround::Instance()->GetWorldPos(m.m_pos));
		++loop;

		for (auto& m : BackGround::Instance()->GetMapInfo()) {
			if (m.m_InfoType == InfoType::WayPoint) {
				this->m_Character.at(loop) = std::make_shared<EarlyCharacter>();
				ObjectManager::Instance()->InitObject(this->m_Character.at(loop), this->m_Character.at(loop), "data/model/Early/");
				this->m_Character.at(loop)->SetPos(BackGround::Instance()->GetWorldPos(m.m_pos));
				++loop;
				if (loop >= this->m_Character.size()) { break; }
			}
		}
	}
	void Dispose(void) noexcept {
		for (auto& m : this->m_Character) {
			m.reset();
		}
		this->m_Character.clear();
	}
public:
	const auto& GetCharacter(void) const noexcept { return this->m_Character; }
	auto& SetCharacter(void) noexcept { return this->m_Character; }
};
