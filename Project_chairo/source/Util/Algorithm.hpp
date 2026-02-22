#pragma once
#define NOMINMAX
#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#pragma warning( push, 3 )
#include "DxLib.h"
#include <vector>
#include <cmath>
#include <algorithm>
#pragma warning( pop )

namespace Util {
	// --------------------------------------------------------------------------------------------------
	// Vector2
	// --------------------------------------------------------------------------------------------------
	class VECTOR2D {
	public:
		float x{ 0 };
		float y{ 0 };
	public:
		VECTOR2D(void) noexcept : x(0), y(0) {}
		VECTOR2D(DxLib::VECTOR value) noexcept { this->Set(value.x, value.y); }
		inline DxLib::VECTOR get(void) const noexcept { return DxLib::VGet(this->x, this->y, 0.f); }				// 変換
		inline static const VECTOR2D vget(float x, float y) noexcept { return DxLib::VGet(x, y, 0.f); }	// 入力
		// 
		static const VECTOR2D down(void) noexcept { return vget(0.f, -1.f); }
		static const VECTOR2D left(void) noexcept { return vget(-1.f, 0.f); }
		static const VECTOR2D one(void) noexcept { return vget(1.f, 1.f); }
		static const VECTOR2D right(void) noexcept { return vget(1.f, 0.f); }
		static const VECTOR2D up(void) noexcept { return vget(0.f, 1.f); }
		static const VECTOR2D zero(void) noexcept { return vget(0.f, 0.f); }
		// 
		inline float magnitude(void) const noexcept {
			float Square = sqrMagnitude();
			return Square < 0.0000001f ? 0.0f : std::sqrtf(Square);
		}// サイズ
		inline VECTOR2D normalized(void) const noexcept {
			float Size = magnitude();
			if (Size <= 0.0f) {
				return vget(-1.0f, -1.0f);
			}
			return *this / Size;
		}		// 正規化
		inline float sqrMagnitude(void) const noexcept { return this->x * this->x + this->y * this->y; }		// サイズ
		// 
		bool Equals(const VECTOR2D& obj) const noexcept { return *this == obj; }
		void Set(float x1, float y1) noexcept {
			this->x = x1;
			this->y = y1;
		}

		inline static float			Angle(const VECTOR2D& A, const VECTOR2D& B) noexcept { return std::acos(Dot(A, B) / (A.magnitude() * B.magnitude())); }
		inline static VECTOR2D		ClampMagnitude(const VECTOR2D& A, float Limit) noexcept { return A.normalized() * std::clamp(A.magnitude(), 0.f, Limit); }
		inline static float			Cross(const VECTOR2D& A, const VECTOR2D& B) noexcept { return A.x * B.y - A.y * B.x; }
		inline static float			Distance(const VECTOR2D& A, const VECTOR2D& B) noexcept { return (A - B).magnitude(); }
		inline static float			Dot(const VECTOR2D& A, const VECTOR2D& B) noexcept { return A.x * B.x + A.y * B.y; }
		//static VECTOR2D			LerpVector2DX(const VECTOR2D& A, const VECTOR2D& B, float Per) noexcept { return Lerp<VECTOR2D>(A, B, Per); }
		//static VECTOR2D			LerpUnclampedVector2DX(const VECTOR2D& A, const VECTOR2D& B, float Per) noexcept { return Lerp<VECTOR2D>(A, B, Per); }
		inline static VECTOR2D		Max(const VECTOR2D& A, const VECTOR2D& B) noexcept { return vget(std::max(A.x, B.x), std::max(A.y, B.y)); }
		inline static VECTOR2D		Min(const VECTOR2D& A, const VECTOR2D& B) noexcept { return vget(std::min(A.x, B.x), std::min(A.y, B.y)); }
		// MoveTowards
		inline static VECTOR2D		Normalize(const VECTOR2D& A) noexcept { return A.normalized(); }
		// OrthoNormalize
		// Project
		// ProjectOnPlane
		inline static VECTOR2D		Reflect(const VECTOR2D& inDirection, const VECTOR2D& inNormal) noexcept { return inDirection + inNormal * (Dot(inNormal, inDirection * -1.f)) * 2.f; }
		// RotateTowards
		inline static VECTOR2D		Scale(const VECTOR2D& A, const VECTOR2D& B) noexcept { return vget((A.x * B.x), (A.y * B.y)); }
		inline static float			SignedAngle(const VECTOR2D& A, const VECTOR2D& B) noexcept { return std::atan2f(Cross(A, B), Dot(A, B)); }
		// Slerp
		// SlerpUnclamped
		// SmoothDamp

