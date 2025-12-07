#include "Camera.hpp"

#include "../Draw/MainDraw.hpp"

#pragma warning(disable:5259)
const Camera::Camera3D* Util::SingletonBase<Camera::Camera3D>::m_Singleton = nullptr;

namespace Camera {

	void Camera3D::Update(void) noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		if (this->m_SendShakeTime == 0.f) { return; }
		auto RandRange = this->m_Timer / this->m_SendShakeTime * this->m_SendShakePower;
		Easing(&this->m_Shake1, Util::VECTOR3D::vget(GetRandf(RandRange), GetRandf(RandRange), GetRandf(RandRange)), 0.8f);
		Easing(&this->m_Shake2, this->m_Shake1, 0.8f);
		this->m_Timer = std::max(this->m_Timer - DrawerMngr->GetDeltaTime(), 0.f);
	}
};
