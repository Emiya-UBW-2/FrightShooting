#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)
#include "../Util/Enum.hpp"
#include "../Util/Util.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Draw/MainDraw.hpp"
#include "../Draw/Camera.hpp"
#include "../Draw/PostPass.hpp"
#include "../Draw/Light.hpp"
#include "../Draw/MV1.hpp"

static float GetRandf(float arg) noexcept { return -arg + static_cast<float>(GetRand(static_cast<int>(arg * 2.f * 10000.f))) / 10000.f; }

//インスタシング
class Model_Instance {
private:
	std::vector<VERTEX3D>	m_Vertex;				//
	std::vector<DWORD>		m_Index;				//
	int						m_VerBuf{ -1 };			//
	int						m_IndexBuf{ -1 };		//
	Draw::MV1						m_obj;					//モデル
	Draw::GraphHandle				m_pic;					//画像ハンドル
	int						m_vnum{ -1 };			//
	int						m_pnum{ -1 };			//
	MV1_REF_POLYGONLIST		m_RefMesh{};			//
	int						m_Mesh{ 0 };			//
	int						m_Count{ 0 };			//数
public:
	Model_Instance(void) noexcept {}
	Model_Instance(const Model_Instance&) = delete;
	Model_Instance(Model_Instance&&) = delete;
	Model_Instance& operator=(const Model_Instance&) = delete;
	Model_Instance& operator=(Model_Instance&&) = delete;
	virtual ~Model_Instance(void) noexcept {}
private:
	void			Init_one(void) noexcept {
		MV1RefreshReferenceMesh(this->m_obj.get(), -1, TRUE, FALSE, m_Mesh);				//参照用メッシュの更新
		this->m_RefMesh = MV1GetReferenceMesh(this->m_obj.get(), -1, TRUE, FALSE, m_Mesh);	//参照用メッシュの取得
	}
public:
	//リセット
	void			Reset(void) noexcept {
		this->m_Count = 0;
		this->m_vnum = 0;
		this->m_pnum = 0;
		this->m_Vertex.clear();								//頂点データとインデックスデータを格納するメモリ領域の確保
		this->m_Vertex.reserve(2000);						//頂点データとインデックスデータを格納するメモリ領域の確保
		this->m_Index.clear();								//頂点データとインデックスデータを格納するメモリ領域の確保
		this->m_Index.reserve(2000);						//頂点データとインデックスデータを格納するメモリ領域の確保
	}
	void			Set(const float& caliber, const Util::VECTOR3D& Position, const Util::VECTOR3D& Normal, const Util::VECTOR3D& Zvec) {
		this->m_Count++;
		Set_start(this->m_Count);
		float asize = 200.f * caliber;
		const auto& y_vec = Normal;
		auto z_vec = Util::VECTOR3D::Cross(y_vec, Zvec).normalized();
		auto scale = Util::VECTOR3D::vget(asize / std::abs(Util::VECTOR3D::Dot(y_vec, Zvec)), asize, asize);
		Util::Matrix4x4 mat = Util::Matrix4x4::GetScale(scale) * Util::Matrix4x4::Axis1(y_vec, z_vec) * Util::Matrix4x4::Mtrans(Position + y_vec * 0.02f);
		Set_one(mat);
	}
	void			Set_start(int value) noexcept {
		this->m_Count = value;
		int Num = this->m_RefMesh.VertexNum * this->m_Count;
		this->m_Vertex.resize(static_cast<size_t>(Num));			//頂点データとインデックスデータを格納するメモリ領域の確保
		Num = this->m_RefMesh.PolygonNum * 3 * this->m_Count;
		this->m_Index.resize(static_cast<size_t>(Num));		//頂点データとインデックスデータを格納するメモリ領域の確保
	}
	void			Set_one(const Util::Matrix4x4& mat) noexcept {
		this->m_obj.SetMatrix(mat);
		Init_one();
		for (size_t j = 0; j < size_t(this->m_RefMesh.VertexNum); ++j) {
			auto& g = this->m_Vertex[j + this->m_vnum];
			const auto& r = this->m_RefMesh.Vertexs[j];
			g.pos = r.Position;
			g.norm = r.Normal;
			g.dif = r.DiffuseColor;
			g.spc = r.SpecularColor;
			g.u = r.TexCoord[0].u;
			g.v = r.TexCoord[0].v;
			g.su = r.TexCoord[1].u;
			g.sv = r.TexCoord[1].v;
		}
		for (size_t j = 0; j < size_t(this->m_RefMesh.PolygonNum); ++j) {
			for (size_t k = 0; k < std::size(this->m_RefMesh.Polygons[j].VIndex); ++k) {
				this->m_Index[j * 3 + k + this->m_pnum] = WORD(this->m_RefMesh.Polygons[j].VIndex[k] + this->m_vnum);
			}
		}
		this->m_vnum += this->m_RefMesh.VertexNum;
		this->m_pnum += this->m_RefMesh.PolygonNum * 3;
	}
public:
	void			Init(Draw::MV1& mv1path, int MeshNum) noexcept {
		SetUseASyncLoadFlag(FALSE);
		this->m_Mesh = MeshNum;
		this->m_pic.MV1GetTexture(mv1path.get(), MV1GetMaterialDifMapTexture(mv1path.get(), MV1GetMeshMaterial(mv1path.get(), m_Mesh)));			 //grass
		mv1path.Duplicate(this->m_obj);				//弾痕
		Init_one();
	}
	void			Init(std::string pngpath, std::string mv1path, int MeshNum) noexcept {
		SetUseASyncLoadFlag(FALSE);
		this->m_Mesh = MeshNum;
		this->m_pic.Load(pngpath);		 //grass
		Draw::MV1::Load(mv1path, &this->m_obj);				//弾痕
		Init_one();
	}
	void			Execute(void) noexcept {
		this->m_VerBuf = CreateVertexBuffer((int)this->m_Vertex.size(), DX_VERTEX_TYPE_NORMAL_3D);
		this->m_IndexBuf = CreateIndexBuffer((int)this->m_Index.size(), DX_INDEX_TYPE_32BIT);
		SetVertexBufferData(0, this->m_Vertex.data(), (int)this->m_Vertex.size(), this->m_VerBuf);
		SetIndexBufferData(0, this->m_Index.data(), (int)this->m_Index.size(), this->m_IndexBuf);
	}
	void			Draw(void) const noexcept {
		DrawPolygonIndexed3D_UseVertexBuffer(this->m_VerBuf, this->m_IndexBuf, this->m_pic.get(), TRUE);
	}
	void			Dispose(void) noexcept {
		this->m_Vertex.clear();
		this->m_Index.clear();
		this->m_obj.Dispose();
		this->m_pic.Dispose();
	}
};

