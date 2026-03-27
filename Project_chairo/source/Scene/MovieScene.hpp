#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../Util/SceneManager.hpp"

#include "../MainScene/BaseObject.hpp"

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
public:
	void Load_Sub(void) noexcept override {}
	void Init_Sub(void) noexcept override {}
	void Update_Sub(void) noexcept override{
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
	}
	void ShadowDraw_Sub(void) const noexcept override {
		if (!m_IsDraw) { return; }
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
	}
};
struct StoryModel {
	std::string					m_ObjPath{ "" };
	std::shared_ptr<MovieObj>	m_MovieObj;
	Util::Matrix4x4				m_Mat;
	int							m_UniqueID{};
	int							m_AnimID{ InvalidID };
	//char		padding[4]{};
};
struct StoryPop {
	int				m_StartFrame{};
	char		padding[4]{};
	Util::VECTOR3D	m_CameraPos;
	Util::VECTOR3D	m_CameraTarget;
	std::vector<StoryModel>	m_Models{};
};
class StoryScript {
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
			File::InputFileStream FileStream;
			FileStream.Open("data/Story/" + Path + ".txt");
			while (true) {
				if (FileStream.ComeEof()) { break; }
				std::vector<std::string> Args;
				File::GetArgs(FileStream.SeekLineAndGetStr(), &Args);
				//
				{
					if (Args.at(0) == "SetCut") {
						m_StoryPop.emplace_back();
						m_StoryPop.back().m_StartFrame = std::stoi(Args.at(1));
					}
					if (Args.at(0) == "SetCameraPos") {
						m_StoryPop.back().m_CameraPos = Util::VECTOR3D::vget(std::stof(Args.at(1)), std::stof(Args.at(2)), std::stof(Args.at(3))) * Scale3DRate;
					}
					if (Args.at(0) == "SetCameraTarget") {
						m_StoryPop.back().m_CameraTarget = Util::VECTOR3D::vget(std::stof(Args.at(1)), std::stof(Args.at(2)), std::stof(Args.at(3))) * Scale3DRate;
					}
					if (Args.at(0) == "SetModel") {
						m_StoryPop.back().m_Models.emplace_back();
						auto& b = m_StoryPop.back().m_Models.back();
						b.m_ObjPath = Args.at(1);
						ObjectManager::Instance()->LoadModel(b.m_ObjPath);
						b.m_UniqueID = std::stoi(Args.at(2));
						b.m_Mat =
							Util::Matrix4x4::RotAxis(Util::VECTOR3D::right(), std::stof(Args.at(6))) *
							Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), std::stof(Args.at(7))) *
							Util::Matrix4x4::RotAxis(Util::VECTOR3D::forward(), std::stof(Args.at(8))) *
							Util::Matrix4x4::Mtrans(Util::VECTOR3D::vget(std::stof(Args.at(3)), std::stof(Args.at(4)), std::stof(Args.at(5))) * Scale3DRate);
					}
					if (Args.at(0) == "SetModelAnimation") {
						for (auto& b : m_StoryPop.back().m_Models) {
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
	}
	void Init() noexcept {
		for (auto& s : m_StoryPop) {
			for (auto& b : s.m_Models) {
				if (b.m_ObjPath != "") {
					b.m_MovieObj = std::make_shared<MovieObj>();
					ObjectManager::Instance()->InitObject(b.m_MovieObj, b.m_MovieObj, b.m_ObjPath);
				}
			}
		}
	}
	void Update() noexcept {
		for (int loop = 0; loop < static_cast<int>(m_StoryPop.size()); ++loop) {
			auto& Now = m_StoryPop.at(static_cast<size_t>(loop));
			bool IsEnd = true;
			if (static_cast<int>(m_StoryPop.size()) - 1 < loop) {
				auto& Next = m_StoryPop.at(static_cast<size_t>(loop + 1));
				IsEnd = (static_cast<int>(m_Frame) < Next.m_StartFrame);
			}
			if (Now.m_StartFrame <= static_cast<int>(m_Frame) && IsEnd) {
				auto* CameraParts = Camera::Camera3D::Instance();
				CameraParts->SetCamPos(Now.m_CameraPos, Now.m_CameraTarget, Util::VECTOR3D::up());

				for (auto& m : Now.m_Models) {
					m.m_MovieObj->SetIsDraw(true);
					m.m_MovieObj->SetMatrix(m.m_Mat);
					if (m.m_AnimID != InvalidID) {
						//アニメアップデート
						{
							for (size_t loop2 = 0; loop2 < static_cast<size_t>(m.m_MovieObj->GetModel().GetAnimNum()); ++loop2) {
								m.m_MovieObj->SetAnim(loop2).SetPer((m.m_AnimID == static_cast<int>(loop2)) ? 1.f : 0.f);
							}
							m.m_MovieObj->SetModel().FlipAnimAll();
						}
					}
				}
			}
			else {
				for (auto& m : Now.m_Models) {
					m.m_MovieObj->SetIsDraw(false);
				}
			}
		}
		auto& Next = m_StoryPop.back();
		if (Next.m_StartFrame <= static_cast<int>(m_Frame)) {
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
