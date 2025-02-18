#pragma once  

#include "..\..\Core.h"
#include <string>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "Engine/Util/StrUtil.h"
#include "Engine/Util/Path.h"
#include "../../Define.h"

namespace Engine
{
	class AssetTexture;
	class FontData;
	class MeshData;
	class MaterialBlinnPhong;
	class Material;
	class Path;
}

namespace sketch
{
	class GeoData;
	class GeoSliceArray;

	class ResManager
	{
	public:
		ResManager();
		virtual ~ResManager();

		void init();
		void release();

		bool onDocPrepare(const Engine::Path& projPath); //场景初始化/加载前调用
		bool onDocNew(const Engine::Path& projPath); //初始化新场景
		bool onDocLoad(const Engine::Path& projPath); //加载场景
		bool onDocLoaded(const Engine::Path& projPath); //场景初始化/加载完毕后调用
		bool onDocWillClose(void);
		bool onDocClose(void);
		bool onAppClose(void);

		const std::string& sdfFontUrl() {
			return m_sdfFontUrl;
		}

		Engine::FontData* sceneDefFont();
		Engine::FontData* sceneDefFontHover();

		static std::string PAR_LINETYPE(Define::DRLineType type) {
			return std::string("linetype:") + (const char)(('0' + (char)type)) + ";";
		}

		static std::string PAR_FILLTYPE(Define::DRFillType type) {
			return std::string("filltype:") + (const char)(('0' + (char)type)) + ";";
		}

		static std::string PAR_VERTCOLOR(bool v) {
			return std::string("vertcolor:") + (v ? "1" : "0") + ";";
		}

		static std::string PAR_MTRLCOLOR(bool v) {
			return std::string("mtrlcolor:") + (v ? "1" : "0") + ";";
		}

		inline const std::unordered_map<std::string, GeoData*>& geos() const {
			return m_key2Geo;
		}

		GeoData* refGeo(const std::string& key, bool localCache);
		void unrefGeo(const std::string& key);

		inline void dirtyCache(GeoData* pGeo) {
			m_cacheDirtyGeos.insert(pGeo);
		}

		Engine::MeshData* createMesh();
		void deleteMesh(Engine::MeshData* pMD);

		Engine::MeshData* refMesh(const std::string& key);
		void unrefMesh(const std::string& key);
		void unrefMesh(Engine::MeshData* pMD);

		void update(unsigned long long tm);

		void updateValidKeys();

		void diposeInstBatches(int rdId);

	protected:
		void _initBuiltInRes();

		bool _cleanLocalCache();
		bool _saveCache(GeoData* pGeo);
		bool _loadCache(GeoData* pGeo);

	protected:
		Engine::Path m_localCachePath;
		std::unordered_set<std::string> m_validKeys;

		std::string m_sdfFontUrl;

		Engine::FontData* m_pSceneDefFont;
		Engine::FontData* m_pSceneDefFontHover;

		std::unordered_map<std::string, GeoData*> m_key2Geo;
		std::unordered_map<std::string, GeoSliceArray*> m_key2SliceArray;
		std::unordered_set<GeoData*> m_cacheDirtyGeos;

		std::unordered_map<std::string, Engine::MeshData*> m_key2Mesh;
		std::unordered_map<Engine::MeshData*, std::string> m_mesh2Key;
		std::unordered_map<std::string, int> m_key2MeshRef;
	};
}