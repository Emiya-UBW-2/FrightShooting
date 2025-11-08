#pragma once
#define NOMINMAX
#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#pragma warning( push, 3 )
#include "DxLib.h"
#include<string>
#include<filesystem>
#pragma warning( pop )

#include "Enum.hpp"
#include "Algorithm.hpp"

namespace Util {
	// --------------------------------------------------------------------------------------------------
	// シングルトン
	// --------------------------------------------------------------------------------------------------
	template <class T>
	class SingletonBase {
	private:
		static const T*	m_Singleton;
	public:
		static void Create(void) noexcept {
			m_Singleton = new T();
		}
		static T* Instance(void) noexcept {
#if _DEBUG
			if (m_Singleton == nullptr) {
				MessageBox(NULL, "Failed Instance Create", "", MB_OK);
				exit(-1);
			}
#endif
			// if (m_Singleton == nullptr) { m_Singleton = new T(); }
			return (T*)m_Singleton;
		}
		static void Release(void) noexcept {
			delete m_Singleton;
		}
	protected:
		SingletonBase(void) noexcept {}
		virtual ~SingletonBase(void) noexcept {}
	private:
		SingletonBase(const SingletonBase&) = delete;
		SingletonBase& operator=(const SingletonBase&) = delete;
		SingletonBase(SingletonBase&&) = delete;
		SingletonBase& operator=(SingletonBase&&) = delete;
	};
	// 子のサンプル
	/*
	class A : public SingletonBase<A> {
	private:
		friend class SingletonBase<A>;
	}
	//*/

	// --------------------------------------------------------------------------------------------------
	// RGBA
	// --------------------------------------------------------------------------------------------------
	class ColorRGBA {
		uint32_t		color{};
		char		padding[4]{};
	public:
		ColorRGBA(int R, int G, int B, int A) noexcept {
			Set(R, G, B, A);
		}

		ColorRGBA(void) noexcept {}
		ColorRGBA(const ColorRGBA& o) noexcept {
			this->color = o.color;
		}
		ColorRGBA(ColorRGBA&& o) noexcept {
			this->color = o.color;
			o.color = 0;
		}
		ColorRGBA& operator=(const ColorRGBA& o) noexcept {
			this->color = o.color;
			return *this;
		}
		ColorRGBA& operator=(ColorRGBA&& o) noexcept {
			this->color = o.color;
			o.color = 0;
			return *this;
		}
		virtual ~ColorRGBA(void) noexcept {}
	public:
		int GetA(void) const noexcept { return static_cast<int>((color & 0xff000000) >> (8 * 3)); }
		int GetR(void) const noexcept { return static_cast<int>((color & 0x00ff0000) >> (8 * 2)); }
		int GetG(void) const noexcept { return static_cast<int>((color & 0x0000ff00) >> (8 * 1)); }
		int GetB(void) const noexcept { return static_cast<int>((color & 0x000000ff) >> (8 * 0)); }

		uint32_t Get(void) const noexcept { return color; }

		unsigned int GetColor(void) const noexcept { return	DxLib::GetColor(GetR(), GetG(), GetB()); }
	public:
		void SetA(int value) noexcept { color = (color & 0x00ffffff) | (static_cast<uint32_t>(std::clamp(value, 0, 255)) << (8 * 3)); }
		void SetR(int value) noexcept { color = (color & 0xff00ffff) | (static_cast<uint32_t>(std::clamp(value, 0, 255)) << (8 * 2)); }
		void SetG(int value) noexcept { color = (color & 0xffff00ff) | (static_cast<uint32_t>(std::clamp(value, 0, 255)) << (8 * 1)); }
		void SetB(int value) noexcept { color = (color & 0xffffff00) | (static_cast<uint32_t>(std::clamp(value, 0, 255)) << (8 * 0)); }

