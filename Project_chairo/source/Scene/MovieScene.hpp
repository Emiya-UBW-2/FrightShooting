#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../Util/SceneManager.hpp"

#include "../MainScene/BaseObject.hpp"

#include "../MainScene/Common.hpp"
enum class MovieObjFrame {
	Center,
	Eye,
	Gun1,
	Gun2,
	LWingtip,
	RWingtip,
	Nozzle1,
	Nozzle2,
	Max,
};
static const char* MovieObjFrameName[static_cast<int>(MovieObjFrame::Max)] = {
	"センター",
	"目",
	"機銃1",
	"機銃2",
	"左翼端",
	"右翼端",
	"ノズル1",
	"ノズル2",
};

class MovieObj : public BaseObject {
	bool m_IsDraw = true;
	char		padding[7]{};

	LineDraw				m_LineDraw1;
	LineDraw				m_LineDraw2;

	LineDraw				m_LineDraw3;
	LineDraw				m_LineDraw4;
public:
	MovieObj(void) noexcept {}
	MovieObj(const MovieObj&) = delete;
	MovieObj(MovieObj&&) = delete;
	MovieObj& operator=(const MovieObj&) = delete;
	MovieObj& operator=(MovieObj&&) = delete;
	virtual ~MovieObj(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return static_cast<int>(MovieObjFrame::Max); }
	const char* GetFrameStr(int id) noexcept override { return MovieObjFrameName[id]; }
public:
	void SetIsDraw(bool IsDraw) noexcept {
		m_IsDraw = IsDraw;
	}
	void			SetPlanePosition(Util::Matrix4x4 Mat) noexcept {
		SetMatrix(Mat);
		if (HaveFrame(static_cast<int>(MovieObjFrame::LWingtip))) {
			m_LineDraw1.Set(GetFrameLocalWorldMatrix(static_cast<int>(MovieObjFrame::LWingtip)).pos());
		}
		if (HaveFrame(static_cast<int>(MovieObjFrame::RWingtip))) {
			m_LineDraw2.Set(GetFrameLocalWorldMatrix(static_cast<int>(MovieObjFrame::RWingtip)).pos());
		}
		if (HaveFrame(static_cast<int>(MovieObjFrame::Nozzle1))) {
			m_LineDraw3.Set(GetFrameLocalWorldMatrix(static_cast<int>(MovieObjFrame::Nozzle1)).pos());
		}
		if (HaveFrame(static_cast<int>(MovieObjFrame::Nozzle2))) {
			m_LineDraw4.Set(GetFrameLocalWorldMatrix(static_cast<int>(MovieObjFrame::Nozzle2)).pos());
		}
	}
public:
	void Load_Sub(void) noexcept override {}
	void Init_Sub(void) noexcept override {}
	void Update_Sub(void) noexcept override{
		if (HaveFrame(static_cast<int>(MovieObjFrame::LWingtip))) {
			m_LineDraw1.Update(GetFrameLocalWorldMatrix(static_cast<int>(MovieObjFrame::LWingtip)).pos(), 0.25f);
		}
		if (HaveFrame(static_cast<int>(MovieObjFrame::RWingtip))) {
			m_LineDraw2.Update(GetFrameLocalWorldMatrix(static_cast<int>(MovieObjFrame::RWingtip)).pos(), 0.25f);
		}
		if (HaveFrame(static_cast<int>(MovieObjFrame::Nozzle1))) {
			m_LineDraw3.Update(GetFrameLocalWorldMatrix(static_cast<int>(MovieObjFrame::Nozzle1)).pos(), 0.05f);
		}
		if (HaveFrame(static_cast<int>(MovieObjFrame::Nozzle2))) {
			m_LineDraw4.Update(GetFrameLocalWorldMatrix(static_cast<int>(MovieObjFrame::Nozzle2)).pos(), 0.05f);
		}
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		if (!m_IsDraw) { return; }
		GetModel().DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		if (!m_IsDraw) { return; }
		GetModel().DrawModel();
	}
	void DrawFront_Sub(void) const noexcept override {
		if (!m_IsDraw) { return; }
		if (HaveFrame(static_cast<int>(MovieObjFrame::LWingtip))) {
			m_LineDraw1.Draw(0.05f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64), DX_BLENDMODE_ALPHA);
		}
		if (HaveFrame(static_cast<int>(MovieObjFrame::RWingtip))) {
			m_LineDraw2.Draw(0.05f * Scale3DRate / 2.f, DxLib::GetColor(64, 64, 64), DX_BLENDMODE_ALPHA);
		}
		if (HaveFrame(static_cast<int>(MovieObjFrame::Nozzle1))) {
			m_LineDraw3.Draw(0.5f * Scale3DRate / 2.f, DxLib::GetColor(255, 64, 12), DX_BLENDMODE_ADD);
		}
		if (HaveFrame(static_cast<int>(MovieObjFrame::Nozzle2))) {
			m_LineDraw4.Draw(0.5f * Scale3DRate / 2.f, DxLib::GetColor(255, 64, 12), DX_BLENDMODE_ADD);
		}
	}
	void ShadowDraw_Sub(void) const noexcept override {
		if (!m_IsDraw) { return; }
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
	}
};
struct StoryModel {
	int				m_StartFrame{};
	int				m_EndFrame{};
	std::string					m_ObjPath{ "" };
	std::shared_ptr<MovieObj>	m_MovieObj;
	Util::Matrix4x4				m_Mat;
	Util::Matrix4x4				m_MatEnd;
	int							m_UniqueID{};
	int							m_AnimID{ InvalidID };
	//char		padding[4]{};
};
struct StoryPop {
	int				m_StartFrame{};
	int				m_EndFrame{};
	//char		padding[4]{};
	Util::VECTOR3D	m_CameraPos;
	Util::VECTOR3D	m_CameraTarget;
	Util::VECTOR3D	m_CameraPosEnd;
	Util::VECTOR3D	m_CameraTargetEnd;
	float	m_CameraFov{};
	float	m_CameraFovEnd{};
	float	m_CameraShakePow{0.f};
};
class StoryScript {
	std::vector<StoryModel>	m_Models{};
	std::vector<StoryPop>	m_StoryPop;
	float					m_Frame{};
	float					m_ZPosGoal{};

