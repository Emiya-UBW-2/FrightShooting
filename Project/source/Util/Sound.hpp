#pragma once

#include "Util.hpp"

#pragma warning(disable:4710)
#pragma warning( push, 3 )
#include <array>
#include <vector>
#pragma warning( pop )

namespace Sound {
	// 共通のSEのEnum
	enum class SoundSelectCommon {
		UI_Select,
		UI_CANCEL,
		UI_OK,
		UI_NG,
		Num,
	};
	//サウンドの分類
	enum class SoundType {
		SE,
		BGM,
		VOICE,
	};
	typedef int64_t SoundUniqueID;
	// 1音声を管理するハンドル
	class SoundHandle : public Util::DXHandle {
	public:
		SoundHandle(void) noexcept {}
		SoundHandle(const SoundHandle&) = delete;
		SoundHandle(SoundHandle&& o) noexcept {
			Util::DXHandle::SetHandleDirect(o.get());
			o.SetHandleDirect(InvalidID);
		}
		SoundHandle& operator=(const SoundHandle&) = delete;
		SoundHandle& operator=(SoundHandle&& o) noexcept {
			Util::DXHandle::SetHandleDirect(o.get());
			o.SetHandleDirect(InvalidID);
			return *this;
		}
		virtual ~SoundHandle(void) noexcept {}
	protected:
		// 破棄する際に呼ばれる関数
		void	Dispose_Sub(void) noexcept override {
			DeleteSoundMem(Util::DXHandle::get());
		}
	public:// 変更系
		// サウンドを再生
		bool	Play(int type, int flag = 1) const noexcept { return (PlaySoundMem(Util::DXHandle::get(), type, flag) == 0); }
		void	Play3D(const Util::VECTOR3D& pos, float radius, int type_t = DX_PLAYTYPE_BACK)const {
			SetPosition(pos);
			Radius(radius);
			Play(type_t, TRUE);
		}
		// サウンドを停止
		bool	Stop(void) const noexcept { return (InvalidID != Util::DXHandle::get()) ? (StopSoundMem(Util::DXHandle::get()) == 0) : false; }
		// サウンドの音量を設定(0~255)
		bool	SetVol(int vol) const noexcept { return (ChangeVolumeSoundMem(std::clamp(vol, 0, 255), Util::DXHandle::get()) == 0); }
		// サウンドの左右バランスを決定(-255~255)
		bool	Pan(int panpal) const noexcept { return (ChangePanSoundMem(panpal, Util::DXHandle::get()) == 0); }

