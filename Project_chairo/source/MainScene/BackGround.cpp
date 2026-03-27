#pragma warning(disable:5259)
#include "BackGround.hpp"

#include "../MainScene/Common.hpp"
#include "GameRule.hpp"

const BackGround* Util::SingletonBase<BackGround>::m_Singleton = nullptr;

void BackGround::Load() noexcept {
	Draw::MV1::Load("data/model/Sky/model.mv1", &SkyBoxID);
	Draw::MV1::Load(GameRule::Instance()->GetStageModel(), &MapID);
	Draw::MV1::Load(GameRule::Instance()->GetStageModel(), &ColID);
}