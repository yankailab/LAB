/*
 * _GPhoto.cpp
 *
 *  Created on: Feb 20, 2020
 *      Author: yankai
 */

#include "_GPhoto.h"

namespace kai
{

	_GPhoto::_GPhoto()
	{
		m_type = vision_gphoto;

		m_cmdUnmount = "gio mount -s gphoto2";
	}

	_GPhoto::~_GPhoto()
	{
		close();
	}

	int _GPhoto::init(void *pKiss)
	{
		CHECK_(_VisionBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("mount", &m_cmdUnmount);

		return OK_OK;
	}

	bool _GPhoto::open(void)
	{
		if (!m_cmdUnmount.empty())
			system(m_cmdUnmount.c_str());

		return true;
	}

	void _GPhoto::close(void)
	{
		this->_VisionBase::close();
	}

	int _GPhoto::start(void)
	{
		NULL__(m_pT, OK_ERR_NULLPTR);
		return m_pT->start(getUpdate, this);
	}

	void _GPhoto::update(void)
	{
		while (m_pT->bAlive())
		{
			if (!m_bOpen)
			{
				if (!open())
				{
					m_pT->sleepT(SEC_2_USEC);
					continue;
				}
			}

			m_pT->autoFPS();

		}
	}

	bool _GPhoto::shutter(string &fName)
	{
		return true;
	}

}