		bool	SetPosition(const Util::VECTOR3D& pos) const noexcept { return (Set3DPositionSoundMem(pos.get(), Util::DXHandle::get()) == 0); }
		bool	Radius(float radius) const noexcept { return (Set3DRadiusSoundMem(radius, Util::DXHandle::get()) == 0); }
		bool	PresetReverbParam(int PresetNo) const noexcept { return (Set3DPresetReverbParamSoundMem(PresetNo, Util::DXHandle::get()) == 0); }
	public:// 取得系
		// サウンドが再生中かどうかを取得
		bool	CheckPlay(void) const noexcept { return (DxLib::CheckSoundMem(Util::DXHandle::get()) == TRUE); }
		// ミリ秒単位で総裁性時間を取得
		LONGLONG GetTotalTIme(void) const noexcept { return DxLib::GetSoundTotalTime(Util::DXHandle::get()); }
		// サウンドの音量を取得
		int		GetVol(void) const noexcept { return GetVolumeSoundMem2(Util::DXHandle::get()); }
	public:// 読み込み系
		// ファイル名からサウンドを読み込み
		void Load(const std::basic_string<TCHAR>& FileName, int BufferNum = 3) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::LoadSoundMemWithStrLen(FileName.data(), FileName.length(), BufferNum));
		}
		// 渡されたサウンドハンドルと同じデータを自分にも複製して別ハンドルとして持つ
		void Duplicate(const SoundHandle& o) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::DuplicateSoundMem(o.get()));
		}
	};
	// 同じ音声を多重に持つクラス(1ハンドルで鳴らせる音は一つであるため)
	class SoundHandles {
		std::vector<SoundHandle>	m_HandleList;
		size_t						m_Size = 0;
		size_t						m_nowSelect = 0;
	public:
		// コンストラクタ
		SoundHandles(size_t buffersize, std::string path_t, bool is3Dsound = true) noexcept {
			if (is3Dsound) {
				SetCreate3DSoundFlag(TRUE);
			}
			this->m_Size = buffersize;
			this->m_HandleList.resize(this->m_Size);
			this->m_HandleList[0].Load(path_t);
			for (size_t loop = 1; loop < this->m_Size; ++loop) {
				this->m_HandleList[loop].Duplicate(this->m_HandleList[0]);
			}
			SetCreate3DSoundFlag(FALSE);
		}
		SoundHandles(void) noexcept = delete;
		SoundHandles(const SoundHandles&) = delete;// コピーしてはいけないのですべてdelete
		SoundHandles(SoundHandles&&) = delete;
		SoundHandles& operator=(const SoundHandles&) = delete;
		SoundHandles& operator=(SoundHandles&&) = delete;
		// デストラクタ
		~SoundHandles(void) noexcept {
			StopAll();
			this->m_HandleList.clear();
		}
	public:
		// サウンドが再生中かどうかを取得
		bool	CheckPlay(void) const noexcept { return this->m_HandleList[this->m_nowSelect].CheckPlay(); }
		// サウンドを一つ再生
		size_t	Play(int type_t = DX_PLAYTYPE_BACK, int Flag_t = 1, int panpal = -256) noexcept {
			auto Answer = this->m_nowSelect;
			auto& NowHandle = this->m_HandleList[this->m_nowSelect];
			NowHandle.Play(type_t, Flag_t);
			if (panpal != -256) { NowHandle.Pan(panpal); }
			++this->m_nowSelect %= this->m_Size;
			return Answer;
		}
		size_t	Play3D(const Util::VECTOR3D& pos_t, float radius, int type_t = DX_PLAYTYPE_BACK) noexcept {
			auto Answer = this->m_nowSelect;
			this->m_HandleList[this->m_nowSelect].Play3D(pos_t, radius, type_t);
			++this->m_nowSelect %= this->m_Size;
			return Answer;
		}
		void			SetPosition(const Util::VECTOR3D& pos_t) noexcept {
			for (auto& h : this->m_HandleList) {
				h.SetPosition(pos_t);
			}
		}
		// サウンドをすべて停止
		void			StopAll(void) noexcept {
			for (auto& h : this->m_HandleList) {
				h.Stop();
			}
		}
		// サウンドの音量をすべて変更
		void			SetVolAll(int vol) noexcept {
			for (auto& h : this->m_HandleList) {
				h.SetVol(vol);
			}
		}
	public:
		// ミリ秒単位で総再生時間を取得
		LONGLONG		GetTotalTIme(void) noexcept { return this->m_HandleList[0].GetTotalTIme(); }
	};
	// SoundTypeごとに分かれた音声管理クラス
	class Soundhave {
	private:
		SoundUniqueID					m_SoundID{ 0 };		//サウンドの識別用ID
		std::unique_ptr<SoundHandles>	m_Handles;				//実際に音声を持っているハンドル
		int								m_LocalVolume = 255;	//音声単位で指定できる音量(フェードアウトなどで使用)
		SoundType						m_SoundType{ SoundType::SE };//自分の音声タイプ

		size_t							m_buffersize{};
		std::string						m_path_t{};
		bool							m_is3Dsound = true;
		char		padding[7]{};
	public:
		//コンストラクタ
		Soundhave(SoundUniqueID SoundIDSelect, size_t buffersize, std::string path_t, SoundType soundType, bool is3Dsound = true) noexcept {
			// パスに何も描かれていない場合は特に何もしない
			if (path_t == "") { return; }
			this->m_SoundID = SoundIDSelect;
			this->m_Handles = std::make_unique<SoundHandles>(buffersize, path_t, is3Dsound);
			this->m_SoundType = soundType;
			FlipVolume();
			this->m_buffersize = buffersize;
			this->m_path_t = path_t;
			this->m_is3Dsound = is3Dsound;
		}
		Soundhave(const Soundhave&) = delete;
		Soundhave(Soundhave&&) = delete;
		Soundhave& operator=(const Soundhave&) = delete;
		Soundhave& operator=(Soundhave&&) = delete;
		// デストラクタ
		~Soundhave(void) noexcept {
			this->m_Handles.reset();
		}
	public:
		//サウンドの識別用ID
		const auto& GetSoundID(void)const noexcept { return this->m_SoundID; }
		bool Equal(size_t buffersize, std::string path_t, SoundType soundType, bool is3Dsound = true) const noexcept {
			if (this->m_buffersize != buffersize) { return false; }
			if (this->m_SoundType != soundType) { return false; }
			if (this->m_is3Dsound != is3Dsound) { return false; }
			if (this->m_path_t != path_t) { return false; }
			return true;
		}
	public:
		// サウンドが再生中かどうかを取得
		bool			CheckPlay(void) const noexcept { return this->m_Handles->CheckPlay(); }
		//再生中の音声をすべて停止
		void			StopAll(void) noexcept { this->m_Handles->StopAll(); }
		//再生
		size_t			Play(int type_t = DX_PLAYTYPE_BACK, int Flag_t = TRUE, int panpal = -256) noexcept { return this->m_Handles->Play(type_t, Flag_t, panpal); }
		size_t			Play3D(const Util::VECTOR3D& pos_t, float radius, int type_t = DX_PLAYTYPE_BACK) noexcept { return this->m_Handles->Play3D(pos_t, radius, type_t); }
		void			SetPosition(const Util::VECTOR3D& pos_t) noexcept { this->m_Handles->SetPosition(pos_t); }
		// 音声の総裁性時間をミリ秒で取得
		LONGLONG		GetTotalTIme(void) noexcept { return this->m_Handles->GetTotalTIme(); }
		// その音量でのみの音量設定
		void			SetLocalVolume(int vol) noexcept {
			this->m_LocalVolume = std::clamp(vol, 0, 255);
			FlipVolume();
		}
		// 保持している音声すべての音量を反映
		void			FlipVolume(void) noexcept;
	};
	// SE,BGMのプール
	class SoundPool : public Util::SingletonBase<SoundPool> {
	private:
		friend class Util::SingletonBase<SoundPool>;
	private:
		//各種サウンドを保持しておくリスト
		std::array<std::vector<std::unique_ptr<Soundhave>>, 3>	m_SoundHas;
		SoundUniqueID											m_SoundID{ 0 };		//サウンドの識別用ID
	private:
		// コンストラクタ
		SoundPool(void) noexcept {
			this->m_SoundID = 0;
			for (auto& hs : this->m_SoundHas) {
				hs.reserve(128);
			}
		}// コピーしてはいけないので通常のコンストラクタ以外をすべてdelete
		SoundPool(const SoundPool&) = delete;
		SoundPool(SoundPool&&) = delete;
		SoundPool& operator=(const SoundPool&) = delete;
		SoundPool& operator=(SoundPool&&) = delete;
		// デストラクタはシングルトンなので呼ばれません
	public:
		// 特定のIDにサウンドを追加
		SoundUniqueID			Add(SoundType Type, size_t buffersize, std::string path_t, bool is3Dsound = true) noexcept {
			auto ID = this->m_SoundID;
			//末尾に追加
			this->m_SoundHas[static_cast<size_t>(Type)].emplace_back(std::make_unique<Soundhave>(this->m_SoundID, buffersize, path_t, Type, is3Dsound));
			++m_SoundID;
			return ID;
		}
		// 特定のIDのサウンドを取得
		SoundUniqueID GetUniqueID(SoundType Type, size_t buffersize, std::string path_t, bool is3Dsound = true) noexcept {
			for (auto& h : this->m_SoundHas[static_cast<size_t>(Type)]) {
				if (h->Equal(buffersize, path_t, Type, is3Dsound)) {
					return h->GetSoundID();
				}
			}
			return Add(Type, buffersize, path_t, is3Dsound);
		}
		std::unique_ptr<Soundhave>& Get(SoundType Type, SoundUniqueID UniqueID) noexcept {
			for (auto& h : this->m_SoundHas[static_cast<size_t>(Type)]) {
				if (h->GetSoundID() == UniqueID) {
					return h;
				}
			}
			return this->m_SoundHas[static_cast<size_t>(Type)][0];// 探したいサウンドがなかった　エラー処理は特にしていませんので注意
		}
		// 保持している音声すべての音量を反映
		void			FlipVolume(void) noexcept {
			for (auto& s : this->m_SoundHas) {
				for (auto& h : s) {
					h->FlipVolume();
				}
			}
		}
		// 
		void			StopAll(SoundType Type) noexcept {
			for (auto& h : this->m_SoundHas[static_cast<size_t>(Type)]) {
				h->StopAll();
			}
		}
		// 特定のIDのサウンドを探し出して削除
		void			Delete(SoundType Type, SoundUniqueID Select) noexcept {
			for (auto& h : this->m_SoundHas[static_cast<size_t>(Type)]) {
				if (h->GetSoundID() == Select) {
					h.reset();
					std::swap(h, this->m_SoundHas[static_cast<size_t>(Type)].back());
					this->m_SoundHas[static_cast<size_t>(Type)].pop_back();
					break;
				}
			}
		}
	};
};