		void Set(int R, int G, int B, int A) noexcept {
			SetR(R);
			SetG(G);
			SetB(B);
			SetA(A);
		}
	public:
		ColorRGBA GetMult(const ColorRGBA& Target) const noexcept {
			ColorRGBA	Color{};
			Color.SetR(static_cast<int>(255.f * (static_cast<float>(this->GetR()) / 255.f * static_cast<float>(Target.GetR()) / 255.f)));
			Color.SetG(static_cast<int>(255.f * (static_cast<float>(this->GetG()) / 255.f * static_cast<float>(Target.GetG()) / 255.f)));
			Color.SetB(static_cast<int>(255.f * (static_cast<float>(this->GetB()) / 255.f * static_cast<float>(Target.GetB()) / 255.f)));
			Color.SetA(static_cast<int>(255.f * (static_cast<float>(this->GetA()) / 255.f * static_cast<float>(Target.GetA()) / 255.f)));
			return Color;
		}
	public:
		// 比較
		inline bool operator==(const ColorRGBA& obj) const noexcept { return (this->color == obj.color); }
		inline bool operator!=(const ColorRGBA& obj) const noexcept { return !(*this == obj); }
		// 加算
		inline ColorRGBA operator+(const ColorRGBA& obj) const noexcept {
			ColorRGBA Answer;
			Answer.SetR(this->GetR() + obj.GetR());
			Answer.SetG(this->GetG() + obj.GetG());
			Answer.SetB(this->GetB() + obj.GetB());
			Answer.SetA(this->GetA() + obj.GetA());
			return Answer;
		}
		inline void operator+=(const ColorRGBA& obj) noexcept {
			(*this) = (*this) + obj;
		}
		// 減算
		inline ColorRGBA operator-(const ColorRGBA& obj) const noexcept {
			ColorRGBA Answer;
			Answer.SetR(this->GetR() - obj.GetR());
			Answer.SetG(this->GetG() - obj.GetG());
			Answer.SetB(this->GetB() - obj.GetB());
			Answer.SetA(this->GetA() - obj.GetA());
			return Answer;
		}
		inline void operator-=(const ColorRGBA& obj) noexcept {
			(*this) = (*this) - obj;
		}
		// 乗算
		inline ColorRGBA operator*(float p1) const noexcept {
			ColorRGBA Answer;
			Answer.SetR(static_cast<int>(static_cast<float>(this->GetR()) * p1));
			Answer.SetG(static_cast<int>(static_cast<float>(this->GetG()) * p1));
			Answer.SetB(static_cast<int>(static_cast<float>(this->GetB()) * p1));
			Answer.SetA(static_cast<int>(static_cast<float>(this->GetA()) * p1));
			return Answer;
		}
		inline void operator*=(float p1) noexcept {
			(*this) = (*this) * p1;
		}
		// 除算
		inline ColorRGBA operator/(float p1) const noexcept { return *this * (1.f / p1); }
		inline void operator/=(float p1) noexcept { *this *= (1.f / p1); }
	public:
		void SetByJson(const std::vector<int>& value) noexcept {
			this->SetA(255);
			size_t loop = 0;
			for (const int& dp : value) {
				if (loop == 0) {
					this->SetR(dp);
				}
				else if (loop == 1) {
					this->SetG(dp);
				}
				else if (loop == 2) {
					this->SetB(dp);
				}
				else if (loop == 3) {
					this->SetA(dp);
				}
				++loop;
			}
		}
	};

	inline ColorRGBA Lerp(const ColorRGBA& A, const ColorRGBA& B, float Per) noexcept {
		ColorRGBA Answer;
		Answer.SetR(static_cast<int>(Lerp(static_cast<float>(A.GetR()), static_cast<float>(B.GetR()), Per)));
		Answer.SetG(static_cast<int>(Lerp(static_cast<float>(A.GetG()), static_cast<float>(B.GetG()), Per)));
		Answer.SetB(static_cast<int>(Lerp(static_cast<float>(A.GetB()), static_cast<float>(B.GetB()), Per)));
		Answer.SetA(static_cast<int>(Lerp(static_cast<float>(A.GetA()), static_cast<float>(B.GetA()), Per)));
		return Answer;
	}

