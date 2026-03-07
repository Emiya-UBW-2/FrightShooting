#include "Algorithm.hpp"

#pragma warning(disable:4464)

#include "../Draw/MainDraw.hpp"

namespace Util {
	float GetEasingRatio(float ratio) noexcept {
		auto* DrawerMngr = Draw::MainDraw::Instance();
		return (1.f - std::powf(ratio, 60.f * DrawerMngr->GetDeltaTime()));
	}
}