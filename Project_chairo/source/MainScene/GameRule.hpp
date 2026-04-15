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
	}
	void Init(void) noexcept {
		this->m_Plane = std::make_shared<MyPlane>();
		ObjectManager::Instance()->InitObject(this->m_Plane, this->m_Plane, "data/model/Plane/");
	}
	void Dispose(void) noexcept {
		this->m_Plane.reset();
	}
public:
	auto& SetPlane(void) noexcept { return this->m_Plane; }
};


enum class GameType : size_t {
	Normal,
	AllRange,
	Max,
};
static const char* GameTypeName[static_cast<int>(GameType::Max)] = {
	"Normal",
	"AllRange",
};

class GameRule : public Util::SingletonBase<GameRule> {
private:
	friend class Util::SingletonBase<GameRule>;
private:
	int						m_HP{};
	bool					m_IsStartEvent{ false };
	char		padding[3]{};

	std::string			m_ModelName;
	std::string			m_StageName;
	std::string			m_EventName;
	GameType			m_GameType{ GameType::AllRange };
private:
	GameRule(void) noexcept {}
	GameRule(const GameRule&) = delete;
	GameRule(GameRule&&) = delete;
	GameRule& operator=(const GameRule&) = delete;
	GameRule& operator=(GameRule&&) = delete;
	virtual ~GameRule(void) noexcept { Dispose(); }
public:
	void SetHP(int value) noexcept {
		this->m_HP = value;
	}
	const auto& GetHP(void) const noexcept { return this->m_HP; }

	void SetIsStartEvent(bool value) noexcept {
		this->m_IsStartEvent = value;
	}
	const auto& GetIsStartEvent(void) const noexcept { return this->m_IsStartEvent; }


	void SetStageModel(std::string_view str) noexcept {
		this->m_ModelName = str;
	}
	const auto& GetStageModel(void) const noexcept { return this->m_ModelName; }

	void SetNextStage(std::string_view str) noexcept {
		this->m_StageName = str;
	}
	const auto& GetNextStage(void) const noexcept { return this->m_StageName; }

	void SetNextEvent(std::string_view str) noexcept {
		this->m_EventName = str;
	}
	const auto& GetNextEvent(void) const noexcept { return this->m_EventName; }

	void SetGameType(GameType type) noexcept {
		this->m_GameType = type;
	}
	const auto& GetGameType(void) const noexcept { return this->m_GameType; }
public:
	void Dispose(void) noexcept {
	}
};
