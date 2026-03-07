#pragma once
#pragma warning(disable:4464)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#define NOMINMAX
#pragma warning( push, 3 )
#include "DxLib.h"
#include <array>
#pragma warning( pop )
#include "../Util/Util.hpp"
#include "../Util/Algorithm.hpp"

namespace Draw {
	enum class LightType {
		POINT = DX_LIGHTTYPE_POINT,
		SPOT = DX_LIGHTTYPE_SPOT,
		DIRECTIONAL = DX_LIGHTTYPE_DIRECTIONAL,
	};
	class LightHandle : public Util::DXHandle {
	protected:
		void	Dispose_Sub(void) noexcept override {
			DeleteLightHandle(Util::DXHandle::get());
		}
	public:
		LightHandle(void) noexcept {}
		LightHandle(const LightHandle&) = delete;
		LightHandle(LightHandle&&) = delete;
		LightHandle& operator=(const LightHandle&) = delete;
		LightHandle& operator=(LightHandle&&) = delete;
		virtual ~LightHandle(void) noexcept {}
	public:
		void	SetPos(const Util::VECTOR3D& Position) const noexcept { SetLightPositionHandle(Util::DXHandle::get(), Position.get()); }
		void SetPos(const Util::VECTOR3D& Position, const Util::VECTOR3D& Direction) const noexcept {
			SetPos(Position);
			SetLightDirectionHandle(Util::DXHandle::get(), Direction.get());
		}
	public:
		void CreateSpot(const Util::VECTOR3D& Position, const Util::VECTOR3D& Direction, float OutAngle, float InAngle, float Range, float Atten0, float Atten1, float Atten2) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::CreateSpotLightHandle(Position.get(), Direction.get(), OutAngle, InAngle, Range, Atten0, Atten1, Atten2));
		}
		void CreatePoint(const Util::VECTOR3D& Position, float Range, float Atten0, float Atten1, float Atten2) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::CreatePointLightHandle(Position.get(), Range, Atten0, Atten1, Atten2));
		}
		void CreateDir(const Util::VECTOR3D& Directional) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::CreateDirLightHandle(Directional.get()));
		}
	};
	// ライトプール
	class LightPool : public Util::SingletonBase<LightPool> {
	private:
		friend class Util::SingletonBase<LightPool>;
	private:
		static const size_t Size = 5;
		std::array<LightHandle, Size>	handles;
		size_t							m_now = 0;
	private:
		LightPool(void) noexcept {}
		LightPool(const LightPool&) = delete;
		LightPool(LightPool&&) = delete;
		LightPool& operator=(const LightPool&) = delete;
		LightPool& operator=(LightPool&&) = delete;
	public:
		const LightHandle& Put(LightType Lighttype, const Util::VECTOR3D& pos) noexcept {
			size_t prev = this->m_now;
			handles[m_now].Dispose();
			// handles[m_now].time = GetNowHiPerformanceCount();
			switch (Lighttype) {
			case LightType::POINT:
				handles[m_now].CreatePoint(pos, 2.5f, 0.5f, 1.5f, 0.5f);
				break;
			case LightType::SPOT:
				handles[m_now].CreateSpot(pos, Util::VECTOR3D::down(), DX_PI_F / 2, DX_PI_F / 4, 2.5f, 0.5f, 1.5f, 0.5f);
				break;
			case LightType::DIRECTIONAL:
				handles[m_now].CreateDir(pos);
				break;
			default:
				break;
			}
			++m_now %= Size;
			return handles[prev];
		}
		void			Update(void) noexcept {
			/*
			for (auto& h : handles)
			{
				if (h.IsActive())
				{
					if ((GetNowHiPerformanceCount() - h.time) >= 1000000 / 30)
					{
						h.Dispose();
					}
				}
			}
			//*/
		}
		void			Dispose(void) noexcept {
			for (auto& h : handles)
			{
				if (h.IsActive())
				{
					h.Dispose();
				}
			}
		}
	};
};
