#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5259)
#include "DrawUISystem.hpp"

#include "../Util/Localize.hpp"

namespace Draw {
	bool DrawModule::PartsParam::IsHitPoint(int x, int y, Param2D Parent) const noexcept {
		if (!this->m_IsHitCheck) { return false; }
		switch (this->m_Type) {
		case PartsType::Box:
		case PartsType::NineSlice:
		{
			float Rad = this->m_Now.Rad + Parent.Rad;
			Util::VECTOR2D PosOfs = Parent.OfsNoRad + this->m_Now.OfsNoRad + this->m_Now.Ofs.Rotate(Rad);

			Util::VECTOR2D Size;
			Size.x = this->m_Now.Size.x * this->m_Now.Scale.x * Parent.Scale.x;
			Size.y = this->m_Now.Size.y * this->m_Now.Scale.y * Parent.Scale.y;

			float x1 = PosOfs.x - Size.x * this->m_Now.Center.x;
			float y1 = PosOfs.y - Size.y * this->m_Now.Center.y;
			float x2 = PosOfs.x + Size.x * (1.f - this->m_Now.Center.x);
			float y2 = PosOfs.y + Size.y * (1.f - this->m_Now.Center.y);

			Util::VECTOR2D center;
			center.x = PosOfs.x + Size.x * (0.5f - this->m_Now.Center.x) * 2.f;
			center.y = PosOfs.y + Size.y * (0.5f - this->m_Now.Center.y) * 2.f;

			if (HitPointToSquare(Util::VECTOR2D::vget(static_cast<float>(x), static_cast<float>(y)),
				center + (Util::VECTOR2D::vget(x1, y1) - center).Rotate(Rad),
				center + (Util::VECTOR2D::vget(x2, y1) - center).Rotate(Rad),
				center + (Util::VECTOR2D::vget(x2, y2) - center).Rotate(Rad),
				center + (Util::VECTOR2D::vget(x1, y2) - center).Rotate(Rad)
			)) {
				return true;
			}
		}
		break;
		case PartsType::Image:
		case PartsType::String:
		case PartsType::Json:
		case PartsType::Max:
		default:
			break;
		}
		return false;
	}
	void DrawModule::PartsParam::Update(DrawUISystem* DrawUI, Param2D Parent) const noexcept {
		float Rad = this->m_Now.Rad + Parent.Rad;
		Util::VECTOR2D PosOfs = Parent.OfsNoRad + this->m_Now.OfsNoRad + this->m_Now.Ofs.Rotate(Rad);

		Util::VECTOR2D scale;
		scale.x = this->m_Now.Scale.x * Parent.Scale.x;
		scale.y = this->m_Now.Scale.y * Parent.Scale.y;

		Util::ColorRGBA	Color = this->m_Now.Color.GetMult(Parent.Color);

		switch (this->m_Type) {
		case PartsType::Json:
		{
			Param2D Child;
			Child.OfsNoRad = PosOfs;
			Child.Rad = Rad;
			Child.Scale = scale;
			Child.Color = Color;
			DrawUI->Get(this->m_DrawModuleHandle).Update(DrawUI, Child);
		}
		break;
		case PartsType::Box:
		case PartsType::NineSlice:
		case PartsType::Image:
		case PartsType::String:
		case PartsType::Max:
		default:
			break;
		}
	}
	void DrawModule::PartsParam::Draw(DrawUISystem* DrawUI, Param2D Parent) const noexcept {
		float Rad = this->m_Now.Rad + Parent.Rad;
		Util::VECTOR2D PosOfs = Parent.OfsNoRad + this->m_Now.OfsNoRad + this->m_Now.Ofs.Rotate(Rad);

		Util::VECTOR2D scale;
		scale.x = this->m_Now.Scale.x * Parent.Scale.x;
		scale.y = this->m_Now.Scale.y * Parent.Scale.y;

		Util::VECTOR2D Size;
		Size.x = this->m_Now.Size.x * scale.x;
		Size.y = this->m_Now.Size.y * scale.y;

		Util::ColorRGBA	Color = this->m_Now.Color.GetMult(Parent.Color);

		float x1 = PosOfs.x - Size.x * this->m_Now.Center.x;
		float y1 = PosOfs.y - Size.y * this->m_Now.Center.y;
		float x2 = PosOfs.x + Size.x * (1.f - this->m_Now.Center.x);
		float y2 = PosOfs.y + Size.y * (1.f - this->m_Now.Center.y);

		switch (this->m_Type) {
		case PartsType::Box:
		{
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Color.GetA());

			if (Rad == 0.f) {
				DxLib::DrawBox(
					static_cast<int>(x1), static_cast<int>(y1),
					static_cast<int>(x2), static_cast<int>(y2),
					Color.GetColor(),
					TRUE
				);
			}
			else {
				Util::VECTOR2D center;
				center.x = PosOfs.x + Size.x * (0.5f - this->m_Now.Center.x) * 2.f;
				center.y = PosOfs.y + Size.y * (0.5f - this->m_Now.Center.y) * 2.f;

				Util::VECTOR2D  P1 = center + (Util::VECTOR2D::vget(x1, y1) - center).Rotate(Rad);
				Util::VECTOR2D  P2 = center + (Util::VECTOR2D::vget(x2, y1) - center).Rotate(Rad);
				Util::VECTOR2D  P3 = center + (Util::VECTOR2D::vget(x2, y2) - center).Rotate(Rad);
				Util::VECTOR2D  P4 = center + (Util::VECTOR2D::vget(x1, y2) - center).Rotate(Rad);
				DxLib::DrawQuadrangle(
					static_cast<int>(P1.x), static_cast<int>(P1.y),
					static_cast<int>(P2.x), static_cast<int>(P2.y),
					static_cast<int>(P3.x), static_cast<int>(P3.y),
					static_cast<int>(P4.x), static_cast<int>(P4.y),
					Color.GetColor(),
					TRUE
				);
			}
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		break;
		case PartsType::NineSlice:
		{
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Color.GetA());
			DxLib::SetDrawBright(Color.GetR(), Color.GetG(), Color.GetB());
			Draw9SliceGraph(
				Util::VECTOR2D::vget(x1, y1), Util::VECTOR2D::vget(x2, y2),
				this->m_Min, this->m_Max,
				Util::VECTOR2D::vget(1.f, 1.f) - this->m_Now.Center,
				Rad,
				this->m_ImageHandle->get(),
				true,
				false
			);
			DxLib::SetDrawBright(255, 255, 255);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		break;
		case PartsType::Image:
		{
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Color.GetA());
			DxLib::SetDrawBright(Color.GetR(), Color.GetG(), Color.GetB());

			if (Rad == 0.f) {
				DxLib::DrawExtendGraph(
					static_cast<int>(x1), static_cast<int>(y1),
					static_cast<int>(x2), static_cast<int>(y2),
					this->m_ImageHandle->get(),
					true
				);
			}
			else {
				/*
				DxLib::DrawRotaGraph3(
					static_cast<int>((x2 + x1) / 2.f), static_cast<int>((y2 + y1) / 2.f),
					static_cast<int>((x2 + x1) / 2.f), static_cast<int>((y2 + y1) / 2.f),
					scale.x, scale.y, Rad, this->ImageHandle->get(), true);
				//*/
				DxLib::DrawRotaGraphFast(
					static_cast<int>((x2 + x1) / 2.f), static_cast<int>((y2 + y1) / 2.f),
					scale.y, Rad, this->m_ImageHandle->get(), true);
			}
			DxLib::SetDrawBright(255, 255, 255);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		break;
		case PartsType::String:
		{
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Color.GetA());
			if (Rad == 0.f) {
				auto* Font = FontPool::Instance();
				auto* Localize = Util::LocalizePool::Instance();
				/*
				DxLib::DrawBox(
					static_cast<int>(x1), static_cast<int>(y1),
					static_cast<int>(x2), static_cast<int>(y2),
					Color.GetColor(),
					TRUE
				);
				this->m_FontHandle->DrawStringAutoFit(
					static_cast<int>(x1), static_cast<int>(y1),
					static_cast<int>(x2), static_cast<int>(y2),
					Color.GetColor(), ColorPalette::Black, this->String);
				//*/
				Font->Get(FontType::DIZ_UD_Gothic, static_cast<int>(18.f * scale.y), 3)->DrawString(
					FontXCenter::MIDDLE, FontYCenter::MIDDLE,
					static_cast<int>((x2 + x1) / 2.f), static_cast<int>((y2 + y1) / 2.f),
					Color.GetColor(), ColorPalette::Black,
					Util::SjistoUTF8((this->m_String != "") ? this->m_String : Localize->Get(this->m_StringID)));
			}
			else {
			}
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		break;
		case PartsType::Json:
		{
			DrawUI->Get(this->m_DrawModuleHandle).Draw(DrawUI);
		}
		break;
		case PartsType::Max:
		default:
			break;
		}
	}
	void DrawModule::PartsParam::CalcAnim(const AnimParam& Anim, bool IsFirstFrame, float Per) noexcept {
		if (Anim.m_OfsNoRadChanged) {
			if (IsFirstFrame) {
				this->m_Before.OfsNoRad = this->m_Now.OfsNoRad;
			}
			this->m_Now.OfsNoRad = Util::Lerp(this->m_Before.OfsNoRad, Anim.m_Target.OfsNoRad, Per);
		}
		if (Anim.m_OfsChanged) {
			if (IsFirstFrame) {
				this->m_Before.Ofs = this->m_Now.Ofs;
			}
			this->m_Now.Ofs = Util::Lerp(this->m_Before.Ofs, Anim.m_Target.Ofs, Per);
		}
		if (Anim.m_SizeChanged) {
			if (IsFirstFrame) {
				this->m_Before.Size = this->m_Now.Size;
			}
			this->m_Now.Size = Util::Lerp(this->m_Before.Size, Anim.m_Target.Size, Per);
		}
		if (Anim.m_ScaleChanged) {
			if (IsFirstFrame) {
				this->m_Before.Scale = this->m_Now.Scale;
			}
			this->m_Now.Scale = Util::Lerp(this->m_Before.Scale, Anim.m_Target.Scale, Per);
		}
		if (Anim.m_CenterChanged) {
			if (IsFirstFrame) {
				this->m_Before.Center = this->m_Now.Center;
			}
			this->m_Now.Center = Util::Lerp(this->m_Before.Center, Anim.m_Target.Center, Per);
		}
		if (Anim.m_RadChanged) {
			if (IsFirstFrame) {
				this->m_Before.Rad = this->m_Now.Rad;
			}
			this->m_Now.Rad = Util::Lerp(this->m_Before.Rad, Anim.m_Target.Rad, Per);
		}
		if (Anim.m_ColorChanged) {
			if (IsFirstFrame) {
				this->m_Before.Color = this->m_Now.Color;
			}
			this->m_Now.Color = Util::Lerp(this->m_Before.Color, Anim.m_Target.Color, Per);
		}
	}
	void DrawModule::PartsParam::AddChild(DrawUISystem* DrawUI, std::string_view ChildName, std::string_view ChildBranch, std::string_view FilePath, Param2D Param) noexcept {
		this->m_Name = ChildName;
		this->m_Type = PartsType::Json;
		this->m_Base = Param;
		this->m_DrawModuleHandle = DrawUI->Add(FilePath, ChildBranch);
		SetDefault();
	}
	void DrawModule::PartsParam::SetByJson(DrawUISystem* DrawUI, nlohmann::json& data, const std::string& BranchName) noexcept {
		this->m_Name = data["Name"];
		{
			std::string TypeStr = data["Type"];
			for (int loop = 0; loop < static_cast<int>(PartsType::Max); ++loop) {
				if (TypeStr == PartsTypeStr[loop]) {
					this->m_Type = static_cast<PartsType>(loop);
					break;
				}
			}
		}
		if (data.contains("OfsNoRad")) {
			this->m_Base.OfsNoRad.SetByJson(data["OfsNoRad"]);
		}
		if (data.contains("Ofs")) {
			this->m_Base.Ofs.SetByJson(data["Ofs"]);
		}
		if (data.contains("Size")) {
			this->m_Base.Size.SetByJson(data["Size"]);
		}
		if (data.contains("Scale")) {
			this->m_Base.Scale.SetByJson(data["Scale"]);
		}
		if (data.contains("Center")) {
			this->m_Base.Center.SetByJson(data["Center"]);
		}
		if (data.contains("Rad")) {
			this->m_Base.Rad = data["Rad"];
			this->m_Base.Rad = Util::deg2rad(this->m_Base.Rad);
		}
		if (data.contains("BaseColor")) {
			this->m_Base.Color.SetByJson(data["BaseColor"]);
		}
		else {
			this->m_Base.Color.Set(255, 255, 255, 255);
		}

		if (data.contains("FontID")) {
			this->m_StringID = data["FontID"];
			this->m_String = "";
		}
		if (data.contains("Min")) {
			this->m_Min.SetByJson(data["Min"]);
		}
		if (data.contains("Max")) {
			this->m_Max.SetByJson(data["Max"]);
		}

		if (data.contains("Image")) {
			this->m_ImagePath = data["Image"];
			this->m_ImageHandle = GraphPool::Instance()->Get(this->m_ImagePath)->Get();
		}
		if (data.contains("FilePath")) {
			std::string FilePath = data["FilePath"];
			std::string ChildBranch;
			if (BranchName != "") {
				ChildBranch = BranchName + "/";
			}
			ChildBranch += this->m_Name;
			this->m_DrawModuleHandle = DrawUI->Add(FilePath.c_str(), ChildBranch.c_str());
		}
		if (data.contains("IsHitCheck")) {
			this->m_IsHitCheck = data["IsHitCheck"];
		}
		if (data.contains("Override")) {
			std::string ChildBranch;
			if (BranchName != "") {
				ChildBranch = BranchName + "/";
			}
			ChildBranch += this->m_Name;
			int ID = DrawUI->GetID(ChildBranch);
			if (ID != InvalidID) {
				for (auto& o : data["Override"]) {
					std::string Target = o["Target"];
					auto* Parts = DrawUI->Get(ID).GetParts(Target);
					if (o.contains("FontID")) {
						Parts->m_StringID = o["FontID"];
						Parts->m_String = "";
					}
				}
			}
		}

		SetDefault();
	}

