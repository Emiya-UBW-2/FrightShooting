#pragma warning(disable:5259)

#include "BaseObject.hpp"

const ObjectManager* Util::SingletonBase<ObjectManager>::m_Singleton = nullptr;

inline void ResourceModel::LoadEnd(const std::shared_ptr<BaseObject>& pObj) noexcept {
	if (this->m_IsEndLoadData) { return; }
	{
		this->m_Frames.clear();
		if (pObj->GetFrameNum() > 0) {
			this->m_Frames.resize(static_cast<size_t>(pObj->GetFrameNum()));
			for (auto& f : this->m_Frames) {
				f.FrameID = InvalidID;
			}
			size_t count = 0;
			for (int frameNum = 0, Max = this->GetModel().GetFrameNum(); frameNum < Max; ++frameNum) {
				if (Util::UTF8toSjis(this->GetModel().GetFrameName(frameNum)) == pObj->GetFrameStr(static_cast<int>(count))) {
					// そのフレームを登録
					this->m_Frames[count].FrameID = frameNum;
					//this->m_Frames[count].LocalMat = Util::Matrix4x4::Mtrans(this->GetModel().GetFrameLocalMatrix(this->m_Frames[count].FrameID).pos());
					this->m_Frames[count].LocalMat = this->GetModel().GetFrameLocalMatrix(this->m_Frames[count].FrameID);
					this->m_Frames[count].WorldMat = this->GetModel().GetFrameLocalWorldMatrix(this->m_Frames[count].FrameID);
				}
				else if (frameNum < Max - 1) {
					continue;// 飛ばす
				}
				++count;
				frameNum = 0;
				if (count >= this->m_Frames.size()) {
					break;
				}
			}
		}
	}
	this->m_IsEndLoadData = true;
}


void			ObjectManager::AddObject(const SharedObj& NewObj) noexcept {
	for (auto& object : this->m_ObjectList) {
		if (object) { continue; }
		object = NewObj;
		object->SetObjectID(this->m_LastUniqueID);
		return;
	}
	this->m_ObjectList.emplace_back(NewObj);
	this->m_ObjectList.back()->SetObjectID(this->m_LastUniqueID);
	++this->m_LastUniqueID;
}
const SharedModel* ObjectManager::LoadModel(std::string_view path, std::string_view objname) noexcept {
	auto Find = std::find_if(this->m_ModelList.begin(), this->m_ModelList.end(), [&](const SharedModel& tgt) {return tgt && tgt->GetPathCompare(path, objname); });
	if (Find != this->m_ModelList.end()) {
		return &*Find;
	}
	else {
		this->m_ModelList.emplace_back(std::make_shared<ResourceModel>());
		this->m_ModelList.back()->LoadStart(PHYSICS_SETUP::DISABLE, path, objname);
		return  &this->m_ModelList.back();
	}
}
void			ObjectManager::LoadModelAfter(const SharedObj& pObj, const SharedObj& pAnim, std::string_view filepath, std::string_view objfilename) noexcept {
	const SharedModel* Ptr = LoadModel(filepath, objfilename);
	if (!(*Ptr)->GetIsEndLoadData()) {
		(*Ptr)->LoadEnd(pObj);
	}
	pObj->CopyModel(*Ptr);
	if (pAnim) {
		Draw::MV1::SetAnime(&pObj->SetModel(), pAnim->GetModel());
	}
}
//
void			ObjectManager::InitObject(const SharedObj& pObj) noexcept {
	AddObject(pObj);
	pObj->Load();
	pObj->Init();
}
void			ObjectManager::InitObject(const SharedObj& pObj, std::string_view filepath, std::string_view objfilename) noexcept {
	AddObject(pObj);
	LoadModelAfter(pObj, pObj, filepath, objfilename);
	pObj->SetFilePath(filepath);
	pObj->Load();
	pObj->Init();
}
void			ObjectManager::InitObject(const SharedObj& pObj, const SharedObj& pAnim, std::string_view filepath, std::string_view objfilename) noexcept {
	AddObject(pObj);
	LoadModelAfter(pObj, pAnim, filepath, objfilename);
	pObj->SetFilePath(filepath);
	pObj->Load();
	pObj->Init();
}
SharedObj* ObjectManager::GetObj(int UniqueID) noexcept {
	for (auto& object : this->m_ObjectList) {
		if (!object) { continue; }
		if (object->GetObjectID() == UniqueID) {
			return &object;
		}
	}
	return nullptr;
}
void ObjectManager::DelObj(const SharedObj& ptr) noexcept {
	//実体削除
	for (auto& object : this->m_ObjectList) {
		if (!object) { continue; }
		if (object == ptr) {
			object->Dispose();
			object.reset();
			break;
		}
	}
}
//
void			ObjectManager::UpdateObject(void) noexcept {
	// オブジェクトが増えた場合に備えて範囲forは使わない
	for (size_t loop = 0; loop < this->m_ObjectList.size(); ++loop) {
		auto& object = this->m_ObjectList[loop];
		if (!object) { continue; }
		object->Update();
	}
	// オブジェクトの排除チェック
	for (auto& object : this->m_ObjectList) {
		if (!object) { continue; }
		if (!object->GetIsDelete()) { continue; }
		object->Dispose();
		object.reset();
	}
}
void			ObjectManager::DeleteAll(void) noexcept {
	for (auto& object : this->m_ObjectList) {
		if (!object) { continue; }
		object->Dispose();
		object.reset();
	}
	this->m_ObjectList.clear();

	for (auto& model : this->m_ModelList) {
		if (!model) { continue; }
		model->DisposeModel();
		model.reset();
	}
	this->m_ModelList.clear();

	this->m_LastUniqueID = 0;// 一旦ユニークIDもリセット
}