	// --------------------------------------------------------------------------------------------------
	// 汎用ハンドル
	// --------------------------------------------------------------------------------------------------
	class DXHandle {
	private:
		int		m_handle{ InvalidID };
		char		padding[4]{};
	protected:
		constexpr DXHandle(int h) noexcept : m_handle(h) {}
	public:
		constexpr DXHandle(void) noexcept : m_handle(InvalidID) {}
		DXHandle(const DXHandle&) = delete;
		DXHandle(DXHandle&& o) noexcept {
			SetHandleDirect(o.get());
			o.SetHandleDirect(InvalidID);
		}
		DXHandle& operator=(const DXHandle&) = delete;
		DXHandle& operator=(DXHandle&& o) noexcept {
			SetHandleDirect(o.get());
			o.SetHandleDirect(InvalidID);
			return *this;
		}

		virtual ~DXHandle(void) noexcept {
			Dispose();
		}
	public:
		int get(void) const noexcept { return this->m_handle; }
		bool IsActive(void) const noexcept { return this->m_handle != InvalidID; }
		operator bool(void) const noexcept { return IsActive(); }
	public:
		void Dispose(void) noexcept {
			if (IsActive()) {
				Dispose_Sub();
				SetHandleDirect(InvalidID);
			}
		}
	protected:
		void SetHandleDirect(int handle) noexcept { this->m_handle = handle; }
		virtual void Dispose_Sub(void) noexcept {}
	};

	//
	static auto		UTF8toSjis(const std::string& srcUTF8) {
		//Unicodeへ変換後の文字列長を得る
		int lenghtUnicode = static_cast<int>(MultiByteToWideChar(CP_UTF8, 0, srcUTF8.c_str(), static_cast<int>(srcUTF8.size()) + 1, nullptr, 0));

		//必要な分だけUnicode文字列のバッファを確保
		wchar_t* bufUnicode = new wchar_t[static_cast<size_t>(lenghtUnicode)];

		//UTF8からUnicodeへ変換
		MultiByteToWideChar(CP_UTF8, 0, srcUTF8.c_str(), static_cast<int>(srcUTF8.size()) + 1, bufUnicode, lenghtUnicode);

		//ShiftJISへ変換後の文字列長を得る
		int lengthSJis = WideCharToMultiByte(CP_THREAD_ACP, 0, bufUnicode, -1, nullptr, 0, nullptr, nullptr);

		//必要な分だけShiftJIS文字列のバッファを確保
		char* bufShiftJis = new char[static_cast<size_t>(lengthSJis)];

		//UnicodeからShiftJISへ変換
		WideCharToMultiByte(CP_THREAD_ACP, 0, bufUnicode, lenghtUnicode + 1, bufShiftJis, lengthSJis, nullptr, nullptr);

		std::string strSJis(bufShiftJis);

		delete[] bufUnicode;
		delete[] bufShiftJis;

		return strSJis;
	}
	//
	static auto		SjistoUTF8(const std::string& srcSjis) {
		std::wstring wide;
		{
			int lengthSJis = MultiByteToWideChar(CP_ACP, 0U, srcSjis.c_str(), -1, nullptr, 0U);
			std::vector<wchar_t> dest(static_cast<size_t>(lengthSJis), L'\0');
			MultiByteToWideChar(CP_ACP, 0, srcSjis.c_str(), -1, dest.data(), static_cast<int>(dest.size()));
			dest.resize(std::char_traits<wchar_t>::length(dest.data()));
			dest.shrink_to_fit();
			wide = std::wstring(dest.begin(), dest.end());
		}
		std::string ret;
		{
			int lenghtUnicode = static_cast<int>(WideCharToMultiByte(CP_UTF8, 0U, wide.c_str(), -1, nullptr, 0, nullptr, nullptr));
			std::vector<char> dest(static_cast<size_t>(lenghtUnicode), '\0');
			WideCharToMultiByte(CP_UTF8, 0U, wide.c_str(), -1, dest.data(), static_cast<int>(dest.size()), nullptr, nullptr);
			dest.resize(std::char_traits<char>::length(dest.data()));
			dest.shrink_to_fit();
			ret = std::string(dest.begin(), dest.end());
		}
		return ret;
	}

