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

class BackGround : public Util::SingletonBase<BackGround> {
private:
	friend class Util::SingletonBase<BackGround>;
private:
private:
	Draw::MV1				SkyBoxID{};
private:
	BackGround(void) noexcept {}
	BackGround(const BackGround&) = delete;
	BackGround(BackGround&&) = delete;
	BackGround& operator=(const BackGround&) = delete;
	BackGround& operator=(BackGround&&) = delete;
	virtual ~BackGround(void) noexcept { Dispose(); }
public:
public:
	void Load(const char* MapName) noexcept {
		Draw::MV1::Load("data/model/SkyBox/model.mqoz", &SkyBoxID);
	}
	void Init(void) noexcept {
	}
	void Update(void) noexcept {
	}
	void Dispose(void) noexcept {
		SkyBoxID.Dispose();
	}

	void BGDraw(void) const noexcept {
		DxLib::SetUseLighting(FALSE);
		SkyBoxID.DrawModel();
		DxLib::SetUseLighting(TRUE);
	}
	void SetShadowDrawRigid(void) const noexcept {}
	void SetShadowDraw(void) const noexcept {}
	void Draw(void) const noexcept {}
	void ShadowDrawFar(void) const noexcept {}
	void ShadowDraw(void) const noexcept {}
};
