#pragma once
#pragma warning(disable:4464)
#include "../Util/Util.hpp"
#include "../Util/Algorithm.hpp"
#include "ImageDraw.hpp"

namespace Camera {
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// カメラシェイク
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class Camera3D : public Util::SingletonBase<Camera3D> {
	private:
		friend class Util::SingletonBase<Camera3D>;
	private:
		float						m_SendShakeTime{ 1.f };
		float						m_SendShakePower{ 1.f };
		float						m_Timer{ 0.f };
		char		padding[4]{};
		Util::VECTOR3D				m_Shake1{};
		Util::VECTOR3D				m_Shake2{};
		Draw::Camera3DInfo			m_Camera{};
		Draw::Camera3DInfo			m_CameraForDraw{};
	public:
		const auto& GetCamera(void) const noexcept { return this->m_Camera; }
		const auto& GetShake(void) const noexcept { return this->m_Shake2; }
		const auto& GetCameraForDraw(void) const noexcept { return this->m_CameraForDraw; }
	public:
		void			SetCamPos(const Util::VECTOR3D& cam_pos, const Util::VECTOR3D& cam_vec, const Util::VECTOR3D& cam_up) noexcept {
			this->m_Camera.SetCamPos(cam_pos, cam_vec, cam_up);
			this->m_CameraForDraw.SetCamPos(cam_pos + this->m_Shake2, cam_vec + this->m_Shake2 * 2.f, cam_up);
		}
		void			SetCamInfo(float cam_fov_, float cam_near_, float cam_far_) noexcept {
			this->m_Camera.SetCamInfo(cam_fov_, cam_near_, cam_far_);
			this->m_CameraForDraw.SetCamInfo(cam_fov_, cam_near_, cam_far_);
		}
		void			SetCamShake(float time, float power) noexcept {
			this->m_SendShakeTime = time;
			this->m_SendShakePower = power;
			this->m_Timer = this->m_SendShakeTime;
		}
		void			StopCamShake(void) noexcept {
			this->m_SendShakeTime = 1.f;
			this->m_SendShakePower = 1.f;
			this->m_Timer = 0.f;
			this->m_Shake1 = Util::VECTOR3D::zero();
			this->m_Shake2 = Util::VECTOR3D::zero();
		}
	private:
		Camera3D(void) noexcept {}
		Camera3D(const Camera3D&) = delete;
		Camera3D(Camera3D&&) = delete;
		Camera3D& operator=(const Camera3D&) = delete;
		Camera3D& operator=(Camera3D&&) = delete;

		virtual ~Camera3D(void) noexcept {}
	public:
		static float GetRandf(float arg) noexcept { return -arg + static_cast<float>(DxLib::GetRand(static_cast<int>(arg * 2.f * 10000.f))) / 10000.f; }

		void Update(void) noexcept;
	};
}
