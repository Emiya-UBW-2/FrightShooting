#pragma warning(disable:5259)
#include "BackGround.hpp"

#include "../MainScene/Common.hpp"

const BackGround* Util::SingletonBase<BackGround>::m_Singleton = nullptr;

void BackGround::Load() noexcept {
	Draw::MV1::Load("data/model/Sky/model.mv1", &SkyBoxID);

	switch (GameRule::Instance()->GetGameType()) {
	case GameType::Normal:
		Draw::MV1::Load("data/model/Map/model.mv1", &MapID);
		Draw::MV1::Load("data/model/Map/model.mv1", &ColID);
		break;
	case GameType::AllRange:
		Draw::MV1::Load("data/model/Map2/model.mv1", &MapID);
		Draw::MV1::Load("data/model/Map2/model.mv1", &ColID);
		break;
	default:
		break;
	}
}