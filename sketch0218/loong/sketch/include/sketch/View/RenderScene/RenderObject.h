#pragma once  

#include <string>
#include <mutex>

#include "Engine/Math/Vec3.h"
#include "Engine/Math/AxisPlane.h"
#include "Engine/Scene/Object3D.h"

#include "sketch/Define.h"
#include "sketch/Document/DocElement.h"
#include "sketch/Document/DocListener.h"
#include "sketch/Document/Scene/TFObjectDoc.h"
#include "sketch/View/RenderView/View.h"

namespace Engine
{
	class MeshData;
	class DrawBatch;
}

namespace sketch
{
	class View;
	class RenderScene;

	class  RenderObject : public Engine::Object3D, public DocListener
	{
	public:
		static const double CTRL_POINT_SIZE;

	public:
		RenderObject(RenderScene* pScene);
		virtual ~RenderObject();

		inline RenderScene* scene() const {
			return m_pScene;
		}

		inline long long id() const {
			return m_pDoc->id();
		}

		inline Define::ObjectType type() const {
			return m_type;
		}

		virtual void setDoc(ObjectDoc* pObjDoc);
		inline TFObjectDoc* doc() const {
			return m_pDoc;
		}

		virtual void onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* params, DocElement* pInvoker) override;

		virtual bool isViewContain(View* pView);

		inline Define::ViewLod lod(View* pView) const {
			//NOTE: lod目前在大场景中非但不能提高性能，反而造成额外开销，暂时屏蔽
			//auto it = m_lod.find(pView->id());
			//if (it != m_lod.end()) 
			//	return it->second.second;

			return Define::VIEWLOD_HIGH;
		}

		virtual void prepareDrawBatch(std::vector<Engine::DrawBatch*>& opaqueBatches, std::vector<Engine::DrawBatch*>& transBatches, View* pView);

		virtual void prepare_render(View* view);

	protected:
		virtual void _updateMatrix();
		virtual void _onUpdateGeometry();
		virtual void _onUpdateTransform();
	protected:
		RenderScene* m_pScene;
		Define::ObjectType m_type;
		TFObjectDoc* m_pDoc;

		bool m_geoDirty;
	};
}