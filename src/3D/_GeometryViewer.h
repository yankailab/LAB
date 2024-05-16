/*
 * _GeometryViewer.h
 *
 *  Created on: May 28, 2020
 *      Author: yankai
 */

#ifndef OpenKAI_src_3D__GeometryViewer_H_
#define OpenKAI_src_3D__GeometryViewer_H_

#include "PointCloud/_PCstream.h"
#include "PointCloud/_PCframe.h"
#include "PointCloud/_PCgrid.h"
#include "../UI/O3DUI.h"

namespace kai
{
    enum PC_THREAD_ACTION
	{
		pc_ScanReset,
		pc_ScanSet,
		pc_ScanStart,
		pc_ScanStop,
		pc_ScanTake,
		pc_SavePC,
		pc_Scanning,
		pc_VoxelDown,
		pc_HiddenRemove,
		pc_ResetPC,
		pc_CamAuto,
		pc_CamCtrl,
		pc_RefreshCol,
	};

	struct CAM_PROJ
	{
		float m_fov = 70.0;
		vFloat2 m_vNF = {0, FLT_MAX};	//near far plane
		uint8_t m_fovType = 0;
	};

	struct PC_CAM
	{
		vFloat3 m_vLookAt = {0,0,0};
		vFloat3 m_vEye = {0,0,1};
		vFloat3 m_vUp = {0,1,0};
	};

	class _GeometryViewer : public _GeometryBase
	{
	public:
		_GeometryViewer();
		virtual ~_GeometryViewer();

		virtual bool init(void *pKiss);
		virtual bool link(void);
		virtual bool start(void);
		virtual int check(void);

		virtual void resetCamPose(void);

        virtual void getPCstream(void* p, const uint64_t& tExpire);
        virtual void getPCframe(void* p);
        virtual void getPCgrid(void* p);

	protected:

		// point cloud
		void readAllPC(void);
		void addUIpc(const PointCloud& pc, const string& name);
		void updateUIpc(const PointCloud& pc, const string& name);
		void removeUIpc(const string& name);
		void adjustNpoints(PointCloud* pPC, int nP, int nPbuf);
		void addDummyPoints(PointCloud* pPC, int n, float r, Vector3d vCol = {0,0,0});

		// update thread
		virtual void updateGeometry(void);
		virtual void update(void);
		static void *getUpdate(void *This)
		{
			((_GeometryViewer *)This)->update();
			return NULL;
		}

		// UI
		void updateCamProj(void);
		void updateCamPose(void);
		void camBound(const AxisAlignedBoundingBox& aabb);
		AxisAlignedBoundingBox createDefaultAABB(void);
		
		virtual void updateUI(void);
		static void *getUpdateUI(void *This)
		{
			((_GeometryViewer *)This)->updateUI();
			return NULL;
		}

	protected:
		// point cloud
		int m_nPbuf;
		PointCloud m_PC;
		t::geometry::PointCloud m_Tpc;

		// line set
		LineSet m_staticLineSet;
		LineSet m_dynamicLineSet;

		O3DUI* m_pWin;
		UIState* m_pUIstate;
		_Thread *m_pTui;
		string m_modelName;
		string m_dirSave;

		string m_pathRes;
		Visualizer m_vis;
		vInt2 m_vWinSize;
		string m_device;

		bool m_bFullScreen;
		bool m_bSceneCache;
		int	m_wPanel;
		vFloat2 m_vBtnPadding;
		int m_mouseMode;
		vFloat2 m_vDmove;
		float m_rDummyDome;

		CAM_PROJ m_camProj;
		PC_CAM m_cam;
		PC_CAM m_camDefault;
		PC_CAM m_camAuto;
		vFloat3 m_vCoR;
		AxisAlignedBoundingBox m_aabb;

		vector<_GeometryBase *> m_vpGB;
	};

}
#endif