//kusa
class Grass {
	class grass_t {
	public:
		grass_t(void) noexcept {}
		grass_t(const grass_t&) = delete;
		grass_t(grass_t&&) = delete;
		grass_t& operator=(const grass_t&) = delete;
		grass_t& operator=(grass_t&&) = delete;
		virtual ~grass_t(void) noexcept {}
	public:
		Model_Instance	m_Inst;
		bool			canlook = true;
		char		padding[7]{};
	public:
		void Init(int total) {
			this->m_Inst.Init("data/model/Cloud/tex.png", "data/model/Cloud/model2.mqoz", -1);
			this->m_Inst.Reset();
			this->m_Inst.Set_start(total);
		}
		void Set_one(const Util::Matrix4x4& mat) {
			this->m_Inst.Set_one(mat);
		}
		void put(void) noexcept {
			canlook = true;
			this->m_Inst.Execute();
		}
		void Dispose(void) noexcept {
			this->m_Inst.Dispose();
		}
		/*視界外か否かを判断*/
		void Check_CameraViewClip(const Util::VECTOR3D& min, const Util::VECTOR3D& max) {
			this->canlook = true;
			if (CheckCameraViewClip_Box(min.get(), max.get())) {
				this->canlook = false;
				return;
			}
		}
		void Draw(void) const noexcept {
			if (this->canlook) {
				this->m_Inst.Draw();
			}
		}
	};
	struct GrassPos {
		int X_PosMin = 0;
		int Y_PosMin = 0;
		int X_PosMax = 0;
		int Y_PosMax = 0;
	};
public:
	static const int grassDiv{ 12 };//^2;
	const float size{ 30.f };
private:
	const int grasss = 90;						/*grassの数*/
	int Flag = 0;
	char		padding[4]{};
	std::array<grass_t, grassDiv>grass__;
	std::array<Util::VECTOR3D, grassDiv>grassPosMin;
	std::array<Util::VECTOR3D, grassDiv>grassPosMax;
	std::array<GrassPos, grassDiv> grassPos;
public:
	Grass(void) noexcept {}
	Grass(const Grass&) = delete;
	Grass(Grass&&) = delete;
	Grass& operator=(const Grass&) = delete;
	Grass& operator=(Grass&&) = delete;
	virtual ~Grass(void) noexcept {}
private:
	int GetColorSoftImage(int softimage, int x_, int y_) {
		int _r_, _g_, _b_;
		int CCC = 0;
		GetPixelSoftImage(softimage, x_, y_, &_r_, &_g_, &_b_, nullptr);
		if (_r_ <= 64) {}
		else if (_r_ <= 192) { CCC |= (1 << 1); }
		else if (_r_ <= 256) { CCC |= (1 << 2); }
		if (_g_ <= 64) {}
		else if (_g_ <= 192) { CCC |= (1 << 4); }
		else if (_g_ <= 256) { CCC |= (1 << 5); }
		if (_b_ <= 64) {}
		else if (_b_ <= 192) { CCC |= (1 << 7); }
		else if (_b_ <= 256) { CCC |= (1 << 8); }
		return CCC;
	}
	//y方向に操作する前提
	void SetMinMax(int CCC, int ID, int softimage, int x_t, int/*y_t*/, int sizex, int sizey) {
		int BufC = -1;
		if ((Flag & (1 << ID)) == 0) {
			Flag |= (1 << ID);
			//xmin
			grassPos[static_cast<size_t>(ID)].X_PosMin = x_t;
			//y_t;
			//ymin
			BufC = -1;
			for (int y_ = 0; y_ < sizey; y_++) {
				for (int x_ = grassPos[static_cast<size_t>(ID)].X_PosMin; x_ < sizex; x_++) {
					BufC = GetColorSoftImage(softimage, x_, y_);
					if (BufC == CCC) {
						grassPos[static_cast<size_t>(ID)].Y_PosMin = y_;
						break;
					}
					else {
						BufC = -1;
					}
				}
				if (BufC >= 0) { break; }
			}
			//xmax
			BufC = -1;
			for (int x_ = sizex - 1; x_ >= grassPos[static_cast<size_t>(ID)].X_PosMin; x_--) {
				for (int y_ = sizey - 1; y_ >= grassPos[static_cast<size_t>(ID)].Y_PosMin; y_--) {
					BufC = GetColorSoftImage(softimage, x_, y_);
					if (BufC == CCC) {
						grassPos[static_cast<size_t>(ID)].X_PosMax = x_ + 1;
						break;
					}
					else {
						BufC = -1;
					}
				}
				if (BufC >= 0) { break; }
			}
			//ymax
			BufC = -1;
			for (int y_ = sizey - 1; y_ >= grassPos[static_cast<size_t>(ID)].Y_PosMin; y_--) {
				for (int x_ = grassPos[static_cast<size_t>(ID)].X_PosMax - 1; x_ >= grassPos[static_cast<size_t>(ID)].X_PosMin; x_--) {
					BufC = GetColorSoftImage(softimage, x_, y_);
					if (BufC == CCC) {
						grassPos[static_cast<size_t>(ID)].Y_PosMax = y_ + 1;
						break;
					}
					else {
						BufC = -1;
					}
				}
				if (BufC >= 0) { break; }
			}
			//ok
		}
	}
public:
	void Init(int softimage) {
		float MAPX = 300.f * Scale3DRate;
		float MAPZ = 300.f * Scale3DRate;
		float PosX = 0.f;
		float PosZ = 0.f;

		float MINX = -MAPX / 2.f + PosX;
		float MINZ = -MAPZ / 2.f + PosZ;

		float MAXX = MAPX / 2.f + PosX;
		float MAXZ = MAPZ / 2.f + PosZ;

		int sizex = 0, sizey = 0;
		GetSoftImageSize(softimage, &sizex, &sizey);

		Flag = 0;
		for (int x_ = 0; x_ < sizex; x_++) {
			for (int y_ = 0; y_ < sizey; y_++) {
				int CCC = GetColorSoftImage(softimage, x_, y_);
				//255,0,0
				if (CCC == (1 << 2)) {
					SetMinMax(CCC, 0, softimage, x_, y_, sizex, sizey);
				}
				//255,128,0
				else if (CCC == ((1 << 2) | (1 << 4))) {
					SetMinMax(CCC, 1, softimage, x_, y_, sizex, sizey);
				}
				//255,255,0
				else if (CCC == ((1 << 2) | (1 << 5))) {
					SetMinMax(CCC, 2, softimage, x_, y_, sizex, sizey);
				}
				//128,255,0
				else if (CCC == ((1 << 1) | (1 << 5))) {
					SetMinMax(CCC, 3, softimage, x_, y_, sizex, sizey);
				}
				//0,255,0
				else if (CCC == (1 << 5)) {
					SetMinMax(CCC, 4, softimage, x_, y_, sizex, sizey);
				}
				//0,255,128
				else if (CCC == ((1 << 5) | (1 << 7))) {
					SetMinMax(CCC, 5, softimage, x_, y_, sizex, sizey);
				}
				//0,255,255
				else if (CCC == ((1 << 5) | (1 << 8))) {
					SetMinMax(CCC, 6, softimage, x_, y_, sizex, sizey);
				}
				//0,128,255
				else if (CCC == ((1 << 4) | (1 << 8))) {
					SetMinMax(CCC, 7, softimage, x_, y_, sizex, sizey);
				}
				//0,0,255
				else if (CCC == (1 << 8)) {
					SetMinMax(CCC, 8, softimage, x_, y_, sizex, sizey);
				}
				//128,0,255
				else if (CCC == ((1 << 1) | (1 << 8))) {
					SetMinMax(CCC, 9, softimage, x_, y_, sizex, sizey);
				}
				//255,0,255
				else if (CCC == ((1 << 2) | (1 << 8))) {
					SetMinMax(CCC, 10, softimage, x_, y_, sizex, sizey);
				}
				//255,0,128
				else if (CCC == ((1 << 2) | (1 << 7))) {
					SetMinMax(CCC, 11, softimage, x_, y_, sizex, sizey);
				}

				//MINX + (MAXX - MINX) * x_ / sizex = x_t 
				//MINZ + (MAXZ - MINZ) * y_ / sizey = z_t 
			}
		}
		//*/
		for (int ID = 0; ID < grassDiv; ID++) {
			//ポジション決定
			float xp = MINX + (MAXX - MINX) * static_cast<float>(grassPos[static_cast<size_t>(ID)].X_PosMin) / static_cast<float>(sizex);
			float zp = MINZ + (MAXZ - MINZ) * static_cast<float>(grassPos[static_cast<size_t>(ID)].Y_PosMin) / static_cast<float>(sizey);
			float xp2 = MINX + (MAXX - MINX) * static_cast<float>(grassPos[static_cast<size_t>(ID)].X_PosMax) / static_cast<float>(sizex);
			float zp2 = MINZ + (MAXZ - MINZ) * static_cast<float>(grassPos[static_cast<size_t>(ID)].Y_PosMax) / static_cast<float>(sizey);
			float xsize = xp2 - xp;
			float zsize = zp2 - zp;
			//
			{
				grassPosMin[static_cast<size_t>(ID)] = Util::VECTOR3D::vget(xp, 0.2f, zp);
				grassPosMax[static_cast<size_t>(ID)] = grassPosMin[static_cast<size_t>(ID)] + Util::VECTOR3D::vget(xsize, 1.f, zsize);
				float xmid = xsize / 2.f;
				float zmid = zsize / 2.f;
				if (grasss != 0) {
					auto& tgt_g = grass__[static_cast<size_t>(ID)];
					tgt_g.Init(grasss);
					for (int i = 0; i < grasss; ++i) {
						float x1 = xmid + GetRandf(xmid);
						float z1 = zmid + GetRandf(zmid);
						while (true) {
							int CCC = GetColorSoftImage(softimage,
								(int)(((grassPosMin[static_cast<size_t>(ID)].x + x1) - MINX) / (MAXX - MINX) * float(sizex)),
								(int)(((grassPosMin[static_cast<size_t>(ID)].z + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
							);
							if (CCC != 0) {
								break;
							}
							x1 = xmid + GetRandf(xmid);
							z1 = zmid + GetRandf(zmid);
						}

						auto tmpvect = grassPosMin[static_cast<size_t>(ID)] + Util::VECTOR3D::vget(x1 - xmid, 0.f, z1 - zmid) * (Scale3DRate * 10.f) +
							Util::VECTOR3D::vget(0.f, 0.f * Scale3DRate, 0.f) + 
							Util::VECTOR3D::vget(0.f, GetRandf(2000.f) * Scale3DRate, 0.f);
						tgt_g.Set_one(Util::Matrix4x4::Mtrans(tmpvect));
					}
					tgt_g.put();
				}
			}
			//
		}
	}
	void Dispose(void) noexcept {
		for (int ID = 0; ID < grassDiv; ID++) {
			if (grasss != 0) {
				grass__[static_cast<size_t>(ID)].Dispose();
			}
		}
	}
	void DrawShadow(void) noexcept {
	}
	void CheckDraw(void) noexcept {
		for (int ID = 0; ID < grassDiv; ID++) {
#ifdef DEBUG
			//DrawCube3D(grassPosMin[static_cast<size_t>(ID)].get(), grassPosMax[static_cast<size_t>(ID)].get(), GetColor(0, 0, 0), GetColor(0, 0, 0), FALSE);
#endif
			if (grasss != 0) {
				this->grass__[static_cast<size_t>(ID)].Check_CameraViewClip(grassPosMin[static_cast<size_t>(ID)], grassPosMax[static_cast<size_t>(ID)]);
			}
		}
	}
	void Draw(void) const noexcept {
		SetUseZBuffer3D(true);
		SetWriteZBufferFlag(true);
		SetUseHalfLambertLighting(true);
		for (int ID = 0; ID < grassDiv; ID++) {
#ifdef DEBUG
			//DrawCube3D(grassPosMin[static_cast<size_t>(ID)].get(), grassPosMax[static_cast<size_t>(ID)].get(), GetColor(0, 0, 0), GetColor(0, 0, 0), FALSE);
#endif
			if (grasss != 0) {
				grass__[static_cast<size_t>(ID)].Draw();
			}
		}
		SetUseHalfLambertLighting(false);
		SetWriteZBufferFlag(false);
	}
};

class BackGround : public Util::SingletonBase<BackGround> {
private:
	friend class Util::SingletonBase<BackGround>;
private:
	Draw::MV1				SkyBoxID{};
	Draw::MV1				MapID{};

	Grass					m_Grass;
public:
	BackGround(void) noexcept {}
	BackGround(const BackGround&) = delete;
	BackGround(BackGround&&) = delete;
	BackGround& operator=(const BackGround&) = delete;
	BackGround& operator=(BackGround&&) = delete;
	virtual ~BackGround(void) noexcept { Dispose(); }
public:
public:
	void Load() noexcept {
		Draw::MV1::Load("data/model/SkyBox/model.mqoz", &SkyBoxID);
		Draw::MV1::Load("data/model/Map/model.mv1", &MapID);
	}
	void Init(void) noexcept {
		int A = LoadSoftImage("data/grass.png");
		m_Grass.Init(A);
		DeleteSoftImage(A);
	}
	void Update(void) noexcept {
	}
	void Dispose(void) noexcept {
		SkyBoxID.Dispose();
		MapID.Dispose();
		m_Grass.Dispose();
	}

	void BGDraw(void) const noexcept {
		SkyBoxID.SetMatrix(Util::Matrix4x4::Mtrans(GetCameraPosition()));
		DxLib::SetUseLighting(FALSE);
		SkyBoxID.DrawModel();
		DxLib::SetUseLighting(TRUE);
	}
	void SetShadowDrawRigid(void) const noexcept {
	}
	void SetShadowDraw(void) const noexcept {
		MapID.DrawModel();
	}
	void CheckDraw(void) noexcept {
		m_Grass.CheckDraw();
	}
	void Draw(void) const noexcept {
		Util::VECTOR3D Pos = GetCameraPosition();
		Pos.y = 0.f- 50000.f * Scale3DRate;
		MapID.SetMatrix(Util::Matrix4x4::Mtrans(Pos));
		MapID.DrawModel();

		auto Prev = GetUseBackCulling();
		SetUseBackCulling(DX_CULLING_LEFT);
		m_Grass.Draw();
		SetUseBackCulling(Prev);
	}
	void DepthDraw(int layer) noexcept {
		return;
		auto Prev = GetUseBackCulling();
		if (layer == 0) {
			SetUseBackCulling(DX_CULLING_RIGHT);//背面
		}
		else {
			SetUseBackCulling(DX_CULLING_LEFT);
		}
		m_Grass.Draw();
		SetUseBackCulling(Prev);
	}
	void ShadowDrawFar(void) const noexcept {}
	void ShadowDraw(void) const noexcept {
		MapID.DrawModel();
	}
};
