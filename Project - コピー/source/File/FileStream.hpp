#pragma once
#pragma warning(disable:4514)
#pragma warning(disable:4505)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning( push, 3 )
#include <fstream>
#include <string>
#include <filesystem>
#pragma warning( pop )

namespace File {
#define USE_DXLIB (false)

#if USE_DXLIB
#define NOMINMAX
#pragma warning( push, 3 )
#include "DxLib.h"
#pragma warning( pop )
#endif

	//ファイル読み込み
	class InputFileStream {
#if USE_DXLIB
		int mdata{ InvalidID };
#else
		std::ifstream stream{};
#endif
	public:
		//コンストラクタ
		InputFileStream(void) noexcept {}
		//宣言時にファイルオープン版
		InputFileStream(std::string_view FilePath) noexcept { Open(FilePath); }

		InputFileStream(const InputFileStream&) = delete;
		InputFileStream(InputFileStream&&) = delete;
		InputFileStream& operator=(const InputFileStream&) = delete;
		InputFileStream& operator=(InputFileStream&&) = delete;

		//デストラクタ
		~InputFileStream(void) noexcept { Close(); }
	public:
		//ファイルを開き、探索ポイントを始点に移動
		void Open(std::string_view FilePath) noexcept {
#if USE_DXLIB
			mdata = DxLib::FileRead_open(FilePath, FALSE);
#else
			stream.open(FilePath);
#endif
		}
		// 1行そのまま取得し、次の行に探索ポイントを移る
		std::string SeekLineAndGetStr(void) noexcept {
#if USE_DXLIB
			const int charLength = 512;
			char mstr[charLength] = "";
			DxLib::FileRead_gets(mstr, charLength, mdata);
			return std::string(mstr);
#else
			std::string Buffer;
			std::getline(stream, Buffer);
			return Buffer;
#endif
		}
		// 探索ポイントが終端(EOF)で終わる
		bool ComeEof(void) const noexcept {
#if USE_DXLIB
			return DxLib::FileRead_eof(mdata) != 0;
#else
			return stream.eof();
#endif
		}
		//　閉じる
		void Close(void) noexcept {
#if USE_DXLIB
			if (mdata != InvalidID) {
				DxLib::FileRead_close(mdata);
				mdata = InvalidID;
			}
#else
			stream.close();
#endif
		}
	public:
		// 文字列から=より右の値取得
		static std::string getleft(const std::string& tempname, std::string_view DivWord) noexcept { return tempname.substr(0, tempname.find(DivWord)); }
		// 文字列から=より右の値取得
		static std::string getright(const std::string& tempname, std::string_view DivWord) noexcept { return tempname.substr(tempname.find(DivWord) + 1); }
	};

	//ファイル出力
	class OutputFileStream {
		std::ofstream stream{};
	public:
		//コンストラクタ
		OutputFileStream(void) noexcept {}
		//宣言時にファイルオープン版
		OutputFileStream(std::string_view FilePath) noexcept { Open(FilePath); }

		OutputFileStream(const OutputFileStream&) = delete;
		OutputFileStream(OutputFileStream&&) = delete;
		OutputFileStream& operator=(const OutputFileStream&) = delete;
		OutputFileStream& operator=(OutputFileStream&&) = delete;

		//デストラクタ
		~OutputFileStream(void) noexcept { Close(); }
	public:
		//ファイルを開き、探索ポイントを始点に移動
		void Open(std::string_view FilePath) noexcept {
			stream.open(FilePath);
		}
		// 1行書き込む
		void AddLine(const std::string& Str) noexcept {
			stream << Str + "\n";
		}
		//　閉じる
		void Close(void) noexcept {
			stream.close();
		}
	};

	// ファイルが存在するかチェック
	static bool IsFileExist(std::string_view Path) noexcept {
		return std::filesystem::is_regular_file(Path);
		//DXLIB_Initを通ってからでないと動作しない
		/*
		FILEINFO FileInfo;
		return (FileRead_findFirst(Path, &FileInfo) != (DWORD_PTR)-1);
		//*/
	}

}
