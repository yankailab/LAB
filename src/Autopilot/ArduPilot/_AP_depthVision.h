
#ifndef OpenKAI_src_Autopilot__AP_depthVision_H_
#define OpenKAI_src_Autopilot__AP_depthVision_H_

#include "../../Vision/RGBD/_RGBDbase.h"
#include "_AP_base.h"

#define N_DEPTH_ROI 16

namespace kai
{

	struct DEPTH_ROI
	{
		uint8_t m_orientation;
		vFloat4 m_roi;
		float m_minD;

		void init(void)
		{
			m_minD = 0.0;
			m_orientation = 0;
			m_roi.clear();
		}
	};

	class _AP_depthVision : public _ModuleBase
	{
	public:
		_AP_depthVision();
		~_AP_depthVision();

		int init(void *pKiss);
		void update(void);
		void draw(void *pFrame);

	private:
		_AP_base *m_pAP;
		_RGBDbase *m_pDV;

		int m_nROI;
		DEPTH_ROI m_pROI[N_DEPTH_ROI];
	};

}
#endif