		// 比較
		inline bool operator==(const VECTOR2D& obj) const noexcept { return (*this - obj).magnitude() <= 0.001f; }
		inline bool operator!=(const VECTOR2D& obj) const noexcept { return !(*this == obj); }
		// 加算
		inline VECTOR2D operator+(const VECTOR2D& obj) const noexcept { return vget(this->x + obj.x, this->y + obj.y); }
		inline void operator+=(const VECTOR2D& obj) noexcept {
			this->x += obj.x;
			this->y += obj.y;
		}
		// 減算
		inline VECTOR2D operator-(const VECTOR2D& obj) const noexcept { return vget(this->x - obj.x, this->y - obj.y); }
		inline void operator-=(const VECTOR2D& obj) noexcept {
			this->x -= obj.x;
			this->y -= obj.y;
		}
		// 乗算
		inline VECTOR2D operator*(float p1) const noexcept { return vget(this->x * p1, this->y * p1); }
		inline void operator*=(float p1) noexcept {
			this->x *= p1;
			this->y *= p1;
		}
		// 除算
		inline VECTOR2D operator/(float p1) const noexcept { return *this * (1.f / p1); }
		inline void operator/=(float p1) noexcept { *this *= (1.f / p1); }
	public:
		inline VECTOR2D			Rotate(float Rad) const noexcept {
			VECTOR2D Answer;
			Answer.x = this->x * std::cos(Rad) - this->y * std::sin(Rad);
			Answer.y = this->x * std::sin(Rad) + this->y * std::cos(Rad);
			return Answer;
		}
		void SetByJson(const std::vector<float>& value) noexcept {
			size_t loop = 0;
			for (const float& dp : value) {
				if (loop == 0) {
					this->x = dp;
				}
				else if (loop == 1) {
					this->y = dp;
				}
				++loop;
			}
		}
	};

	// ---------------------------------------------------------------------------------------------
	// Vector3
	// ---------------------------------------------------------------------------------------------
	class VECTOR3D {
	public:
		float x{ 0 };
		float y{ 0 };
		float z{ 0 };
	public:
		VECTOR3D(void) noexcept : x(0), y(0), z(0) {}
		VECTOR3D(DxLib::VECTOR value) noexcept { this->Set(value.x, value.y, value.z); }
		inline DxLib::VECTOR get(void) const noexcept { return DxLib::VGet(this->x, this->y, this->z); }					// 変換
		inline static const VECTOR3D vget(float x, float y, float z) noexcept { return DxLib::VGet(x, y, z); }	// 入力
		// 
		inline static const VECTOR3D back(void) noexcept { return vget(0.f, 0.f, -1.f); }
		inline static const VECTOR3D down(void) noexcept { return vget(0.f, -1.f, 0.f); }
		inline static const VECTOR3D forward(void) noexcept { return vget(0.f, 0.f, 1.f); }
		inline static const VECTOR3D left(void) noexcept { return vget(-1.f, 0.f, 0.f); }

		inline static const VECTOR3D one(void) noexcept { return vget(1.f, 1.f, 1.f); }

		inline static const VECTOR3D right(void) noexcept { return vget(1.f, 0.f, 0.f); }
		inline static const VECTOR3D up(void) noexcept { return vget(0.f, 1.f, 0.f); }
		inline static const VECTOR3D zero(void) noexcept { return vget(0.f, 0.f, 0.f); }
		// 
		inline float magnitude(void) const noexcept {
			float Square = sqrMagnitude();
			return Square < 0.0000001f ? 0.0f : std::sqrtf(Square);
		}// サイズ
		inline VECTOR3D normalized(void) const noexcept {
			float Size = magnitude();
			if (Size <= 0.0f) {
				return vget(-1.0f, -1.0f, -1.0f);
			}
			return *this / Size;
		}		// 正規化
		inline float sqrMagnitude(void) const noexcept { return this->x * this->x + this->y * this->y + this->z * this->z; }		// サイズ
		// 
		inline bool Equals(const VECTOR3D& obj) const noexcept { return *this == obj; }
		inline void Set(float x1, float y1, float z1) noexcept {
			this->x = x1;
			this->y = y1;
			this->z = z1;
		}
		// 
		inline static float			Angle(const VECTOR3D& A, const VECTOR3D& B) noexcept { return std::acos(Dot(A, B) / (A.magnitude() * B.magnitude())); }
		inline static VECTOR3D		ClampMagnitude(const VECTOR3D& A, float Limit) noexcept { return A.normalized() * std::clamp(A.magnitude(), 0.f, Limit); }
		inline static VECTOR3D		Cross(const VECTOR3D& A, const VECTOR3D& B) noexcept { return vget(A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x); }
		inline static float			Distance(const VECTOR3D& A, const VECTOR3D& B) noexcept { return (A - B).magnitude(); }
		inline static float			Dot(const VECTOR3D& A, const VECTOR3D& B) noexcept { return A.x * B.x + A.y * B.y + A.z * B.z; }
		//static VECTOR3D			LerpUtil::VECTOR3D(const VECTOR3D& A, const VECTOR3D& B, float Per) noexcept { return Lerp<VECTOR3D>(A, B, Per); }
		//static VECTOR3D			LerpUnclampedUtil::VECTOR3D(const VECTOR3D& A, const VECTOR3D& B, float Per) noexcept { return Lerp<VECTOR3D>(A, B, Per); }
		inline static VECTOR3D		Max(const VECTOR3D& A, const VECTOR3D& B) noexcept { return vget(std::max(A.x, B.x), std::max(A.y, B.y), std::max(A.z, B.z)); }
		inline static VECTOR3D		Min(const VECTOR3D& A, const VECTOR3D& B) noexcept { return vget(std::min(A.x, B.x), std::min(A.y, B.y), std::min(A.z, B.z)); }
		// MoveTowards
		inline static VECTOR3D		Normalize(const VECTOR3D& A) noexcept { return A.normalized(); }
		// OrthoNormalize
		// Project
		// ProjectOnPlane
		inline static VECTOR3D		Reflect(const VECTOR3D& inDirection, const VECTOR3D& inNormal) noexcept { return inDirection + inNormal * (Dot(inNormal, inDirection * -1.f)) * 2.f; }
		// RotateTowards
		inline static VECTOR3D		Scale(const VECTOR3D& A, const VECTOR3D& B) noexcept { return vget((A.x * B.x), (A.y * B.y), (A.z * B.z)); }
		inline static float			SignedAngle(const VECTOR3D& A, const VECTOR3D& B, const VECTOR3D& Axis) noexcept { return Angle(A, B) * (Dot(Cross(Axis, A), B) > 0.f ? 1.f : -1.f); }
		// Slerp
		// SlerpUnclamped
		// SmoothDamp

