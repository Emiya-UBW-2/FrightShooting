#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)
#include "../Util/Enum.hpp"
#include "../Util/Util.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Draw/MainDraw.hpp"
#include "../Draw/Camera.hpp"
#include "../Draw/PostPass.hpp"
#include "../Draw/Light.hpp"
#include "../Draw/MV1.hpp"

static float GetRandf(float arg) noexcept { return -arg + static_cast<float>(GetRand(static_cast<int>(arg * 2.f * 10000.f))) / 10000.f; }

class BackGround : public Util::SingletonBase<BackGround> {
private:
	friend class Util::SingletonBase<BackGround>;
private:
	Draw::MV1				SkyBoxID{};
	Draw::MV1				MapID{};
public:
	BackGround(void) noexcept {}
	BackGround(const BackGround&) = delete;
	BackGround(BackGround&&) = delete;
	BackGround& operator=(const BackGround&) = delete;
	BackGround& operator=(BackGround&&) = delete;
	virtual ~BackGround(void) noexcept { Dispose(); }
public:
public:
	void Load() noexcept {
		Draw::MV1::Load("data/model/Sky/model.mv1", &SkyBoxID);
		Draw::MV1::Load("data/model/Map/model.mv1", &MapID);
	}
	void Init(void) noexcept {
	}
	void Update(void) noexcept {
	}
	void Dispose(void) noexcept {
		SkyBoxID.Dispose();
		MapID.Dispose();
	}

	void BGDraw(void) const noexcept {
		SkyBoxID.SetMatrix(Util::Matrix4x4::Mtrans(GetCameraPosition()));
		DxLib::SetUseLighting(FALSE);
		SkyBoxID.DrawModel();
		DxLib::SetUseLighting(TRUE);
	}
	void SetShadowDrawRigid(void) const noexcept {
		MapID.DrawModel();
	}
	void SetShadowDraw(void) const noexcept {
	}
	void CheckDraw(void) noexcept {
	}
	void Draw(void) const noexcept {
		//Fog
		SetFogEnable(true);
		SetFogMode(DX_FOGMODE_LINEAR);
		SetFogStartEnd(0.f*Scale3DRate, 250000.f * Scale3DRate);
		SetFogColor(100, 111, 128);
		MapID.DrawModel();
		SetFogEnable(false);
	}
	void ShadowDrawFar(void) const noexcept {
		MapID.DrawModel();
	}
	void ShadowDraw(void) const noexcept {
		MapID.DrawModel();
	}
};