	//

	//	o1 Angleが0の場合の左上座標
	//	o2 Angleが0の場合の右下座標
	//	minp 左上角からの固定長さ
	//	maxp 右下角からの固定長さ
	//	Center　　: 画像を回転描画する画像上の中心座標(左上を(0.0f,0.0f)、右下を(1.0f,1.0f)とした割合)
	//	Angle　　　: 描画角度（ラジアン指定）
	//	GrHandle　 : 描画するグラフィックの識別番号（グラフィックハンドル）
	//	TransFlag　: 画像の透明度を有効にするかどうか( TRUE：有効にする　FALSE：無効にする )
	//	TilingFlag : 角以外の部分をタイリングするか拡縮させるか( TRUE：タイリング　FALSE：拡縮 )
	static void Draw9SliceGraph(
		VECTOR2D o1, VECTOR2D o2,
		VECTOR2D minp, VECTOR2D maxp,
		VECTOR2D Center, float Angle,
		int GrHandle, bool TransFlag, bool TilingFlag) noexcept {
		//最低限のサイズを指定
		if (o2.x < o1.x + minp.x + maxp.x) { o2.x = o1.x + minp.x + maxp.x; }
		if (o2.y < o1.y + minp.y + maxp.y) { o2.y = o1.y + minp.y + maxp.y; }
		//用意する頂点情報
		std::vector<VERTEX2D> Vertex;
		std::vector<unsigned short> Index;

		float xs = (o2.x - o1.x);
		float ys = (o2.y - o1.y);

		VECTOR2D center;
		center.x = o1.x + xs * Center.x;
		center.y = o1.y + ys * Center.y;

		auto SetPoint = [&](float xper, float yper, int xc, int yc) {
			Vertex.resize(Vertex.size() + 1);
			VECTOR2D ofs;
			ofs.x = o1.x + xs * xper;
			ofs.y = o1.y + ys * yper;
			VECTOR2D pos = center + (ofs - center).Rotate(Angle);
			Vertex.back().pos = DxLib::VGet(pos.x, pos.y, 0.f);

			Vertex.back().rhw = 1.0f;
			Vertex.back().dif = DxLib::GetColorU8(255, 255, 255, 255);
			Vertex.back().u = static_cast<float>(xc) / 3.f;
			Vertex.back().v = static_cast<float>(yc) / 3.f;
			return (unsigned short)(Vertex.size() - 1);
			};
		auto SetBox = [&](float xmin, float ymin, float xmax, float ymax, int xc, int yc) {
			Index.emplace_back(SetPoint(xmin, ymin, xc, yc));// 左上の頂点の情報をセット
			auto RU = SetPoint(xmax, ymin, xc + 1, yc);
			auto LD = SetPoint(xmin, ymax, xc, yc + 1);
			Index.emplace_back(RU);// 右上の頂点の情報をセット
			Index.emplace_back(LD);// 左下の頂点の情報をセット
			Index.emplace_back(SetPoint(xmax, ymax, xc + 1, yc + 1));// 右下の頂点の情報をセット
			Index.emplace_back(LD);// 左下の頂点の情報をセット
			Index.emplace_back(RU);// 右上の頂点の情報をセット
			};

		float xminpt = minp.x / xs;
		float xmaxpt = maxp.x / xs;
		float xmaxt = 1.f - xmaxpt;
		float xmidt = xmaxt - xminpt;

		float yminpt = minp.y / ys;
		float ymaxpt = maxp.y / ys;
		float ymaxt = 1.f - ymaxpt;
		float ymidt = ymaxt - yminpt;

		int xtile = 1;
		int ytile = 1;
		//タイリング
		if (TilingFlag) {
			xtile = static_cast<int>(xmidt / ((xminpt + xmaxpt) / 2.f)) + 1;
			if (xtile <= 0) { xtile = 1; }
			ytile = static_cast<int>(ymidt / ((yminpt + ymaxpt) / 2.f)) + 1;
			if (ytile <= 0) { ytile = 1; }
		}

		Vertex.reserve((size_t)(3 * 2 * ((xtile + 2) * (ytile + 2))));
		float xmin = 0.f;
		float xmax = xminpt;
		int xc = 0;
		for (int x = 0; x < xtile + 2; ++x) {
			float ymin = 0.f;
			float ymax = yminpt;
			int yc = 0;
			for (int y = 0; y < ytile + 2; ++y) {
				SetBox(xmin, ymin, xmax, ymax, xc, yc);
				//次
				ymin = ymax;
				ymax = TilingFlag ? (ymin + ymidt / static_cast<float>(ytile)) : ymaxt;
				if (y == 0) {
					yc = 1;
				}
				if (y == ytile) {
					ymax = 1.f;
					yc = 2;
				}
			}
			//次
			xmin = xmax;
			xmax = TilingFlag ? (xmin + xmidt / static_cast<float>(xtile)) : xmaxt;
			if (x == 0) {
				xc = 1;
			}
			if (x == xtile) {
				xmax = 1.f;
				xc = 2;
			}
		}
		DxLib::DrawPolygonIndexed2D(Vertex.data(), static_cast<int>(Vertex.size()), Index.data(), static_cast<int>(Index.size()) / 3, GrHandle, TransFlag ? TRUE : FALSE);
	}