		// 比較
		inline bool operator==(const VECTOR3D& obj) const noexcept { return (*this - obj).magnitude() <= 0.001f; }
		inline bool operator!=(const VECTOR3D& obj) const noexcept { return !(*this == obj); }
		// 加算
		inline VECTOR3D operator+(const VECTOR3D& obj) const noexcept { return vget(this->x + obj.x, this->y + obj.y, this->z + obj.z); }
		inline void operator+=(const VECTOR3D& obj) noexcept {
			this->x += obj.x;
			this->y += obj.y;
			this->z += obj.z;
		}
		// 減算
		inline VECTOR3D operator-(const VECTOR3D& obj) const noexcept { return vget(this->x - obj.x, this->y - obj.y, this->z - obj.z); }
		inline void operator-=(const VECTOR3D& obj) noexcept {
			this->x -= obj.x;
			this->y -= obj.y;
			this->z -= obj.z;
		}
		// 乗算
		inline VECTOR3D operator*(float p1) const noexcept { return vget(this->x * p1, this->y * p1, this->z * p1); }
		inline void operator*=(float p1) noexcept {
			this->x *= p1;
			this->y *= p1;
			this->z *= p1;
		}
		// 除算
		inline VECTOR3D operator/(float p1) const noexcept { return *this * (1.f / p1); }
		inline void operator/=(float p1) noexcept { *this *= (1.f / p1); }

		//
		inline bool IsRangeSmaller(float Lange) const noexcept {
			if (std::abs(this->x) + std::abs(this->y) + std::abs(this->z) > Lange * 3) {
				return false;
			}

			return this->sqrMagnitude() < Lange * Lange;
		}
	};

	// ---------------------------------------------------------------------------------------------
	// Matrix 4x4バージョン
	// ---------------------------------------------------------------------------------------------
	class Matrix4x4 {
		DxLib::MATRIX value;
	public:
		Matrix4x4(void) noexcept : value(DxLib::MGetIdent()) {}
		Matrix4x4(DxLib::MATRIX value) noexcept { this->value = value; }
		DxLib::MATRIX get(void) const noexcept { return this->value; }		// 変換
		// 
		static Matrix4x4 identity(void) noexcept { return DxLib::MGetIdent(); }

		static Matrix4x4 Axis1(const VECTOR3D& yvec, const VECTOR3D& zvec, const VECTOR3D& pos = VECTOR3D::zero()) noexcept { return { DxLib::MGetAxis1(VECTOR3D::Cross(yvec, zvec).get(),yvec.get(),zvec.get(),pos.get()) }; }
		static Matrix4x4 Axis2(const VECTOR3D& yvec, const VECTOR3D& zvec, const VECTOR3D& pos = VECTOR3D::zero()) noexcept { return { DxLib::MGetAxis2(VECTOR3D::Cross(yvec, zvec).get(),yvec.get(),zvec.get(),pos.get()) }; }
		static Matrix4x4 RotAxis(const VECTOR3D& p1, float p2) noexcept { return DxLib::MGetRotAxis(p1.get(), p2); }
		static Matrix4x4 RotVec2(const VECTOR3D& p1, const VECTOR3D& p2) noexcept { return { DxLib::MGetRotVec2(p1.get(), p2.get()) }; }
		static Matrix4x4 GetScale(float scale) noexcept { return { DxLib::MGetScale(DxLib::VGet(scale,scale,scale)) }; }
		static Matrix4x4 GetScale(const VECTOR3D& scale) noexcept { return { DxLib::MGetScale(scale.get()) }; }
		static Matrix4x4 Mtrans(const VECTOR3D& p1) noexcept { return DxLib::MGetTranslate(p1.get()); }
		static VECTOR3D Vtrans(const VECTOR3D& p1, const Matrix4x4& p2) noexcept { return DxLib::VTransform(p1.get(), p2.get()); }
		// 
		Matrix4x4 inverse(void) const noexcept { return DxLib::MInverse(this->get()); }		// 逆
		bool isIdentity(void) const noexcept { return *this == DxLib::MGetIdent(); }		// 逆
		// lossyScale
		VECTOR3D lossyScale(void) const noexcept { return DxLib::MGetSize(this->get()); }
		Matrix4x4 rotation(void) const noexcept { return DxLib::MGetRotElem(this->get()); }		// 逆
		Matrix4x4 transpose(void) const noexcept { return DxLib::MTranspose(this->get()); }		// 逆

