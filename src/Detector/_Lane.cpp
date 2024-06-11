/*
 * _Lane.cpp
 *
 *  Created on: Mar 5, 2018
 *      Author: yankai
 */

#include "_Lane.h"

#ifdef USE_OPENCV

namespace kai
{

	_Lane::_Lane()
	{
		m_pV = NULL;

		m_roiLT.x = 0.2;
		m_roiLT.y = 0.5;
		m_roiLB.x = 0.0;
		m_roiLB.y = 1.0;
		m_roiRT.x = 0.8;
		m_roiRT.y = 0.5;
		m_roiRB.x = 1.0;
		m_roiRB.y = 1.0;

		m_sizeOverhead.x = 400;
		m_sizeOverhead.y = 300;
		m_vSize.clear();
		m_binMed = 3;

		m_nFilter = 0;
		m_nLane = 0;
		m_ppPoint = NULL;
		m_pNp = NULL;
		m_bDrawOverhead = false;
		m_bDrawFilter = false;
	}

	_Lane::~_Lane()
	{
	}

	bool _Lane::init(void *pKiss)
	{
		IF_F(!this->_ModuleBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("bDrawOverhead", &m_bDrawOverhead);
		pK->v("bDrawFilter", &m_bDrawFilter);
		pK->v("binMed", &m_binMed);

		F_INFO(pK->v("roiLTx", &m_roiLT.x));
		F_INFO(pK->v("roiLTy", &m_roiLT.y));
		F_INFO(pK->v("roiLBx", &m_roiLB.x));
		F_INFO(pK->v("roiLBy", &m_roiLB.y));
		F_INFO(pK->v("roiRTx", &m_roiRT.x));
		F_INFO(pK->v("roiRTy", &m_roiRT.y));
		F_INFO(pK->v("roiRBx", &m_roiRB.x));
		F_INFO(pK->v("roiRBy", &m_roiRB.y));
		F_INFO(pK->v("overheadW", &m_sizeOverhead.x));
		F_INFO(pK->v("overheadH", &m_sizeOverhead.y));

		m_mOverhead = Mat(Size(m_sizeOverhead.x, m_sizeOverhead.y), CV_8UC3);

		// color filters
		Kiss *pKF = pK->child("colorFilter");
		NULL_Fl(pKF, "colorFilter not found");

		Kiss *pO;
		m_nFilter = 0;
		while (1)
		{
			IF_F(m_nFilter >= N_LANE_FILTER);
			pO = pKF->child(m_nFilter);
			if (pO->empty())
				break;

			LANE_FILTER *pF = &m_pFilter[m_nFilter];
			F_INFO(pO->v("iColorSpace", &pF->m_iColorSpace));
			F_INFO(pO->v("iChannel", &pF->m_iChannel));
			F_INFO(pO->v("nTile", &pF->m_nTile));
			F_INFO(pO->v("thr", &pF->m_thr));
			F_INFO(pO->v("clipLim", &pF->m_clipLim));
			pF->init();

			m_nFilter++;
		}

		// lanes
		int nAvr = 0;
		F_INFO(pK->v("nAvr", &nAvr));
		int nMed = 0;
		F_INFO(pK->v("nMed", &nMed));

		Kiss *pKL = pK->child("lane");
		NULL_Fl(pKL, "lane not found");

		m_nLane = 0;
		while (1)
		{
			IF_F(m_nLane >= N_LANE_FILTER);
			pO = pKL->child(m_nLane);
			if (pO->empty())
				break;

			LANE *pLane = &m_pLane[m_nLane];
			pLane->init(m_sizeOverhead.y, nAvr, nMed);
			F_INFO(pO->v("l", &pLane->m_ROI.x));
			F_INFO(pO->v("t", &pLane->m_ROI.y));
			F_INFO(pO->v("r", &pLane->m_ROI.z));
			F_INFO(pO->v("b", &pLane->m_ROI.w));

			m_nLane++;
		}

		m_ppPoint = new Point *[m_nLane];
		m_pNp = new int[m_nLane];
		for (int i = 0; i < m_nLane; i++)
		{
			m_ppPoint[i] = new Point[m_sizeOverhead.y];
			m_pNp[i] = m_sizeOverhead.y;
		}

		string n = "";
		F_ERROR_F(pK->v("_VisionBase", &n));
		m_pV = (_VisionBase *)(pK->getInst(n));
		NULL_Fl(m_pV, "_VisionBase is NULL");

		return true;
	}

	bool _Lane::start(void)
	{
		NULL_F(m_pT);
		return m_pT->start(getUpdate, this);
	}

	int _Lane::check(void)
	{
		NULL__(m_pV, -1);
		IF__(m_pV->getFrameRGB()->m()->empty(), -1);

		return this->_ModuleBase::check();
	}

	void _Lane::update(void)
	{
		while (m_pT->bAlive())
		{
			m_pT->autoFPSfrom();

			detect();

			m_pT->autoFPSto();
		}
	}

	void _Lane::detect(void)
	{
		IF_(check() < 0);
		Mat *pM = m_pV->getFrameRGB()->m();

		//Warp transform to get overhead view
		if (m_vSize.x != pM->cols || m_vSize.y != pM->rows)
		{
			m_vSize.x = pM->cols;
			m_vSize.y = pM->rows;
			updateVisionSize();
		}

		cv::warpPerspective(*pM, m_mOverhead, m_mPerspective,
							m_mOverhead.size(), cv::INTER_LINEAR);

		//Filter image to get binary view
		filterBin();

		for (int i = 0; i < m_nLane; i++)
		{
			LANE *pL = &m_pLane[i];
			pL->findLane(m_mBin);
			pL->updatePolyFit();
			pL->updateDeviation();
		}
	}

	void _Lane::filterBin(void)
	{
		Mat mAll = Mat::zeros(m_mOverhead.size(), CV_8UC1);

		for (int i = 0; i < m_nFilter; i++)
		{
			mAll += m_pFilter[i].filter(m_mOverhead);
		}

		cv::normalize(mAll, mAll, 0, 255, cv::NORM_MINMAX);
		cv::medianBlur(mAll, m_mBin, m_binMed);
	}

	void _Lane::updateVisionSize(void)
	{
		Point2f LT = Point2f((float)(m_roiLT.x * m_vSize.x),
							 (float)(m_roiLT.y * m_vSize.y));
		Point2f LB = Point2f((float)(m_roiLB.x * m_vSize.x),
							 (float)(m_roiLB.y * m_vSize.y));
		Point2f RT = Point2f((float)(m_roiRT.x * m_vSize.x),
							 (float)(m_roiRT.y * m_vSize.y));
		Point2f RB = Point2f((float)(m_roiRB.x * m_vSize.x),
							 (float)(m_roiRB.y * m_vSize.y));

		//LT, LB, RB, RT
		Point2f ptsFrom[] = {LT, LB, RB, RT};
		Point2f ptsTo[] =
			{cv::Point2f(0, 0),
			 cv::Point2f(0, (float)m_sizeOverhead.y),
			 cv::Point2f((float)m_sizeOverhead.x, (float)m_sizeOverhead.y),
			 cv::Point2f((float)m_sizeOverhead.x, 0)};

		m_mPerspective = getPerspectiveTransform(ptsFrom, ptsTo);
		m_mPerspectiveInv = getPerspectiveTransform(ptsTo, ptsFrom);
	}

	void _Lane::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_ModuleBase::console(pConsole);

		for (int i = 0; i < m_nLane; i++)
		{
			LANE *pL = &m_pLane[i];
			IF_CONT(!pL->m_pPoly);
			string msg = "Lane " + i2str(i) + ": " + f2str(pL->m_pPoly[0]) + " " + f2str(pL->m_pPoly[1]) + " " + f2str(pL->m_pPoly[2]);
			((_Console *)pConsole)->addMsg(msg, 1);
		}
	}

