#include "_APmavlink_depthVision.h"

namespace kai
{

	_APmavlink_depthVision::_APmavlink_depthVision()
	{
		m_pAP = nullptr;
		m_pDV = nullptr;
		m_nROI = 0;
	}

	_APmavlink_depthVision::~_APmavlink_depthVision()
	{
	}

	int _APmavlink_depthVision::init(void *pKiss)
	{
		CHECK_(this->_ModuleBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		//link
		string n;
		n = "";
		pK->v("APcopter_base", &n);
		m_pAP = (_APmavlink_base *)(pK->findModule(n));

		n = "";
		pK->v("_RGBDbase", &n);
		m_pDV = (_RGBDbase *)(pK->findModule(n));
		NULL__(m_pDV, OK_ERR_NOT_FOUND);

		m_nROI = 0;
		while (1)
		{
			IF__(m_nROI >= N_DEPTH_ROI, OK_ERR_INVALID_VALUE);
			Kiss *pKs = pK->child(m_nROI);
			if (pKs->empty())
				break;

			DEPTH_ROI *pR = &m_pROI[m_nROI];
			pR->init();
			pKs->v("orientation", (int *)&pR->m_orientation);
			pKs->v("l", &pR->m_roi.x);
			pKs->v("t", &pR->m_roi.y);
			pKs->v("r", &pR->m_roi.z);
			pKs->v("b", &pR->m_roi.w);

			m_nROI++;
		}

		return OK_OK;
	}

	void _APmavlink_depthVision::update(void)
	{
		NULL_(m_pAP);
		NULL_(m_pAP->m_pMav);
		_Mavlink *pMavlink = m_pAP->m_pMav;
		NULL_(m_pDV);

		vFloat2 range = m_pDV->getRangeD();
		mavlink_distance_sensor_t D;

		for (int i = 0; i < m_nROI; i++)
		{
			DEPTH_ROI *pR = &m_pROI[i];

			float d = m_pDV->d(pR->m_roi);
			if (d <= range.x)
				d = range.y;
			if (d > range.y)
				d = range.y;
			pR->m_minD = d;

			D.type = 0;
			D.max_distance = (uint16_t)(range.y * 100); //unit: centimeters
			D.min_distance = (uint16_t)(range.x * 100);
			D.current_distance = (uint16_t)(pR->m_minD * 100);
			D.orientation = pR->m_orientation;
			D.covariance = 255;

			pMavlink->distanceSensor(D);
			LOG_I("orient: " + i2str(pR->m_orientation) + " minD: " + f2str(pR->m_minD));
		}
	}

	void _APmavlink_depthVision::draw(void *pFrame)
	{
		NULL_(pFrame);
		this->_ModuleBase::draw(pFrame);
		IF_(check() != OK_OK);

		Frame *pF = (Frame *)pFrame;
		Mat *pM = pF->m();

		NULL_(m_pDV);

		for (int i = 0; i < m_nROI; i++)
		{
			DEPTH_ROI *pR = &m_pROI[i];
			vFloat4 roi = pR->m_roi;
			float d = m_pDV->d(roi);

			Rect r;
			r.x = roi.x * pM->cols;
			r.y = roi.y * pM->rows;
			r.width = roi.z * pM->cols - r.x;
			r.height = roi.w * pM->rows - r.y;
			rectangle(*pM, r, Scalar(0, 255, 0), 1);

			putText(*pM, f2str(d),
					Point(r.x + 15, r.y + 25),
					FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 255), 1);
		}
	}

}