	bool m_IsEnd = false;
	char		padding[7]{};
public:
	const auto& GetIsEnd() const noexcept { return m_IsEnd; }
public:
	void Load(std::string Path) noexcept {
		//
		{
			m_StoryPop.clear();
			m_Models.clear();
			File::InputFileStream FileStream;
			FileStream.Open("data/Event/" + Path + ".txt");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				std::vector<std::string> Args;
				File::GetArgs(FileStream.SeekLineAndGetStr(), &Args);
				//
				{
					if (Args.at(0) == "SetCut") {
						if (m_StoryPop.size() > 0) {
							m_StoryPop.back().m_EndFrame = std::stoi(Args.at(1));
						}
						m_StoryPop.emplace_back();
						m_StoryPop.back().m_StartFrame = std::stoi(Args.at(1));
					}
					if (Args.at(0) == "EndCut") {
						m_StoryPop.back().m_EndFrame = std::stoi(Args.at(1));
					}
					if (Args.at(0) == "SetCameraPos") {
						m_StoryPop.back().m_CameraPos = Util::VECTOR3D::vget(std::stof(Args.at(1)), std::stof(Args.at(2)), std::stof(Args.at(3))) * Scale3DRate;
						m_StoryPop.back().m_CameraPosEnd = m_StoryPop.back().m_CameraPos;
					}
					if (Args.at(0) == "SetCameraTarget") {
						m_StoryPop.back().m_CameraTarget = Util::VECTOR3D::vget(std::stof(Args.at(1)), std::stof(Args.at(2)), std::stof(Args.at(3))) * Scale3DRate;
						m_StoryPop.back().m_CameraTargetEnd = m_StoryPop.back().m_CameraTarget;
					}
					if (Args.at(0) == "SetCameraPosEnd") {
						m_StoryPop.back().m_CameraPosEnd = Util::VECTOR3D::vget(std::stof(Args.at(1)), std::stof(Args.at(2)), std::stof(Args.at(3))) * Scale3DRate;
					}
					if (Args.at(0) == "SetCameraTargetEnd") {
						m_StoryPop.back().m_CameraTargetEnd = Util::VECTOR3D::vget(std::stof(Args.at(1)), std::stof(Args.at(2)), std::stof(Args.at(3))) * Scale3DRate;
					}
					if (Args.at(0) == "SetCameraFov") {
						m_StoryPop.back().m_CameraFov = Util::deg2rad(std::stof(Args.at(1)));
						m_StoryPop.back().m_CameraFovEnd = m_StoryPop.back().m_CameraFov;
					}
					if (Args.at(0) == "SetCameraFovEnd") {
						m_StoryPop.back().m_CameraFovEnd = Util::deg2rad(std::stof(Args.at(1)));
					}
					if (Args.at(0) == "SetCameraShake") {
						m_StoryPop.back().m_CameraShakePow = Util::deg2rad(std::stof(Args.at(1)));
					}
					if (Args.at(0) == "SetModel") {
						m_Models.emplace_back();
						auto& b = m_Models.back();
						b.m_ObjPath = Args.at(1);
						ObjectManager::Instance()->LoadModel(b.m_ObjPath);
						b.m_UniqueID = std::stoi(Args.at(2));
						b.m_Mat =
							Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), std::stof(Args.at(6))) *
							Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), std::stof(Args.at(7))) *
							Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), std::stof(Args.at(8))) *
							Util::Matrix4x4::Mtrans(Util::VECTOR3D::vget(std::stof(Args.at(3)), std::stof(Args.at(4)), std::stof(Args.at(5))) * Scale3DRate);
						b.m_MatEnd = b.m_Mat;
						b.m_StartFrame = std::stoi(Args.at(9));
						b.m_EndFrame = std::stoi(Args.at(10));
					}
					if (Args.at(0) == "SetModelEnd") {
						for (auto& b : m_Models) {
							if (b.m_ObjPath == Args.at(1) && b.m_UniqueID == std::stoi(Args.at(2))) {
								b.m_MatEnd =
									Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), std::stof(Args.at(6))) *
									Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), std::stof(Args.at(7))) *
									Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), std::stof(Args.at(8))) *
									Util::Matrix4x4::Mtrans(Util::VECTOR3D::vget(std::stof(Args.at(3)), std::stof(Args.at(4)), std::stof(Args.at(5))) * Scale3DRate);
							}
						}
					}
					if (Args.at(0) == "SetModelAnimation") {
						for (auto& b : m_Models) {
							if (b.m_ObjPath == Args.at(1) && b.m_UniqueID == std::stoi(Args.at(2))) {
								b.m_AnimID = std::stoi(Args.at(3));
								break;
							}
						}
					}
				}
			}
			FileStream.Close();
		}
		m_Frame = 0.f;
		m_IsEnd = false;
	}
	void Init() noexcept {
		for (auto& b : m_Models) {
			if (b.m_ObjPath != "") {
				b.m_MovieObj = std::make_shared<MovieObj>();
				ObjectManager::Instance()->InitObject(b.m_MovieObj, b.m_MovieObj, b.m_ObjPath);
			}
		}
	}
	void Update() noexcept {
		for (int loop = 0; loop < static_cast<int>(m_StoryPop.size()); ++loop) {
			auto& Now = m_StoryPop.at(static_cast<size_t>(loop));
			if (Now.m_StartFrame <= static_cast<int>(m_Frame) && static_cast<int>(m_Frame) < Now.m_EndFrame) {
				float Per = static_cast<float>(m_Frame - Now.m_StartFrame) / static_cast<float>(Now.m_EndFrame - Now.m_StartFrame);
				auto* CameraParts = Camera::Camera3D::Instance();
				CameraParts->SetCamPos(
					Util::Lerp(Now.m_CameraPos, Now.m_CameraPosEnd, Per),
					Util::Lerp(Now.m_CameraTarget, Now.m_CameraTargetEnd, Per),
					Util::VECTOR3D::up());
				CameraParts->SetCamInfo(
					Util::Lerp(Now.m_CameraFov, Now.m_CameraFovEnd, Per),
					0.1f * Scale3DRate, 300.f * Scale3DRate);
				if (Now.m_CameraShakePow > 0.f) {
					CameraParts->SetCamShake(1.f, Now.m_CameraShakePow * Scale3DRate);
				}
				else {
					CameraParts->StopCamShake();
				}
			}
		}
		for (auto& m : m_Models) {
			if (m.m_StartFrame <= static_cast<int>(m_Frame) && (static_cast<int>(m_Frame) < m.m_EndFrame)) {
				float Per = static_cast<float>(m_Frame - m.m_StartFrame) / static_cast<float>(m.m_EndFrame - m.m_StartFrame);
				m.m_MovieObj->SetIsDraw(true);
				m.m_MovieObj->SetMatrix(Util::Lerp(m.m_Mat, m.m_MatEnd, Per));
				if (m.m_AnimID != InvalidID) {
					//アニメアップデート
					{
						for (size_t loop2 = 0; loop2 < static_cast<size_t>(m.m_MovieObj->GetModel().GetAnimNum()); ++loop2) {
							m.m_MovieObj->SetAnim(loop2).SetPer((m.m_AnimID == static_cast<int>(loop2)) ? 1.f : 0.f);
							if (m.m_AnimID == static_cast<int>(loop2)) {
								m.m_MovieObj->SetAnim(loop2).Update(false, 1.0f);
							}
						}
						m.m_MovieObj->SetModel().FlipAnimAll();
					}
				}
				if (m.m_StartFrame == static_cast<int>(m_Frame)) {
					m.m_MovieObj->SetPlanePosition(Util::Lerp(m.m_Mat, m.m_MatEnd, Per));
				}
			}
			else {
				m.m_MovieObj->SetIsDraw(false);
			}
		}
		auto& Next = m_StoryPop.back();
		if (Next.m_EndFrame - 60 <= static_cast<int>(m_Frame)) {
			m_IsEnd = true;
		}
		m_Frame += 1.f;
	}
};