		VECTOR3D pos(void) const noexcept { return Vtrans(VECTOR3D::zero(), this->get()); }
		VECTOR3D xvec(void) const noexcept { return Vtrans(VECTOR3D::right(), rotation()); }
		VECTOR3D yvec(void) const noexcept { return Vtrans(VECTOR3D::up(), rotation()); }
		VECTOR3D zvec(void) const noexcept { return Vtrans(VECTOR3D::forward(), rotation()); }
		VECTOR3D zvec2(void) const noexcept { return Vtrans(VECTOR3D::back(), rotation()); }//左手座標系で右手座標系のキャラを描画した際の正面
		// 
		void GetRadian(float* angle_x, float* angle_y, float* angle_z) const noexcept {
			constexpr float threshold = 0.001f;
			if (std::abs(value.m[1][2] - 1.0f) < threshold) { // R(2,1) = sin(x) = 1の時
				if (angle_x) {
					*angle_x = DX_PI_F / 2.f;
				}
				if (angle_y) {
					*angle_y = 0.f;
				}
				if (angle_z) {
					*angle_z = std::atan2f(value.m[0][1], value.m[0][0]);
				}
			}
			else if (std::abs(value.m[1][2] + 1.0f) < threshold) { // R(2,1) = sin(x) = -1の時
				if (angle_x) {
					*angle_x = -DX_PI_F / 2.f;
				}
				if (angle_y) {
					*angle_y = 0.f;
				}
				if (angle_z) {
					*angle_z = std::atan2f(value.m[0][1], value.m[0][0]);
				}
			}
			else {
				if (angle_x) {
					*angle_x = std::asinf(value.m[1][2]);
				}
				if (angle_y) {
					*angle_y = std::atan2f(-value.m[0][2], value.m[2][2]);
				}
				if (angle_z) {
					*angle_z = std::atan2f(-value.m[1][0], value.m[1][1]);
				}
			}
		}
		void SetRadian(float x, float y, float z) noexcept {
			value.m[0][0] = std::cos(y) * std::cos(z) - std::sin(x) * std::sin(y) * std::sin(z);
			value.m[1][0] = -std::cos(x) * std::sin(z);
			value.m[2][0] = std::sin(y) * std::cos(z) + std::sin(x) * std::cos(y) * std::sin(z);
			value.m[0][1] = std::cos(y) * std::sin(z) + std::sin(x) * std::sin(y) * std::cos(z);
			value.m[1][1] = std::cos(x) * std::cos(z);
			value.m[2][1] = std::sin(y) * std::sin(z) - std::sin(x) * std::cos(y) * std::cos(z);
			value.m[0][2] = -std::cos(x) * std::sin(y);
			value.m[1][2] = std::sin(x);
			value.m[2][2] = std::cos(x) * std::cos(y);
		}
		// 比較
		bool operator==(const Matrix4x4& obj) const noexcept {
			for (int x = 0; x < 4; ++x) {
				for (int y = 0; y < 4; ++y) {
					if (this->get().m[x][y] != obj.get().m[x][y]) {
						return false;
					}
				}
			}
			return true;


		}
		bool operator!=(const Matrix4x4& obj) const noexcept { return !(*this == obj); }
		// 乗算
		Matrix4x4 operator*(const Matrix4x4& obj)  const noexcept { return DxLib::MMult(this->get(), obj.get()); }
		void operator*=(const Matrix4x4& obj) noexcept { *this = *this * obj; }
	};

	// ---------------------------------------------------------------------------------------------
	// Matrix 3x3バージョン
	// ---------------------------------------------------------------------------------------------
	namespace MAT33 {
		// 行列構造体
		struct MATRIX33 {
			float					m[3][3];
		};
		// 相互変換
		static void M33toMATRIX(DxLib::MATRIX* pTarget, const MATRIX33& pAtr) noexcept {
			*pTarget = DxLib::MGetIdent();
			for (int x = 0; x < 3; ++x) {
				for (int y = 0; y < 3; ++y) {
					pTarget->m[x][y] = pAtr.m[x][y];
				}
			}
		}
		static void MATRIXtoM33(MATRIX33* pTarget, const DxLib::MATRIX& pAtr) noexcept {
			for (int x = 0; x < 3; ++x) {
				for (int y = 0; y < 3; ++y) {
					pTarget->m[x][y] = pAtr.m[x][y];
				}
			}
		}

