#include "sketch/View/RenderRes/ResManager.h"

#include "Engine/Asset/AssetBitmapFont.h"
#include "Engine/Asset/AssetData/FontData.h"
#include "Engine/Asset/AssetData/MaterialBlinnPhong.h"
#include "Engine/Asset/AssetData/SdfFontData.h"
#include "Engine/Asset/AssetData/Texture2D.h"
#include "Engine/Asset/AssetFont.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetMaterial.h"
#include "Engine/Asset/AssetMesh.h"
#include "Engine/Asset/AssetSdfFont.h"
#include "Engine/Asset/AssetTexture.h"
#include "Engine/Math/GeoMesh/GeoMesh.h"
#include "Engine/Math/Math.h"
#include "Engine/Math/Vec4i.h"
#include "Engine/Render/Device.h"
#include "Engine/Util/FileUtil.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/SysUtil.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Localization/LocalManager.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/STCore.h"
#include "sketch/View/Controller/Controller.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/ViewManager.h"
#include "sketch/resource.h"

using namespace Engine;

namespace sketch
{
ResManager::ResManager() : m_pSceneDefFont(nullptr), m_pSceneDefFontHover(nullptr) {}

ResManager::~ResManager() { this->release(); }

void ResManager::init() { this->_initBuiltInRes(); }

void ResManager::release()
{
    {// delete ref geos
        for(auto& it: m_key2Geo)
        {
            delete it.second;
        }
        m_key2Geo.clear();

        for(auto& it: m_key2SliceArray)
        {
            delete it.second;
        }
        m_key2SliceArray.clear();

        m_cacheDirtyGeos.clear();
    }

    {// delete ref meshes
        for(auto& it: m_key2Mesh)
        {
            STE::assetMgr()->deleteAsset(it.first);
        }
        m_key2Mesh.clear();
        m_mesh2Key.clear();
        m_key2MeshRef.clear();
    }
}

bool ResManager::onDocPrepare(const Engine::Path& projPath)
{
    m_localCachePath = projPath / "temp\\geocache";
    if(!FileUtil::hasDir(m_localCachePath))
        FileUtil::createDir(m_localCachePath, true);

    return true;
}

bool ResManager::onDocNew(const Engine::Path& projPath) { return true; }

bool ResManager::onDocLoad(const Engine::Path& projPath) { return true; }

bool ResManager::onDocLoaded(const Engine::Path& projPath) { return true; }

bool ResManager::onDocWillClose(void) { return true; }

bool ResManager::onDocClose(void)
{
    // 清理无效的本地缓存文件
    if(!m_validKeys.empty())
        this->_cleanLocalCache();

    // delete ref geo
    for(auto& it: m_key2Geo)
    {
        delete it.second;
    }
    m_key2Geo.clear();

    for(auto& it: m_key2SliceArray)
    {
        delete it.second;
    }
    m_key2SliceArray.clear();

    m_cacheDirtyGeos.clear();

    // delete ref meshes
    for(auto& it: m_key2Mesh)
    {
        STE::assetMgr()->deleteAsset(it.first);
    }
    m_key2Mesh.clear();
    m_mesh2Key.clear();
    m_key2MeshRef.clear();

    m_localCachePath.clear();
    m_validKeys.clear();

    return true;
}

bool ResManager::onAppClose(void) { return true; }

void ResManager::_initBuiltInRes()
{
    STE::assetMgr()->createAssetFromResource(Asset::ASSET_TEXTURE, "Icon\\view_clip.png", (HMODULE)STCore::inst->moduleHandle(), IDB_VIEW_CLIP_ICON,
                                             "PNG");

    STE::assetMgr()->createAssetFromResource(Asset::ASSET_TEXTURE, "Icon\\view_close.png", (HMODULE)STCore::inst->moduleHandle(), IDB_VIEW_CLOSE_ICON,
                                             "PNG");

    STE::assetMgr()->createAssetFromResource(Asset::ASSET_TEXTURE, "Icon\\view_max.png", (HMODULE)STCore::inst->moduleHandle(), IDB_VIEW_MAX_ICON,
                                             "PNG");

    STE::assetMgr()->createAssetFromResource(Asset::ASSET_TEXTURE, "Icon\\view_min.png", (HMODULE)STCore::inst->moduleHandle(), IDB_VIEW_MIN_ICON,
                                             "PNG");

    // 图纸填充图案资源
    STE::assetMgr()->createAssetFromResource(Asset::ASSET_TEXTURE, "Icon\\polygon_pattern_lines.png", (HMODULE)STCore::inst->moduleHandle(),
                                             IDB_POLYGON_PATTERN_LINES, "PNG");
    STE::assetMgr()->createAssetFromResource(Asset::ASSET_TEXTURE, "Icon\\polygon_pattern_chess.png", (HMODULE)STCore::inst->moduleHandle(),
                                             IDB_POLYGON_PATTERN_CHESS, "PNG");
    STE::assetMgr()->createAssetFromResource(Asset::ASSET_TEXTURE, "Icon\\polygon_pattern_bricks.png", (HMODULE)STCore::inst->moduleHandle(),
                                             IDB_POLYGON_PATTERN_BRICKS, "PNG");

    // 字体资源
    STE::assetMgr()->createAssetFromFile(Asset::ASSET_FONT, "fonts\\shs.ttc?fontSize=22");
    STE::assetMgr()->createAssetFromFile(Asset::ASSET_FONT, "fonts\\shs.ttc?fontSize=20");
    STE::assetMgr()->createAssetFromFile(Asset::ASSET_FONT, "fonts\\shs.ttc?fontSize=16");
#if 0
		m_sdfFontUrl = "fonts\\shs.ttc?fontSize=48&sdf";
		STE::assetMgr()->createAssetFromFile(Asset::ASSET_SDF_FONT, m_sdfFontUrl);
		STE::assetMgr()->createAssetFromFile(Asset::ASSET_BITMAP_FONT, "fonts\\shs.ttc?fontSize=16&bmp");
		STE::assetMgr()->createAssetFromFile(Asset::ASSET_MESH_FONT, "fonts\\shs.ttc?fontSize=32&mesh");
#else
    m_sdfFontUrl = STE::assetMgr()->ttFontPath() + "?fontSize=48&sdf";
    if(!STCore::inst->docMgr()->cfgMgr()->settings()->preloadSdfTexts())
    {
        STE::assetMgr()->createAssetFromFile(Asset::ASSET_SDF_FONT, m_sdfFontUrl);
    }
    else
    {
        STE::assetMgr()->createAssetFromFile(Asset::ASSET_SDF_FONT, m_sdfFontUrl, [&](Asset*) {
            std::string fontUrl = STCore::inst->viewMgr()->resMgr()->sdfFontUrl();
            AssetSdfFont* pFontAsset = (AssetSdfFont*)STE::assetMgr()->getAsset(fontUrl);
            AssetBitmapFont* pBmpAsset = (AssetBitmapFont*)STE::assetMgr()->getAsset(fontUrl);
            if(pFontAsset == nullptr || !pFontAsset->isReady())
                return nullptr;

            std::wstring loadingTexts = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            loadingTexts += L"abcdefghijklmnopquvwxyz";
            loadingTexts += L" 0123456789:+-—*×/=!?！:：^_︵~@#$￥%^&<>(),.·，。°、'`\"()（）[]{}|\\";
            loadingTexts += L"πΦ★☆∞";

#ifndef _DEBUG//
            loadingTexts += L"龙宫建模型深化零结构组件钢梁板坡口工程项目创建人负责时间日期细部分析视图文本编辑";
            loadingTexts += L"打开和报告窗帮助重量数质编号规格焊接缝表面螺栓杆帽孔径长度材质单总校对审核制造名称";
            loadingTexts += L"修订版次比例平面位置坐标标高前后顶底";
            loadingTexts += L"纸张说明仅针对表示外形及尺寸实际的应结合要求工艺确定";
            loadingTexts += L"必须上下左右料通常在是否长度方向会被减短多少处理，面显示线条不连续";
            loadingTexts += L"不按比例未打断区域取清单材质";
#endif//
            if(UIFuncs::setStartPageTips)
            {
                UIFuncs::setStartPageTips(STCore::inst->docMgr()->cfgMgr()->localMgr()->local(L"start_page_tips_preload_sdf"));
            }

            if(UIFuncs::initJobProgress && UIFuncs::startPageProgress)
            {
                double progStep = UIFuncs::initJobProgress(L"preload_sdf") / (double)loadingTexts.length();
                double curProgress = UIFuncs::startPageProgress();
                for(auto& curText: loadingTexts)
                {
                    wchar_t pText[2] = { curText, 0 };
                    pFontAsset->getFontData()->evalTextBox(std::wstring(pText), false, false);

                    curProgress += progStep;
                    UIFuncs::setStartPageProgress((int)curProgress);
                }

                UIFuncs::onInitJobDone(L"preload_sdf");
            }
            return nullptr;
        });
    }
    STE::assetMgr()->createAssetFromFile(Asset::ASSET_BITMAP_FONT, STE::assetMgr()->ttFontPath() + "?fontSize=16&bmp");
    STE::assetMgr()->createAssetFromFile(Asset::ASSET_MESH_FONT, STE::assetMgr()->ttFontPath() + "?fontSize=32&mesh");
#endif
    STE::assetMgr()->createAssetFromFile(Asset::ASSET_SHAPE_FONT, "fonts\\hzdx.shp");
}

Engine::MeshData* ResManager::createMesh()
{
    AssetMesh* pAst = (AssetMesh*)STE::assetMgr()->createAsset(Asset::ASSET_MESH);
    MeshData* pMD = pAst->createMeshData();
    pAst->setReady(true);

    m_key2Mesh[pAst->key()] = pMD;
    m_mesh2Key[pMD] = pAst->key();
    return pMD;
}

void ResManager::deleteMesh(Engine::MeshData* pMD)
{
    auto it = m_mesh2Key.find(pMD);
    if(it == m_mesh2Key.end())
        return;

    STE::assetMgr()->deleteAsset(it->second);
    m_key2Mesh.erase(it->second);
    m_mesh2Key.erase(it);
}

GeoData* ResManager::refGeo(const std::string& key, bool localCache)
{
    if(key.empty())
        return nullptr;

    // auto it = m_key2Geo.find(key);
    // if (it != m_key2Geo.end()) {
    //	if (localCache)
    //		it->second->setLocalCache(true);
    //
    //	it->second->ref();
    //	return it->second;
    // }

    GeoData* pGeo = new GeoData();
    // pGeo->setKey(key);
    // pGeo->setLocalCache(localCache);
    // pGeo->ref();
    // pGeo->setReady(false);
    m_key2Geo[key] = pGeo;

    if(localCache)
    {// try load geo data from local cache
        this->_loadCache(pGeo);
    }

    return pGeo;
}

void ResManager::unrefGeo(const std::string& key)
{
    if(key.empty())
        return;

    auto it = m_key2Geo.find(key);
    if(it == m_key2Geo.end())
        return;

    // GeoData* pGeo = it->second;
    // pGeo->unref();
    // if (pGeo->refCount() <= 0) {
    //	m_key2Geo.erase(it);
    //	m_cacheDirtyGeos.erase(pGeo);
    //	delete pGeo;
    // }
}

// GeoSliceArray* ResManager::refSliceArray(const std::string& key)
//{
//	if (key.empty())
//		return nullptr;
//
//	auto it = m_key2SliceArray.find(key);
//	if (it != m_key2SliceArray.end()) {
//		it->second->ref();
//		return it->second;
//	}
//
//	GeoSliceArray* pGeo = new GeoSliceArray();
//	pGeo->setKey(key);
//	pGeo->ref();
//	pGeo->setReady(false);
//	m_key2SliceArray[key] = pGeo;
//	return pGeo;
// }
//
// void ResManager::unrefSliceArray(const std::string& key)
//{
//	if (key.empty())
//		return;
//
//	auto it = m_key2SliceArray.find(key);
//	if (it == m_key2SliceArray.end())
//		return;
//
//	GeoSliceArray* pGeo = it->second;
//	pGeo->unref();
//	if (pGeo->refCount() <= 0) {
//		m_key2SliceArray.erase(it);
//		delete pGeo;
//	}
// }

Engine::MeshData* ResManager::refMesh(const std::string& key)
{
    if(key.empty())
        return nullptr;

    m_key2MeshRef[key] += 1;

    auto it = m_key2Mesh.find(key);
    if(it != m_key2Mesh.end())
        return it->second;

    AssetMesh* pAst = (AssetMesh*)STE::assetMgr()->createAsset(Asset::ASSET_MESH, key);
    MeshData* pMD = pAst->createMeshData();
    pAst->setReady(true);

    m_key2Mesh[key] = pMD;
    m_mesh2Key[pMD] = key;
    return pMD;
}

void ResManager::unrefMesh(const std::string& key)
{
    if(key.empty())
        return;

    auto itref = m_key2MeshRef.find(key);
    if(itref == m_key2MeshRef.end())
        return;

    itref->second -= 1;
    if(itref->second <= 0)
    {
        auto itMD = m_key2Mesh.find(key);
        m_mesh2Key.erase(itMD->second);
        m_key2Mesh.erase(itMD);
        m_key2MeshRef.erase(itref);
        STE::assetMgr()->deleteAsset(key);
    }
}

void ResManager::unrefMesh(Engine::MeshData* pMD)
{
    auto itMD = m_mesh2Key.find(pMD);
    if(itMD == m_mesh2Key.end())
        return;

    // 这里不能用引用，因为itMD会先释放
    std::string key = itMD->second;
    auto itref = m_key2MeshRef.find(key);
    if(itref == m_key2MeshRef.end())
        return;

    itref->second -= 1;
    if(itref->second <= 0)
    {
        m_key2Mesh.erase(key);
        m_mesh2Key.erase(itMD);
        m_key2MeshRef.erase(itref);
        STE::assetMgr()->deleteAsset(key);
    }
}

//	Engine::AssetTexture* ResManager::getLineMaskTex(const std::string& key)
//	{
//		std::string fullKey = std::string("$core$tex_mask$") + key;
//		AssetTexture* pMaskAst = (AssetTexture*)STE::assetMgr()->getAsset(fullKey);
//		if (pMaskAst)
//			return pMaskAst;
//
//		std::vector<std::string> v;
//		StrUtil::split(v, key, ',');
//
//		int len = std::stoi(v[0]);
//		int pitch = align(len, 4);
//		unsigned char* pBuf = new unsigned char[pitch];
//		memset(pBuf, 0, pitch * sizeof(unsigned char));
//
//		int off = 0;
//		for (int i = 1; i < v.size(); i += 2) {
//			off += std::stoi(v[i]);
//			int w = std::stoi(v[i + 1]);
//
//			memset(pBuf + off, 255, w);
//
//			off += w;
//		}
//
//		pMaskAst = (AssetTexture*)STE::assetMgr()->createAsset(Asset::ASSET_TEXTURE, fullKey);
//		Texture2D* pTex = pMaskAst->createTexture2D();
//		pTex->setWidth(len);
//		pTex->setHeight(1);
//		pTex->setFormat(GL_ALPHA8);
//		pTex->setImageData(GL_ALPHA, GL_UNSIGNED_BYTE, pBuf, pitch);
//		pTex->initTexture(false);
//		pMaskAst->setReady(true);
//
//		delete[] pBuf;
//
//		return pMaskAst;
//	}
//
//	Engine::AssetTexture* ResManager::getLineColorStampTex(const std::string& key)
//	{
//		std::string fullKey = std::string("$core$tex_stamp$") + key;
//		AssetTexture* pAst = (AssetTexture*)STE::assetMgr()->getAsset(fullKey);
//		if (pAst)
//			return pAst;
//
//		static std::map<std::string, Vec4i> colorMap = {
//			{"black", {0, 0, 0, 255}},
//			{"white", {255, 255, 255, 255}},
//			{"red", {255, 0, 0, 255}},
//			{"green", {0, 255, 0, 255}},
//			{"blue", {0, 0, 255, 255}},
//			{"yellow", {255, 255, 0, 255}},
//		};
//
//		std::vector<std::string> v;
//		StrUtil::split(v, key, '|');
//
//		int len = 0;
//		for (int i = 0; i < v.size(); ++i) {
//			size_t p = v[i].find(':');
//			if (p == v[i].npos)
//				continue;
//
//			len += std::strtol(v[i].c_str() + p + 1, nullptr, 10);
//		}
//
//		unsigned char* pBuf = new unsigned char[len * 4];
//
//		int n = 0;
//		for (int i = 0; i < v.size(); ++i) {
//			size_t p = v[i].find(':');
//			if (p == v[i].npos)
//				continue;
//
//			std::string scolor = v[i].substr(0, p);
//			int len = std::strtol(v[i].c_str() + p + 1, nullptr, 10);
//
//			auto itColor = colorMap.find(scolor);
//			if (itColor != colorMap.end()) {
//				auto& color = colorMap[scolor];
//				for (int i = 0; i < len; ++i) {
//					pBuf[n++] = color.x;
//					pBuf[n++] = color.y;
//					pBuf[n++] = color.z;
//					pBuf[n++] = color.w;
//				}
//			}
//			else {
//				unsigned int color = (unsigned int)std::stoll(scolor);
//				for (int i = 0; i < len; ++i) {
//					pBuf[n++] = (color & 0xff0000) >> 16;
//					pBuf[n++] = (color & 0xff00) >> 8;
//					pBuf[n++] = (color & 0xff);
//					pBuf[n++] = (color & 0xff000000) >> 24;
//				}
//			}
//		}
//
//		pAst = (AssetTexture*)STE::assetMgr()->createAsset(Asset::ASSET_TEXTURE, fullKey);
//		Texture2D* pTex = pAst->createTexture2D();
//		pTex->setWidth(len);
//		pTex->setHeight(1);
//		pTex->setFormat(GL_RGBA8);
//		pTex->setImageData(GL_RGBA, GL_UNSIGNED_BYTE, pBuf, len * 4);
//		pTex->initTexture(false);
//		pAst->setReady(true);
//
//		delete[] pBuf;
//
//		return pAst;
//	}
//
//	Engine::MaterialBlinnPhong* ResManager::getLineMaskMtrl(unsigned int emi, unsigned int diff, bool light, bool ztest, bool zwrite, bool blend, bool
// cull, uint8_t lineWidth, const std::string& mtrlKey, const std::string& maskKey)
//	{
//		std::string maskTexKey = this->getLineMaskTex(maskKey)->key();
//		std::string fullMtrlKey = mtrlKey + "atest:1; amapuv:0; amapfilter:linear; amapwrap:repeat;amap:" + maskTexKey + ";";
//		return this->getMtrl(emi, diff, light, ztest, zwrite, blend, cull, false, lineWidth, fullMtrlKey);
//	}
//
//	Engine::MaterialBlinnPhong* ResManager::getMtrl(unsigned int emi, unsigned int diff,
//		bool light, bool ztest, bool zwrite, bool blend, bool cull, bool vcolor,
//		uint8_t lineWidth,
//		const std::string& key)
//	{
//		static const uint64_t LIGHT_BIT = uint64_t(1) << 63;
//		static const uint64_t ZTEST_BIT = uint64_t(1) << 62;
//		static const uint64_t ZWRITE_BIT = uint64_t(1) << 61;
//		static const uint64_t BLEND_BIT = uint64_t(1) << 60;
//		static const uint64_t CULL_BIT = uint64_t(1) << 59;
//		static const uint64_t VCOLOR_BIT = uint64_t(1) << 58;
//
//		uint64_t bitKey = (uint64_t(emi) << 32)
//			| diff
//			| (uint64_t(lineWidth) << 24)
//			| (light ? LIGHT_BIT : 0ULL)
//			| (ztest ? ZTEST_BIT : 0ULL)
//			| (zwrite ? ZWRITE_BIT : 0ULL)
//			| (blend ? BLEND_BIT : 0ULL)
//			| (cull ? CULL_BIT : 0ULL)
//			| (vcolor ? VCOLOR_BIT : 0ULL);
//
//		auto it = m_mtrlMap.find(bitKey);
//		if (it != m_mtrlMap.end()) {
//			auto itm = it->second.find(key);
//			if (itm != it->second.end())
//				return itm->second;
//		}
//
//		std::string astKey = std::string("$core$mtrl$") + std::to_string(bitKey) + key;
//		AssetMaterial* pMtrlAst = (AssetMaterial*)STE::assetMgr()->createAsset(Asset::ASSET_MATERIAL, astKey);
//		MaterialBlinnPhong* pMtrl = (MaterialBlinnPhong*)pMtrlAst->createMaterial(Material::MTRL_BLINN_PHONG);
//		m_mtrlMap[bitKey][key] = pMtrl; //cache mtrl
//
//		pMtrl->setEmissive(ARGB_RF(emi), ARGB_GF(emi), ARGB_BF(emi));
//		pMtrl->setDiffuse(ARGB_RF(diff), ARGB_GF(diff), ARGB_BF(diff));
//		pMtrl->setSpecular(0.0f, 0.0f, 0.0f);
//		pMtrl->setGlossiness(32.0f);
//		pMtrl->setSpecularLevel(1.0f);
//		pMtrl->setAlpha(1.0f);
//		pMtrl->setDynamicLighting(light);
//		pMtrl->enableMtrlColor(true);
//		pMtrl->enableVertexColor(vcolor);
//		pMtrl->setDepthTest(ztest);
//		pMtrl->setDepthWrite(zwrite);
//		pMtrl->setBlend(blend);
//		pMtrl->setBlendEq(GL_FUNC_ADD);
//		pMtrl->setBlendSrc(GL_SRC_ALPHA);
//		pMtrl->setBlendDst(GL_ONE_MINUS_SRC_ALPHA);
//		pMtrl->setTwoside(!cull);
//		pMtrl->setLineWidth(lineWidth);
//		pMtrl->setLineStipple(false);
//
//		if (key.empty())
//			return pMtrl;
//
//		size_t off = 0;
//		size_t pos;
//		const char* cbuf = key.c_str();
//		do {
//			pos = key.find(';', off);
//			std::string attrib;
//			std::string val;
//
//			size_t end = (pos == key.npos) ? key.length() : pos;
//			for (int j = (int)off; j < end; ++j) {
//				if (cbuf[j] == ':') {
//					attrib = key.substr(off, j - off);
//					val = key.substr(j + 1, end - j - 1);
//					break;
//				}
//			}
//
//			attrib = StrUtil::trim(attrib);
//
//			if (attrib == "spec") {
//				val = StrUtil::trim(val);
//				Vec3f color;
//				if (val.length() > 2 && val[0] == '0' && val[1] == 'x') { //uint color
//					unsigned int v = std::stoul(val.c_str() + 2, nullptr, 16);
//					color = ARGB_VEC3F(v);
//				}
//				else {
//					size_t p0 = 0;
//					int idx = 0;
//					for (int j = 0; j < val.length(); ++j) {
//						if (val[j] == ',') {
//							color.v[idx++] = std::stof(val.substr(p0, j - p0)) / 255.0f;
//							p0 = j + 1;
//						}
//					}
//
//					color.v[idx++] = std::stof(val.substr(p0)) / 255.0f;
//				}
//
//				pMtrl->setSpecular(color.x, color.y, color.z);
//			}
//			else if (attrib == "glo") {
//				float v = std::stof(val);
//				pMtrl->setGlossiness(v);
//			}
//			else if (attrib == "speclevel") {
//				float v = std::stof(val);
//				pMtrl->setSpecularLevel(v);
//			}
//			else if (attrib == "a") {
//				float v = std::stof(val);
//				pMtrl->setAlpha(v);
//			}
//			else if (attrib == "atest") {
//				bool v = std::stoi(val) == 1;
//				pMtrl->setAlphaTest(v);
//			}
//			else if (attrib == "atestval") {
//				float v = std::stof(val);
//				pMtrl->setAlphaTestValue(v);
//			}
//			else if (attrib == "linew") {
//				float v = std::stof(val);
//				pMtrl->setLineWidth(v);
//			}
//			else if (attrib == "psize") {
//				float v = std::stof(val);
//				pMtrl->setPointSize(v);
//			}
//			else if (attrib == "stppat") {
//				if (SU::startWith(val, "0x")) {
//					uint32_t v = std::stoul(val.c_str() + 2, nullptr, 16);
//					pMtrl->setLineStipplePattern(v);
//				}
//			}
//			else if (attrib == "stpfc") {
//				int v = std::stoi(val);
//				pMtrl->setLineStipple(v > 0);
//				pMtrl->setLineStippleFactor(v);
//			}
//			else if (attrib == "linetype") {
//				int v = std::stoi(val);
//				if (v == Define::DRLINE_TYPE0) {
//					pMtrl->setLineStipple(false);
//				}
// #if 0
//				else if (v == Define::DRLINE_TYPE1) {
//					pMtrl->setLineStipple(true);
//					pMtrl->setLineStippleFactor(1);
//					pMtrl->setLineStipplePattern(0xf0f0u);
//				}
//				else if (v == Define::DRLINE_TYPE2) {
//					pMtrl->setLineStipple(true);
//					pMtrl->setLineStippleFactor(1);
//					pMtrl->setLineStipplePattern(0xfff0u);
//				}
//				else if (v == Define::DRLINE_TYPE3) {
//					pMtrl->setLineStipple(true);
//					pMtrl->setLineStippleFactor(2);
//					pMtrl->setLineStipplePattern(0xff18u);
//				}
//				else if (v == Define::DRLINE_TYPE4) {
//					pMtrl->setLineStipple(true);
//					pMtrl->setLineStippleFactor(1);
//					pMtrl->setLineStipplePattern(0xccccu);
//				}
// #else
//				/// 解决线段缩放到很小时虚线不可见的问题
//				else if (v == Define::DRLINE_TYPE1) {
//					pMtrl->setLineStipple(true);
//					pMtrl->setLineStippleFactor(1);
//					pMtrl->setLineStipplePattern(0x0F0Fu);
//				}
//				else if (v == Define::DRLINE_TYPE2) {
//					pMtrl->setLineStipple(true);
//					pMtrl->setLineStippleFactor(1);
//					pMtrl->setLineStipplePattern(0x0FFFu);
//				}
//				else if (v == Define::DRLINE_TYPE3) {
//					pMtrl->setLineStipple(true);
//					pMtrl->setLineStippleFactor(2);
//					pMtrl->setLineStipplePattern(0x1FE3u);
//				}
//				else if (v == Define::DRLINE_TYPE4) {
//					pMtrl->setLineStipple(true);
//					pMtrl->setLineStippleFactor(1);
//					pMtrl->setLineStipplePattern(0x3333u);
//				}
// #endif
//				else {
//					STASSERT(0);
//				}
//			}
//			else if (attrib == "filltype") {
//				int v = std::stoi(val);
//				switch (v) {
//				case Define::DRFILL_TYPE2: { // 砖块
//					pMtrl->setTileOpacity(true);
//					pMtrl->setTextureOn(MaterialBlinnPhong::MAP_OPACITY, true);
//					pMtrl->setTexture(MaterialBlinnPhong::MAP_OPACITY, "Icon\\polygon_pattern_bricks.png");
//					pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_OPACITY, GL_CLAMP_TO_EDGE);
//					pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_OPACITY, GL_NEAREST);
//				} break;
//				case Define::DRFILL_TYPE3: { // 棋盘
//					pMtrl->setTileOpacity(true);
//					pMtrl->setTextureOn(MaterialBlinnPhong::MAP_OPACITY, true);
//					pMtrl->setTexture(MaterialBlinnPhong::MAP_OPACITY, "Icon\\polygon_pattern_chess.png");
//					pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_OPACITY, GL_CLAMP_TO_EDGE);
//					pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_OPACITY, GL_NEAREST);
//				} break;
//				case Define::DRFILL_TYPE4: { // 斜线
//					pMtrl->setTileOpacity(true);
//					pMtrl->setTextureOn(MaterialBlinnPhong::MAP_OPACITY, true);
//					pMtrl->setTexture(MaterialBlinnPhong::MAP_OPACITY, "Icon\\polygon_pattern_lines.png");
//					pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_OPACITY, GL_CLAMP_TO_EDGE);
//					pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_OPACITY, GL_NEAREST);
//				} break;
//				case Define::DRFILL_TYPE0: // 无
//				default:
//					break;
//				}
//			}
//			else if (attrib == "amap") {
//				pMtrl->setTexture(MaterialBlinnPhong::MAP_OPACITY, val);
//			}
//			else if (attrib == "amapuv") {
//				int v = std::stoi(val);
//				pMtrl->setTextureUV(MaterialBlinnPhong::MAP_OPACITY, v);
//			}
//			else if (attrib == "amapfilter") {
//				val = StrUtil::trim(val);
//
//				if (val == "linear")
//					pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_OPACITY, GL_LINEAR);
//				else
//					pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_OPACITY, GL_NEAREST);
//			}
//			else if (attrib == "amapwrap") {
//				val = StrUtil::trim(val);
//
//				if (val == "clamp")
//					pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_OPACITY, GL_CLAMP_TO_EDGE);
//				else if (val == "repeat")
//					pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_OPACITY, GL_REPEAT);
//				else if (val == "mirror")
//					pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_OPACITY, GL_MIRRORED_REPEAT);
//				else
//					STASSERT(0);
//			}
//			else if (attrib == "emimap") {
//				pMtrl->setTexture(MaterialBlinnPhong::MAP_EMISSIVE, val);
//			}
//			else if (attrib == "emimapuv") {
//				int v = std::stoi(val);
//				pMtrl->setTextureUV(MaterialBlinnPhong::MAP_EMISSIVE, v);
//			}
//			else if (attrib == "emimapfilter") {
//				val = StrUtil::trim(val);
//
//				if (val == "linear")
//					pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_EMISSIVE, GL_LINEAR);
//				else
//					pMtrl->setTextureFilter(MaterialBlinnPhong::MAP_EMISSIVE, GL_NEAREST);
//			}
//			else if (attrib == "emimapwrap") {
//				val = StrUtil::trim(val);
//
//				if (val == "clamp")
//					pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_EMISSIVE, GL_CLAMP_TO_EDGE);
//				else if (val == "repeat")
//					pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_EMISSIVE, GL_REPEAT);
//				else if (val == "mirror")
//					pMtrl->setTextureWrap(MaterialBlinnPhong::MAP_EMISSIVE, GL_MIRRORED_REPEAT);
//				else
//					STASSERT(0);
//			}
//			else if (attrib == "vertcolor") {
//				bool v = std::stoi(val) == 1;
//				pMtrl->enableVertexColor(v);
//			}
//			else if (attrib == "mtrlcolor") {
//				bool v = std::stoi(val) == 1;
//				pMtrl->enableMtrlColor(v);
//			}
//
//			off = pos + 1;
//		} while (pos != key.npos);
//
//		return pMtrl;
//	}

void ResManager::updateValidKeys()
{
    // m_validKeys.clear();
    // for (auto& it : m_key2Geo) {
    //	auto pGeo = it.second;
    //	if (!pGeo->localCache())
    //		continue;
    //
    //	m_validKeys.insert(it.second->key());
    // }
}

void ResManager::diposeInstBatches(int rdId)
{
    // for (auto& it : m_key2Geo) {
    //	auto pGeo = it.second;
    //	pGeo->disposeInstBatches(rdId);
    // }
}

bool ResManager::_cleanLocalCache()
{
    if(m_localCachePath.empty() || !FileUtil::hasDir(m_localCachePath))
        return true;

    std::unordered_set<std::string> validMD5s;
    for(auto& key: m_validKeys)
    {
        validMD5s.insert(StrUtil::md5(key));
    }

    std::vector<Path> vFiles;
    FileUtil::parseFiles(vFiles, m_localCachePath, "", false);

    for(auto& file: vFiles)
    {
        auto md5 = file.file().str();
        if(validMD5s.find(md5) == validMD5s.end())
            FileUtil::deleteFile(file);
    }

    return true;
}

bool ResManager::_saveCache(GeoData* pGeo)
{
    // if (!pGeo || !STCore::inst->docMgr()->cfgMgr()->settings()->geoCache())
    //	return false;
    //
    // std::string md5 = StrUtil::md5(pGeo->key());
    // auto file = m_localCachePath / md5;
    //
    // FU::wait(file);
    //
    // DObject* pFileProps = DObject::alloc();
    // if (FileUtil::hasFile(file)) {
    //	FILE* fp = _wfopen(file.wstr().c_str(), L"rb");
    //	if (fp) {
    //		pFileProps->fromFile(fp);
    //		fclose(fp);
    //	}
    // }
    //
    // DObject* pGeoProps = DObject::alloc();
    // pGeo->dump(pGeoProps);
    // pFileProps->set(pGeo->key(), pGeoProps, true);
    //
    // FU::saveAsync(file, "STGC", pFileProps, "lzma");

    return true;
}

bool ResManager::_loadCache(GeoData* pGeo)
{
    // if (!pGeo || !STCore::inst->docMgr()->cfgMgr()->settings()->geoCache())
    //	return false;
    //
    // std::string md5 = StrUtil::md5(pGeo->key());
    // auto file = m_localCachePath / md5;
    //
    // FU::wait(file);
    // FILE* fp = _wfopen(file.wstr().c_str(), L"rb");
    // if (!fp) {
    //	return false;
    // }
    //
    // if (!FileUtil::parseMagic(fp, "STGC")) {
    //	fclose(fp);
    //	return false;
    // }
    //
    // DObject fileProps;
    // fileProps.fromFile(fp);
    // fclose(fp);
    //
    // const DValue* pGeoProps = nullptr;
    // if (!fileProps.tryGet(pGeo->key(), pGeoProps))
    //	return false;
    //
    // pGeo->modify(*pGeoProps->_dobj());
    // pGeo->computeBBox();
    // pGeo->setReady(true);
    return true;
}

void ResManager::update(unsigned long long tm)
{
    // if (!m_cacheDirtyGeos.empty()) {
    //	for (auto pGeo : m_cacheDirtyGeos) {
    //		if (pGeo->localCache() && !pGeo->geoErr()) {
    //			this->_saveCache(pGeo);
    //		}
    //	}
    //	m_cacheDirtyGeos.clear();
    // }
}

Engine::FontData* ResManager::sceneDefFont()
{
    if(m_pSceneDefFont == nullptr)
    {
        AssetFont* pFontAst = (AssetFont*)STE::assetMgr()->getAsset("fonts\\shs.ttc?fontSize=16");
        if(pFontAst || pFontAst->isReady())
            m_pSceneDefFont = pFontAst->getFontData();
    }

    return m_pSceneDefFont;
}

Engine::FontData* ResManager::sceneDefFontHover()
{
    if(m_pSceneDefFontHover == nullptr)
    {
        AssetFont* pFontAst = (AssetFont*)STE::assetMgr()->getAsset("fonts\\shs.ttc?fontSize=20");
        if(pFontAst || pFontAst->isReady())
            m_pSceneDefFontHover = pFontAst->getFontData();
    }

    return m_pSceneDefFontHover;
}
}// namespace sketch