class MovieScene : public Util::SceneBase {
	StoryScript						m_StoryScript;

	Util::VECTOR3D					CamPosition;
	Util::VECTOR3D					CamTarget;
	Util::VECTOR3D					CamUp;
	float							m_Fade{ 1.f };

	bool							m_Exit{ false };
	char		padding[7]{};

public:
	MovieScene(void) noexcept { SetID(static_cast<int>(EnumScene::Movie)); }
	MovieScene(const MovieScene&) = delete;
	MovieScene(MovieScene&&) = delete;
	MovieScene& operator=(const MovieScene&) = delete;
	MovieScene& operator=(MovieScene&&) = delete;
	virtual ~MovieScene(void) noexcept {}
protected:
	void Load_Sub(void) noexcept override;
	void Init_Sub(void) noexcept override;
	void Update_Sub(void) noexcept override;
	void BGDraw_Sub(void) noexcept override;
	void SetShadowDrawRigid_Sub(void) noexcept override;
	void SetShadowDraw_Sub(void) noexcept override;
	void Draw_Sub(void) noexcept override;
	void DrawFront_Sub(void) noexcept override;
	void DepthDraw_Sub(void) noexcept override;
	void ShadowDrawFar_Sub(void) noexcept override;
	void ShadowDraw_Sub(void) noexcept override;
	void UIDraw_Sub(void) noexcept override;
	void Dispose_Sub(void) noexcept override;
};