		// 単位行列を得る
		static MATRIX33 M33GetIdent(void) noexcept {
			static MATRIX33 Result =
			{
				{
					{ 1.0f, 0.0f, 0.0f },
					{ 0.0f, 1.0f, 0.0f },
					{ 0.0f, 0.0f, 1.0f }
				}
			};
			return Result;
		}
		// 指定の３軸ローカルのベクトルを基本軸上のベクトルに変換する行列を得る
		// x' = XAxis.x * x + YAixs.x * y + ZAxis.z * z + Pos.x
		// y' = XAxis.y * x + YAixs.y * y + ZAxis.y * z + Pos.y
		// z' = XAxis.z * x + YAixs.z * y + ZAxis.z * z + Pos.z
		static MATRIX33 M33GetAxis1(const VECTOR3D& XAxis, const VECTOR3D& YAxis, const VECTOR3D& ZAxis) noexcept {
			MATRIX33 Result =
			{
				{
					{ XAxis.x, XAxis.y, XAxis.z },
					{ YAxis.x, YAxis.y, YAxis.z },
					{ ZAxis.x, ZAxis.y, ZAxis.z }
				}
			};
			return Result;
		}
		// 基本軸上のベクトルを指定の３軸上に投影したベクトルに変換する行列を得る
		// x' = XAxis.x * ( x - Pos.x ) + XAxis.y * ( x - Pos.x ) + XAxis.z * ( x - Pos.x )
		// y' = YAxis.x * ( x - Pos.x ) + YAxis.y * ( x - Pos.x ) + YAxis.z * ( x - Pos.x )
		// z' = ZAxis.x * ( x - Pos.x ) + ZAxis.y * ( x - Pos.x ) + ZAxis.z * ( x - Pos.x )
		static MATRIX33 M33GetAxis2(const VECTOR3D& XAxis, const VECTOR3D& YAxis, const VECTOR3D& ZAxis) noexcept {
			MATRIX33 Result =
			{
				{
					{ XAxis.x, YAxis.x, ZAxis.x },
					{ XAxis.y, YAxis.y, ZAxis.y },
					{ XAxis.z, YAxis.z, ZAxis.z }
				}
			};
			return Result;
		}
		// 指定軸で指定角度回転する行列を得る
		static MATRIX33 M33GetRotAxis(const VECTOR3D& RotateAxis, float Rotate) noexcept {
			MATRIX33 Result{};
			VECTOR3D xv = VECTOR3D::right(), yv = VECTOR3D::up(), zv = VECTOR3D::forward();
			VECTOR3D xv2{}, yv2{}, zv2{}, xv3{}, yv3{}, zv3{};
			float f{}, Sin{}, Cos{};

			zv2 = RotateAxis.normalized();
			yv2 = Util::VECTOR3D::vget(0.0f, 1.0f, 0.0f);
			if (VECTOR3D::Cross(yv2, zv2).sqrMagnitude() < 0.00001f) {
				yv2 = Util::VECTOR3D::vget(0.0f, 0.0f, 1.0f);
			}
			xv2 = VECTOR3D::Cross(zv2, yv2);
			yv2 = VECTOR3D::Cross(xv2, zv2).normalized();
			xv2 = xv2.normalized();

			xv3.x = xv2.x * xv.x + xv2.y * xv.y + xv2.z * xv.z;
			xv3.y = yv2.x * xv.x + yv2.y * xv.y + yv2.z * xv.z;
			xv3.z = zv2.x * xv.x + zv2.y * xv.y + zv2.z * xv.z;

			yv3.x = xv2.x * yv.x + xv2.y * yv.y + xv2.z * yv.z;
			yv3.y = yv2.x * yv.x + yv2.y * yv.y + yv2.z * yv.z;
			yv3.z = zv2.x * yv.x + zv2.y * yv.y + zv2.z * yv.z;

			zv3.x = xv2.x * zv.x + xv2.y * zv.y + xv2.z * zv.z;
			zv3.y = yv2.x * zv.x + yv2.y * zv.y + yv2.z * zv.z;
			zv3.z = zv2.x * zv.x + zv2.y * zv.y + zv2.z * zv.z;

			Sin = std::sinf(Rotate);
			Cos = std::cosf(Rotate);

			f = xv3.x * Cos - xv3.y * Sin;
			xv3.y = xv3.x * Sin + xv3.y * Cos;
			xv3.x = f;

			f = yv3.x * Cos - yv3.y * Sin;
			yv3.y = yv3.x * Sin + yv3.y * Cos;
			yv3.x = f;

			f = zv3.x * Cos - zv3.y * Sin;
			zv3.y = zv3.x * Sin + zv3.y * Cos;
			zv3.x = f;

			Result.m[0][0] = xv2.x * xv3.x + yv2.x * xv3.y + zv2.x * xv3.z;
			Result.m[1][0] = xv2.y * xv3.x + yv2.y * xv3.y + zv2.y * xv3.z;
			Result.m[2][0] = xv2.z * xv3.x + yv2.z * xv3.y + zv2.z * xv3.z;

			Result.m[0][1] = xv2.x * yv3.x + yv2.x * yv3.y + zv2.x * yv3.z;
			Result.m[1][1] = xv2.y * yv3.x + yv2.y * yv3.y + zv2.y * yv3.z;
			Result.m[2][1] = xv2.z * yv3.x + yv2.z * yv3.y + zv2.z * yv3.z;

			Result.m[0][2] = xv2.x * zv3.x + yv2.x * zv3.y + zv2.x * zv3.z;
			Result.m[1][2] = xv2.y * zv3.x + yv2.y * zv3.y + zv2.y * zv3.z;
			Result.m[2][2] = xv2.z * zv3.x + yv2.z * zv3.y + zv2.z * zv3.z;

			return Result;
		}
		// In1 の向きから In2 の向きへ変換する回転行列を得る
		static MATRIX33 M33GetRotVec2(const VECTOR3D& In1, const VECTOR3D& In2) noexcept {
			VECTOR3D av = VECTOR3D::Cross(In1, In2);
			if (av.sqrMagnitude() < 0.0000001f) {
				av = VECTOR3D::right();
			}
			float rad = DxLib::VRad(In1.get(), In2.get());
			return M33GetRotAxis(av, rad);
		}