	void DrawModule::AddChild(DrawUISystem* DrawUI, std::string_view ChildName, std::string_view FilePath, Param2D Param) noexcept {
		this->m_PartsParam.emplace_back();

		std::string ChildBranch = "";
		if (this->m_BranchName != "") {
			ChildBranch = this->m_BranchName + "/";
		}
		ChildBranch += ChildName;
		this->m_PartsParam.back().AddChild(DrawUI, ChildName, ChildBranch.c_str(), FilePath, Param);
	}
	void DrawModule::DeleteChild(std::string_view ChildName) noexcept {
		for (int loop = 0, max = static_cast<int>(this->m_PartsParam.size()); loop < max; ++loop) {
			auto& parts = this->m_PartsParam.at(static_cast<size_t>(loop));
			if (parts.GetName() == ChildName) {
				this->m_PartsParam.erase(this->m_PartsParam.begin() + loop);
				break;
			}
		}
	}

	void DrawModule::Init(DrawUISystem* DrawUI, std::string_view Path, std::string_view Branch) noexcept {
		this->m_PartsParam.clear();
		this->m_AnimData.clear();
		std::ifstream file(Path.data());
		nlohmann::json jsonData = nlohmann::json::parse(file);

		this->m_BranchName = Branch;

		for (auto& data : jsonData["MainParts"]) {
			this->m_PartsParam.emplace_back();
			this->m_PartsParam.back().SetByJson(DrawUI, data, this->m_BranchName);
		}
		for (auto& data : jsonData["Anim"]) {
			this->m_AnimData.emplace_back();
			auto& AnimBack = this->m_AnimData.back();
			//
			{
				std::string Type = data["Type"];
				for (int loop = 0; loop < static_cast<int>(AnimType::Max); ++loop) {
					if (Type == AnimTypeStr[loop]) {
						AnimBack.m_Type = static_cast<AnimType>(loop);
						break;
					}
				}
			}
			//
			AnimBack.m_IsLoop = data["Loop"];
			//
			for (auto& a : data["Anim"]) {
				AnimBack.m_AnimParam.emplace_back();
				auto& AnimParamBack = AnimBack.m_AnimParam.back();
				//
				{
					std::string Target = a["Target"];
					for (auto& parts : this->m_PartsParam) {
						if (Target == parts.GetName()) {
							AnimParamBack.m_TargetID = static_cast<size_t>(&parts - &this->m_PartsParam.front());
							break;
						}
					}
				}
				//
				if (a.contains("OfsNoRad")) {
					AnimParamBack.m_OfsNoRadChanged = true;
					AnimParamBack.m_Target.OfsNoRad.SetByJson(a["OfsNoRad"]);
				}
				if (a.contains("Ofs")) {
					AnimParamBack.m_OfsChanged = true;
					AnimParamBack.m_Target.Ofs.SetByJson(a["Ofs"]);
				}
				if (a.contains("Size")) {
					AnimParamBack.m_SizeChanged = true;
					AnimParamBack.m_Target.Size.SetByJson(a["Size"]);
				}
				if (a.contains("Scale")) {
					AnimParamBack.m_ScaleChanged = true;
					AnimParamBack.m_Target.Scale.SetByJson(a["Scale"]);
				}
				if (a.contains("Center")) {
					AnimParamBack.m_CenterChanged = true;
					AnimParamBack.m_Target.Center.SetByJson(a["Center"]);
				}
				if (a.contains("Rad")) {
					AnimParamBack.m_RadChanged = true;
					AnimParamBack.m_Target.Rad = a["Rad"];
					AnimParamBack.m_Target.Rad = Util::deg2rad(AnimParamBack.m_Target.Rad);
				}
				if (a.contains("Color")) {
					AnimParamBack.m_ColorChanged = true;
					AnimParamBack.m_Target.Color.SetByJson(a["Color"]);
				}
				else {
					AnimParamBack.m_Target.Color.Set(255, 255, 255, 255);
				}
				//
				AnimParamBack.m_StartFrame = a["StartFrame"];
				AnimParamBack.m_EndFrame = a["EndFrame"];
			}
		}

		this->m_AnimDataLastSelect = InvalidID;
		this->m_Frame = 0;

		this->m_IsActive = true;
		this->m_IsSelect = false;

		this->m_UseActive = false;
		for (auto& a : this->m_AnimData) {
			if (AnimType::Active != a.m_Type && AnimType::DisActive != a.m_Type) { continue; }
			this->m_UseActive = true;
		}

		this->m_UseButton = false;
		for (auto& a : this->m_AnimData) {
			if (AnimType::OnSelect != a.m_Type && AnimType::OnPress != a.m_Type) { continue; }
			this->m_UseButton = true;
		}
	}
	void DrawModule::Update(DrawUISystem* DrawUI, Param2D Param) noexcept {
		this->m_BasePositionParam = Param;
		AnimType SelectNow = AnimType::Normal;
		if (this->m_UseActive) {
			if (IsActive()) {
				SelectNow = AnimType::Active;
			}
			else {
				SelectNow = AnimType::DisActive;
			}
		}

		{
			auto* DrawerMngr = MainDraw::Instance();

			this->m_IsSelect = false;
			this->m_IsHitCheck = false;
			for (auto& parts : this->m_PartsParam) {
				if (parts.IsHitPoint(DrawerMngr->GetMousePositionX(), DrawerMngr->GetMousePositionY(), this->m_BasePositionParam)) {
					this->m_IsSelect = true;
					break;
				}

				if (parts.IsHitCheck()) {
					this->m_IsHitCheck = true;
				}
			}
		}
		if (this->m_UseButton) {
			if (this->m_IsSelect) {
				SelectNow = AnimType::OnSelect;
				auto* KeyMngr = Util::KeyParam::Instance();
				if (KeyMngr->GetMenuKeyPress(Util::EnumMenu::Diside)) {
					SelectNow = AnimType::OnPress;
				}
			}
		}

		bool IsSelectAnim = false;
		for (auto& a : this->m_AnimData) {
			//アニメ選択
			if (SelectNow != a.m_Type) { continue; }
			IsSelectAnim = true;
			int index = static_cast<int>(&a - &this->m_AnimData.front());
			//フレーム更新
			if (this->m_AnimDataLastSelect != index) {
				this->m_AnimDataLastSelect = index;
				this->m_Frame = 0;
			}
			else {
				auto Max = 0;
				for (auto& anim : a.m_AnimParam) {
					Max = std::max(Max, anim.m_EndFrame);
				}
				this->m_Frame = std::clamp(this->m_Frame + 1, 0, Max);
				if ((this->m_Frame == Max) && a.m_IsLoop) {
					this->m_Frame = 0;
				}
				this->m_IsAnimeEnd = ((this->m_Frame == Max) && !a.m_IsLoop);
			}

			//
			for (auto& anim : a.m_AnimParam) {
				if (anim.m_StartFrame <= this->m_Frame && this->m_Frame <= anim.m_EndFrame) {
					this->m_PartsParam.at(anim.m_TargetID).CalcAnim(anim, anim.m_StartFrame == this->m_Frame,
						Util::GetPer01(static_cast<float>(anim.m_StartFrame), static_cast<float>(anim.m_EndFrame), static_cast<float>(this->m_Frame))
					);
				}
			}
		}
		if (!IsSelectAnim) {
			this->m_AnimDataLastSelect = InvalidID;
			for (auto& parts : this->m_PartsParam) {
				parts.SetDefault();
			}
		}

		for (auto& parts : this->m_PartsParam) {
			parts.Update(DrawUI, this->m_BasePositionParam);
		}
	}
}