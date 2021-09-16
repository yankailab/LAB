/*
 * _ARmeasureCalib.cpp
 *
 *  Created on: July 26, 2021
 *      Author: yankai
 */

#include "_ARmeasureCalib.h"

namespace kai
{

	_ARmeasureCalib::_ARmeasureCalib()
	{
		m_pA = NULL;
		m_pV = NULL;
		m_pW = NULL;

		m_area = 0;
		m_Dtot = 0;

		m_drawCol = Scalar(0, 255, 0);
		m_pFt = NULL;
	}

	_ARmeasureCalib::~_ARmeasureCalib()
	{
	}

	bool _ARmeasureCalib::init(void *pKiss)
	{
		IF_F(!this->_StateBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		string n;

		n = "";
		pK->v("_ARmeasure", &n);
		m_pA = (_ARmeasure *)(pK->getInst(n));
		IF_Fl(!m_pA, n + " not found");

		n = "";
		pK->v("_VisionBase", &n);
		m_pV = (_VisionBase *)(pK->getInst(n));
		IF_Fl(!m_pV, n + " not found");

		n = "";
		pK->v("_WindowCV", &n);
		m_pW = (_WindowCV *)(pK->getInst(n));
		IF_Fl(!m_pW, n + " not found");

		m_pW->setCbBtn("Action", sOnBtnAction, this);
		m_pW->setCbBtn("Clear", sOnBtnClear, this);

		return true;
	}

	bool _ARmeasureCalib::start(void)
	{
		NULL_F(m_pT);
		return m_pT->start(getUpdate, this);
	}

	int _ARmeasureCalib::check(void)
	{
		NULL__(m_pA, -1);
		NULL__(m_pW, -1);
		NULL__(m_pV, -1);
		IF__(m_pV->BGR()->bEmpty(), -1);

		return 0;
	}

	void _ARmeasureCalib::update(void)
	{
		while (m_pT->bRun())
		{
			m_pT->autoFPSfrom();

			this->_StateBase::update();
			updateVertex();

			m_pT->autoFPSto();
		}
	}

	void _ARmeasureCalib::updateVertex(void)
	{
		IF_(!bActive());

		int nV = m_vVert.size();
		Vector3f vA(0, 0, 0);
		int j = 0;

		for (int i = 0; i < nV; i++)
		{
			j = (i + 1) % nV;
			vA += m_vVert[i].m_vVertW.cross(m_vVert[j].m_vVertW);
		}

		vA *= 0.5;
		m_area = vA.norm();
	}

	// callbacks
	void _ARmeasureCalib::sOnBtnAction(void *pInst, uint32_t f)
	{
		NULL_(pInst);
		((_ARmeasureCalib *)pInst)->action();
	}

	void _ARmeasureCalib::sOnBtnClear(void *pInst, uint32_t f)
	{
		NULL_(pInst);
		((_ARmeasureCalib *)pInst)->clear();
	}

	// UI handler
	void _ARmeasureCalib::action(void)
	{
		IF_(!m_pA->bValid());

		// add new vertex
		ARAREA_VERTEX av;
		av.m_vVertW = m_pA->vDptW();
		m_vVert.push_back(av);
	}

	void _ARmeasureCalib::clear(void)
	{
		m_vVert.clear();
	}

	// UI draw
	void _ARmeasureCalib::drawVertices(Mat *pM)
	{
		NULL_(pM);
		NULL_(m_pFt);

		vFloat2 vF = m_pV->getFf();
		vFloat2 vC = m_pV->getCf();
		cv::Size s = m_pV->BGR()->size();

		// vertices
		int i, j;
		int nV = m_vVert.size();
		for (i = 0; i < nV; i++)
		{
			ARAREA_VERTEX *pA = &m_vVert[i];
			Vector3f vPc = m_pA->aW2C() * pA->m_vVertW;
			pA->m_bZ = m_pA->c2scr(vPc, s, vF, vC, &pA->m_vPs);

			IF_CONT(!pA->m_bZ);
			IF_CONT(!m_pA->bInsideScr(s, pA->m_vPs));

			circle(*pM, pA->m_vPs, 10, m_drawCol, 3, cv::LINE_AA);
		}

		IF_(nV <= 1);

		// distances between vertices
		for (i = 0; i < nV; i++)
		{
			j = (i + 1) % nV;
			ARAREA_VERTEX *pA = &m_vVert[i];
			ARAREA_VERTEX *pB = &m_vVert[j];
			IF_CONT(!pA->m_bZ && !pB->m_bZ);

			Point p = pA->m_vPs;
			Point q = pB->m_vPs;
			IF_CONT(!clipLine(s, p, q));

			line(*pM, p, q, m_drawCol, 3, cv::LINE_AA);
			Vector3f vD = pA->m_vVertW - pB->m_vVertW;

			string sL = f2str(vD.norm(), 2) + "m";
			m_pFt->putText(*pM, sL,
						   (p + q) * 0.5,
						   20,
						   m_drawCol,
						   -1,
						   cv::LINE_AA,
						   false);
		}
	}

	void _ARmeasureCalib::drawMeasure(Mat *pM)
	{
		NULL_(pM);
		NULL_(m_pFt);

		int nV = m_vVert.size();
		IF_(nV <= 2);

		// area
		string sA = "Area = " + f2str(m_area, 2);
		int baseline = 0;
		Size ts = m_pFt->getTextSize(sA,
									 40,
									 -1,
									 &baseline);

		Point pt;
		pt.x = 200;
		pt.y = pM->rows - 45;

		m_pFt->putText(*pM, sA,
					   pt,
					   40,
					   Scalar(255, 255, 255),
					   -1,
					   cv::LINE_AA,
					   false);

		pt.x += ts.width + 10;
		m_pFt->putText(*pM, "m",
					   pt,
					   40,
					   Scalar(255, 255, 255),
					   -1,
					   cv::LINE_AA,
					   false);

		pt.x += 25;
		m_pFt->putText(*pM, "2",
					   pt,
					   25,
					   Scalar(255, 255, 255),
					   -1,
					   cv::LINE_AA,
					   false);
	}

	void _ARmeasureCalib::cvDraw(void *pWindow)
	{
		NULL_(pWindow);
		this->_ModuleBase::cvDraw(pWindow);
		IF_(check() < 0);

		_WindowCV *pWin = (_WindowCV *)pWindow;
		Frame *pF = pWin->getFrame();
		NULL_(pF);
		Mat *pMw = pF->m();
		IF_(pMw->empty());
		m_pFt = pWin->getFont();

		// video input
		Mat mV;
		m_pV->BGR()->m()->copyTo(mV);

		drawVertices(&mV);

		Rect r;
		r.x = 0;
		r.y = 0;
		r.width = mV.cols;
		r.height = mV.rows;
		mV.copyTo((*pMw)(r));

		drawMeasure(pMw);
	}

}