		// 逆行列を作成する
		static int CreateInverseMatrix33(MATRIX33* Out, const MATRIX33& In) noexcept {
			float detA =
				In.m[0][0] * In.m[1][1] * In.m[2][2] +
				In.m[0][1] * In.m[1][2] * In.m[2][0] +
				In.m[0][2] * In.m[1][0] * In.m[2][1] -
				In.m[0][0] * In.m[1][2] * In.m[2][1] -
				In.m[0][1] * In.m[1][0] * In.m[2][2] -
				In.m[0][2] * In.m[1][1] * In.m[2][0];

			if (detA < 0.0000001f && detA > -0.0000001f) {
				return FALSE;
			}

			Out->m[0][0] =
				(
					In.m[1][1] * In.m[2][2] -
					In.m[1][2] * In.m[2][1]
					) / detA;

			Out->m[0][1] =
				(

					In.m[0][2] * In.m[2][1] -
					In.m[0][1] * In.m[2][2]
					) / detA;

			Out->m[0][2] =
				(
					In.m[0][1] * In.m[1][2] -
					In.m[0][2] * In.m[1][1]
					) / detA;

			Out->m[1][0] =
				(

					In.m[1][2] * In.m[2][0] -
					In.m[1][0] * In.m[2][2]
					) / detA;

			Out->m[1][1] =
				(
					In.m[0][0] * In.m[2][2] -
					In.m[0][2] * In.m[2][0]
					) / detA;

			Out->m[1][2] =
				(

					In.m[0][2] * In.m[1][0] -
					In.m[0][0] * In.m[1][2]
					) / detA;

			Out->m[2][0] =
				(
					In.m[1][0] * In.m[2][1] -
					In.m[1][1] * In.m[2][0]
					) / detA;

			Out->m[2][1] =
				(

					In.m[0][1] * In.m[2][0] -
					In.m[0][0] * In.m[2][1]
					) / detA;

			Out->m[2][2] =
				(
					In.m[0][0] * In.m[1][1] -
					In.m[0][1] * In.m[1][0]
					) / detA;

			return TRUE;
		}
		// 逆行列を得る
		static MATRIX33 M33Inverse(const MATRIX33& InM) noexcept {
			MATRIX33 Result;

			if (!CreateInverseMatrix33(&Result, InM)) {
				return M33GetIdent();
			}

			return Result;
		}

		// 転置行列を得る
		static MATRIX33 M33Transpose(const MATRIX33& InM) noexcept {
			MATRIX33 Result =
			{
				{
					{ InM.m[0][0], InM.m[1][0], InM.m[2][0] },
					{ InM.m[0][1], InM.m[1][1], InM.m[2][1] },
					{ InM.m[0][2], InM.m[1][2], InM.m[2][2] }
				}
			};
			return Result;
		}
		// 行列の乗算を行う
		static MATRIX33 M33Mult(const MATRIX33& In1, const MATRIX33& In2) noexcept {
			MATRIX33 Result =
			{
				{
					{
						In1.m[0][0] * In2.m[0][0] + In1.m[0][1] * In2.m[1][0] + In1.m[0][2] * In2.m[2][0],
						In1.m[0][0] * In2.m[0][1] + In1.m[0][1] * In2.m[1][1] + In1.m[0][2] * In2.m[2][1],
						In1.m[0][0] * In2.m[0][2] + In1.m[0][1] * In2.m[1][2] + In1.m[0][2] * In2.m[2][2]
					},
					{
						In1.m[1][0] * In2.m[0][0] + In1.m[1][1] * In2.m[1][0] + In1.m[1][2] * In2.m[2][0],
						In1.m[1][0] * In2.m[0][1] + In1.m[1][1] * In2.m[1][1] + In1.m[1][2] * In2.m[2][1],
						In1.m[1][0] * In2.m[0][2] + In1.m[1][1] * In2.m[1][2] + In1.m[1][2] * In2.m[2][2],
					},
					{
						In1.m[2][0] * In2.m[0][0] + In1.m[2][1] * In2.m[1][0] + In1.m[2][2] * In2.m[2][0],
						In1.m[2][0] * In2.m[0][1] + In1.m[2][1] * In2.m[1][1] + In1.m[2][2] * In2.m[2][1],
						In1.m[2][0] * In2.m[0][2] + In1.m[2][1] * In2.m[1][2] + In1.m[2][2] * In2.m[2][2],
					}
				}
			};
			return Result;
		}
		// 行列を使った座標変換
		static const VECTOR3D V33Transform(const VECTOR3D& InV, const MATRIX33& InM) noexcept {
			VECTOR3D Result{};
			Result.x = InV.x * InM.m[0][0] + InV.y * InM.m[1][0] + InV.z * InM.m[2][0];
			Result.y = InV.x * InM.m[0][1] + InV.y * InM.m[1][1] + InV.z * InM.m[2][1];
			Result.z = InV.x * InM.m[0][2] + InV.y * InM.m[1][2] + InV.z * InM.m[2][2];
			return Result;
		}
	}

	class Matrix3x3 {
		MAT33::MATRIX33	m_value;
	public:
		Matrix3x3(void) noexcept : m_value(MAT33::M33GetIdent()) {}
		Matrix3x3(MAT33::MATRIX33 value) noexcept { this->m_value = value; }
		MAT33::MATRIX33 get(void) const noexcept { return this->m_value; }		// 変換
		MAT33::MATRIX33& get(void) noexcept { return this->m_value; }		// 変換
		DxLib::MATRIX Get44(void) const noexcept {
			DxLib::MATRIX Result;
			MAT33::M33toMATRIX(&Result, this->m_value);
			return Result;
		}// 変換
		Matrix4x4 Get44DX(void) const noexcept {
			DxLib::MATRIX Result;
			MAT33::M33toMATRIX(&Result, this->m_value);
			return Result;
		}// 変換
		static Matrix3x3 Get33DX(const Matrix4x4& value) noexcept {
			MAT33::MATRIX33 Result;
			MAT33::MATRIXtoM33(&Result, value.get());
			return Result;
		}// 変換
		// 
		static Matrix3x3 identity(void) noexcept { return MAT33::M33GetIdent(); }

