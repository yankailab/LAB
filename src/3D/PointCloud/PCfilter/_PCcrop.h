/*
 * _PCcrop.h
 *
 *  Created on: Sept 3, 2020
 *      Author: yankai
 */

#ifndef OpenKAI_src_3D_PointCloud_PCcrop_H_
#define OpenKAI_src_3D_PointCloud_PCcrop_H_

#include "../../_GeometryBase.h"

namespace kai
{

	enum POINTCLOUD_VOL_TYPE
	{
		pc_vol_box = 0,
		pc_vol_ball = 1,
		pc_vol_cylinder = 2,
	};

	struct POINTCLOUD_VOL
	{
		POINTCLOUD_VOL_TYPE m_type;
		bool m_bInside; //true: inside valid

		vFloat2 m_vX;
		vFloat2 m_vY;
		vFloat2 m_vZ;

		vFloat3 m_vC;
		vFloat2 m_vR;

		void init(void)
		{
			m_type = pc_vol_ball;
			m_bInside = false;
			m_vX.clear();
			m_vY.clear();
			m_vZ.clear();
			m_vC.clear();
			m_vR.clear();
		}

		bool bValid(vFloat3 &vP)
		{
			bool bInside = true;

			if (m_type == pc_vol_box)
			{
				if (vP.x < m_vX.x)
					bInside = false;
				if (vP.x > m_vX.y)
					bInside = false;
				if (vP.y < m_vY.x)
					bInside = false;
				if (vP.y > m_vY.y)
					bInside = false;
				if (vP.z < m_vZ.x)
					bInside = false;
				if (vP.z > m_vZ.y)
					bInside = false;
			}
			else if (m_type == pc_vol_ball)
			{
				vFloat3 vR = vP - m_vC;
				float r = vR.len();
				if (r < m_vR.x)
					bInside = false;
				if (r > m_vR.y)
					bInside = false;
			}
			else if (m_type == pc_vol_cylinder)
			{
				vFloat3 vR = vP - m_vC;
				vR.z = 0.0;
				float r = vR.len();
				if (r < m_vR.x)
					bInside = false;
				if (r > m_vR.y)
					bInside = false;
			}

			IF_F(m_bInside != bInside);
			return true;
		}
	};

	class _PCcrop : public _GeometryBase
	{
	public:
		_PCcrop();
		virtual ~_PCcrop();

		int init(void *pKiss);
		int start(void);
		int check(void);

	private:
		void update(void);
		void updateFilter(void);
		virtual bool bFilter(Eigen::Vector3d &vP);
		static void *getUpdate(void *This)
		{
			((_PCcrop *)This)->update();
			return NULL;
		}

	public:
		vector<POINTCLOUD_VOL> m_vFilter;
	};

}
#endif
