/*
 * _Contrast.cpp
 *
 *  Created on: March 12, 2019
 *      Author: yankai
 */

#include "_Contrast.h"

namespace kai
{

	_Contrast::_Contrast()
	{
		m_type = vision_contrast;
		m_pV = NULL;

		m_alpha = 1.0;
		m_beta = 0.0;
	}

	_Contrast::~_Contrast()
	{
		close();
	}

	bool _Contrast::init(void *pKiss)
	{
		IF_F(!_VisionBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("alpha", &m_alpha);
		pK->v("beta", &m_beta);

		string n;
		n = "";
		pK->v("_VisionBase", &n);
		m_pV = (_VisionBase *)(pK->findModule(n));
		IF_Fl(!m_pV, n + ": not found");

		return true;
	}

	bool _Contrast::open(void)
	{
		NULL_F(m_pV);
		m_bOpen = m_pV->isOpened();

		return m_bOpen;
	}

	void _Contrast::close(void)
	{
		this->_VisionBase::close();
	}

	bool _Contrast::start(void)
	{
		NULL_F(m_pT);
		return m_pT->start(getUpdate, this);
	}

	void _Contrast::update(void)
	{
		while (m_pT->bAlive())
		{
			if (!m_bOpen)
				open();

			m_pT->autoFPSfrom();

			if (m_bOpen)
			{
				filter();
			}

			m_pT->autoFPSto();
		}
	}

	void _Contrast::filter(void)
	{
		IF_(m_pV->getFrameRGB()->bEmpty());

		Mat m;
		m_pV->getFrameRGB()->m()->convertTo(m, -1, m_alpha, m_beta);
		m_fRGB.copy(m);
	}

}