		static Matrix3x3 Axis1(const VECTOR3D& yvec, const VECTOR3D& zvec) noexcept { return { MAT33::M33GetAxis1(VECTOR3D::Cross(yvec, zvec),yvec,zvec) }; }
		static Matrix3x3 Axis2(const VECTOR3D& yvec, const VECTOR3D& zvec) noexcept { return { MAT33::M33GetAxis2(VECTOR3D::Cross(yvec, zvec),yvec,zvec) }; }
		static Matrix3x3 RotAxis(const VECTOR3D& p1, float p2) noexcept { return MAT33::M33GetRotAxis(p1, p2); }
		static Matrix3x3 RotVec2(const VECTOR3D& p1, const VECTOR3D& p2) noexcept { return { MAT33::M33GetRotVec2(p1, p2) }; }
		static VECTOR3D Vtrans(const VECTOR3D& p1, const Matrix3x3& p2) noexcept { return MAT33::V33Transform(p1, p2.get()); }
		// 
		Matrix3x3 inverse(void) const noexcept { return M33Inverse(this->get()); }		// 逆
		bool isIdentity(void) const noexcept { return *this == MAT33::M33GetIdent(); }		// 逆
		// lossyScale
		Matrix3x3 transpose(void) const noexcept { return MAT33::M33Transpose(this->get()); }		// 逆

		VECTOR3D xvec(void) const noexcept { return Vtrans(VECTOR3D::right(), *this); }
		VECTOR3D yvec(void) const noexcept { return Vtrans(VECTOR3D::up(), *this); }
		VECTOR3D zvec(void) const noexcept { return Vtrans(VECTOR3D::forward(), *this); }
		VECTOR3D zvec2(void) const noexcept { return Vtrans(VECTOR3D::back(), *this); }//左手座標系で右手座標系のキャラを描画した際の正面
		// 
		void GetRadian(float* angle_x, float* angle_y, float* angle_z) const noexcept {
			constexpr float threshold = 0.001f;
			if (std::abs(this->m_value.m[1][2] - 1.0f) < threshold) { // R(2,1) = sin(x) = 1の時
				if (angle_x) {
					*angle_x = DX_PI_F / 2.f;
				}
				if (angle_y) {
					*angle_y = 0.f;
				}
				if (angle_z) {
					*angle_z = std::atan2f(this->m_value.m[0][1], this->m_value.m[0][0]);
				}
			}
			else if (std::abs(this->m_value.m[1][2] + 1.0f) < threshold) { // R(2,1) = sin(x) = -1の時
				if (angle_x) {
					*angle_x = -DX_PI_F / 2.f;
				}
				if (angle_y) {
					*angle_y = 0.f;
				}
				if (angle_z) {
					*angle_z = std::atan2f(this->m_value.m[0][1], this->m_value.m[0][0]);
				}
			}
			else {
				if (angle_x) {
					*angle_x = std::asinf(this->m_value.m[1][2]);
				}
				if (angle_y) {
					*angle_y = std::atan2f(-this->m_value.m[0][2], this->m_value.m[2][2]);
				}
				if (angle_z) {
					*angle_z = std::atan2f(-this->m_value.m[1][0], this->m_value.m[1][1]);
				}
			}
		}
		void SetRadian(float x, float y, float z) noexcept {
			this->m_value.m[0][0] = std::cos(y) * std::cos(z) - std::sin(x) * std::sin(y) * std::sin(z);
			this->m_value.m[1][0] = -std::cos(x) * std::sin(z);
			this->m_value.m[2][0] = std::sin(y) * std::cos(z) + std::sin(x) * std::cos(y) * std::sin(z);
			this->m_value.m[0][1] = std::cos(y) * std::sin(z) + std::sin(x) * std::sin(y) * std::cos(z);
			this->m_value.m[1][1] = std::cos(x) * std::cos(z);
			this->m_value.m[2][1] = std::sin(y) * std::sin(z) - std::sin(x) * std::cos(y) * std::cos(z);
			this->m_value.m[0][2] = -std::cos(x) * std::sin(y);
			this->m_value.m[1][2] = std::sin(x);
			this->m_value.m[2][2] = std::cos(x) * std::cos(y);
		}
		// 比較
		bool operator==(const Matrix3x3& obj) const noexcept {
			for (int x = 0; x < 3; ++x) {
				for (int y = 0; y < 3; ++y) {
					if (this->get().m[x][y] != obj.get().m[x][y]) {
						return false;
					}
				}
			}
			return true;
		}
		bool operator!=(const Matrix3x3& obj) const noexcept { return !(*this == obj); }
		// 乗算
		Matrix3x3 operator*(const Matrix3x3& obj)  const noexcept { return MAT33::M33Mult(this->get(), obj.get()); }
		void operator*=(const Matrix3x3& obj) noexcept { *this = *this * obj; }
	};
	// ---------------------------------------------------------------------------------------------
	// Vector3
	// ---------------------------------------------------------------------------------------------