	void _Lane::draw(void* pFrame)
	{
		NULL_(pFrame);
		this->_ModuleBase::draw(pFrame);
		IF_(check() < 0);

		Frame *pF = (Frame*)pFrame;
		Mat *pM = pF->m();
		IF_(pM->empty());

		IF_(m_mPerspectiveInv.empty());

		int i, j;
		LANE *pL;

		//visualization of the lane
		Mat mLane = Mat::zeros(m_mOverhead.size(), CV_8UC1);
		Mat mOverlay = Mat::zeros(pM->size(), CV_8UC1);

		for (i = 0; i < m_nLane; i++)
		{
			pL = &m_pLane[i];
			IF_CONT(!pL->m_pPoly);

			for (j = 0; j < m_sizeOverhead.y; j++)
			{
				m_ppPoint[i][j] = Point(pL->vPoly(j), j);
			}
		}

		polylines(mLane, m_ppPoint, m_pNp, m_nLane, false, cv::Scalar(255), 3, 4);
		cv::warpPerspective(mLane, mOverlay, m_mPerspectiveInv, mOverlay.size(), cv::INTER_LINEAR);

		vector<cv::Mat> vBGR;
		cv::split(*pM, vBGR);
		vBGR[0] -= mOverlay;
		vBGR[1] -= mOverlay;
		vBGR[2] += mOverlay;
		cv::merge(vBGR, *pM);

		if (m_bDrawOverhead && !m_mOverhead.empty())
		{
			Mat mO = m_mOverhead.clone();

			for (i = 0; i < m_nLane; i++)
			{
				pL = &m_pLane[i];
				IF_CONT(!pL->m_pPoly);

				for (j = 0; j < m_sizeOverhead.y; j++)
				{
					circle(mO, Point(pL->vPoly(j), j), 1, Scalar(0, 255, 0), 1);
					circle(mO, Point(pL->vFilter(j), j), 1, Scalar(0, 0, 255), 1);
				}
			}

			imshow(this->getName() + ":Overhead", mO);
		}

		if (m_bDrawOverhead && !m_mBin.empty())
		{
			imshow(this->getName() + ":Bin", m_mBin);
		}
	}

}
#endif