	// ディレクトリ内のファイル走査
	static void GetFileNamesInDirectory(const char* pPath, std::vector<WIN32_FIND_DATA>* pData) noexcept {
		pData->clear();
		WIN32_FIND_DATA win32fdt;
		HANDLE hFind = FindFirstFile(pPath, &win32fdt);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (win32fdt.cFileName[0] != '.') {
					pData->emplace_back(win32fdt);
				}

			} while (FindNextFile(hFind, &win32fdt));
		} // else{ return false; }
		FindClose(hFind);
	}

	// ファイルが存在するかチェック
	static bool IsFileExist(const char* Path) noexcept {
		return std::filesystem::is_regular_file(Path);
		//DXLIB_Initを通ってからでないと動作しない
		/*
		FILEINFO FileInfo;
		return (FileRead_findFirst(Path, &FileInfo) != (DWORD_PTR)InvalidID);
		//*/
	}

	// 直線と点の一番近い点
	static float GetMinLenSegmentToPoint(const Util::VECTOR3D& startpos, const Util::VECTOR3D& endpos, const Util::VECTOR3D& tgt) noexcept {
		return Segment_Point_MinLength(startpos.get(), endpos.get(), tgt.get());
	}
	// 直線と直線の一番近い距離
	static float GetMinLenSegmentToSegment(const Util::VECTOR3D& startpos, const Util::VECTOR3D& endpos, const Util::VECTOR3D& tgtstartpos, const Util::VECTOR3D& tgtendpos) noexcept {
		return Segment_Segment_MinLength(startpos.get(), endpos.get(), tgtstartpos.get(), tgtendpos.get());
	}
	// 線分同士の交差判定
	static bool GetSegmenttoSegment(const Util::VECTOR3D& SegmentAPos1, const Util::VECTOR3D& SegmentAPos2, const Util::VECTOR3D& SegmentBPos1, const Util::VECTOR3D& SegmentBPos2, SEGMENT_SEGMENT_RESULT* Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = SegmentBPos1.get();
		VECTOR PosBt = SegmentBPos2.get();

		Segment_Segment_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, Result);
		float len = 0.001f;
		return (Result->SegA_SegB_MinDist_Square <= (len * len));
	}
}