	// 点と矩形との2D判定
	static bool HitPointToRectangle(int xp, int yp, int x1, int y1, int x2, int y2) noexcept { return (xp >= x1 && xp <= x2 && yp >= y1 && yp <= y2); }
	// 矩形と矩形との2D判定
	static bool HitRectangleToRectangle(int xp1, int yp1, int xp2, int yp2, int x1, int y1, int x2, int y2) noexcept { return (xp1 < x2 && x1 < xp2) && (yp1 < y2 && y1 < yp2); }
	// 点と四角形との2D判定
	static bool HitPointToSquare(VECTOR2D point, VECTOR2D c1, VECTOR2D c2, VECTOR2D c3, VECTOR2D c4) noexcept {
		if (0 > VECTOR2D::Cross(c2 - c1, point - c1)) { return false; }
		if (0 > VECTOR2D::Cross(c3 - c2, point - c2)) { return false; }
		if (0 > VECTOR2D::Cross(c4 - c3, point - c3)) { return false; }
		if (0 > VECTOR2D::Cross(c1 - c4, point - c4)) { return false; }
		return true;
	}
	// 球とカプセルとの判定
	static bool GetHitSphereToCapsule(const VECTOR3D& pos, float size, const VECTOR3D& tri_p1, const VECTOR3D& tri_p2, float size2) noexcept {
		return HitCheck_Sphere_Capsule(pos.get(), size, tri_p1.get(), tri_p2.get(), size2) == TRUE;
	}
	// 線分同士の交差判定
	static bool GetSegmenttoSegment(const VECTOR3D& SegmentAPos1, const VECTOR3D& SegmentAPos2, const VECTOR3D& SegmentBPos1, const VECTOR3D& SegmentBPos2, SEGMENT_SEGMENT_RESULT* Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = SegmentBPos1.get();
		VECTOR PosBt = SegmentBPos2.get();

		Segment_Segment_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, Result);
		float len = 0.001f;
		return (Result->SegA_SegB_MinDist_Square <= (len * len));
	}

	// 直線と点の一番近い点
	static float GetMinLenSegmentToPoint(const Util::VECTOR3D& startpos, const Util::VECTOR3D& endpos, const Util::VECTOR3D& tgt) noexcept {
		return Segment_Point_MinLength(startpos.get(), endpos.get(), tgt.get());
	}
	// 直線と直線の一番近い距離
	static float GetMinLenSegmentToSegment(const Util::VECTOR3D& startpos, const Util::VECTOR3D& endpos, const Util::VECTOR3D& tgtstartpos, const Util::VECTOR3D& tgtendpos) noexcept {
		return Segment_Segment_MinLength(startpos.get(), endpos.get(), tgtstartpos.get(), tgtendpos.get());
	}

	// 線形補完
	template <class T>
	inline T Lerp(const T& A, const T& B, float Per) noexcept {
		if (Per == 0.f) {
			return A;
		}
		else if (Per == 1.f) {
			return B;
		}
		else {
			return A + (T)((B - A) * Per);
		}
	}
	// Matrix版の線形補完
	inline Matrix4x4 Lerp(const Matrix4x4& A, const Matrix4x4& B, float Per) noexcept {
		return Matrix4x4::Axis1(
			Lerp(A.yvec(), B.yvec(), Per).normalized(),
			Lerp(A.zvec(), B.zvec(), Per).normalized(),
			Lerp(A.pos(), B.pos(), Per));
	}
	inline Matrix3x3 Lerp(const Matrix3x3& A, const Matrix3x3& B, float Per) noexcept {
		return Matrix3x3::Axis1(
			Lerp(A.yvec(), B.yvec(), Per).normalized(),
			Lerp(A.zvec(), B.zvec(), Per).normalized());
	}

	extern float GetEasingRatio(float ratio) noexcept;

	template <class T>
	inline void Easing(T* A, const T& B, float Per) noexcept {
		*A = Util::Lerp(*A, B, GetEasingRatio(Per));
	}

	inline float GetPer01(float Min, float Max, float Per) noexcept {
		return std::clamp((Per - Min) / (Max - Min), 0.f, 1.f);
	}
	
	// 0~2Piに収める
	inline float AngleRange360(float value) noexcept {
		if (value > 0.f) {
			while (true) {
				if (value < DX_PI_F * 2.f) { break; }
				value -= DX_PI_F * 2.f;
			}
		}
		if (value < 0.f) {
			while (true) {
				if (value > -DX_PI_F * 2.f) { break; }
				value += DX_PI_F * 2.f;
			}
		}
		return value;
	}

	// -Pi~Piに収める
	inline float AngleRange180(float value) noexcept {
		value = AngleRange360(value);
		if (value >= DX_PI_F) {
			value -= DX_PI_F * 2.f;
		}
		return value;
	}
	// --------------------------------------------------------------------------------------------------
	// 角度変換
	// --------------------------------------------------------------------------------------------------
	// 角度からラジアンに
	extern void* enabler;// ダミー変数
	template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type*& = enabler>
	constexpr float deg2rad(T p1) noexcept { return static_cast<float>(p1) * DX_PI_F / 180.f; }
	// ラジアンから角度に
	template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type*& = enabler>
	constexpr float rad2deg(T p1) noexcept { return static_cast<float>(p1) * 180.f / DX_PI_F; }

	// 余弦定理
	constexpr float GetCosFormula(float a, float b, float c) noexcept {
		if (b + c > a && c + a > b && a + b > c) {
			return std::clamp((b * b + c * c - a * a) / (2.f * b * c), -1.f, 1.f);
		}
		return 1.f;
	}
}