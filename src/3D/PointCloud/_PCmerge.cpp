/*
 * _PCmerge.cpp
 *
 *  Created on: May 24, 2020
 *      Author: yankai
 */

#include "_PCmerge.h"

namespace kai
{

	_PCmerge::_PCmerge()
	{
		m_rVoxel = 0.0;
	}

	_PCmerge::~_PCmerge()
	{
	}

	int _PCmerge::init(void *pKiss)
	{
		CHECK_(_GeometryBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("rVoxel", &m_rVoxel);

		string n;

		vector<string> vPCB;
		pK->a("vPCbase", &vPCB);
		IF__(vPCB.empty(), OK_ERR_NOT_FOUND);

		for (string p : vPCB)
		{
			_GeometryBase *pPCB = (_GeometryBase *)(pK->findModule(p));
			IF_CONT(!pPCB);

			m_vpGB.push_back(pPCB);
		}
		IF__(m_vpGB.empty(), OK_ERR_NOT_FOUND);

		return OK_OK;
	}

	int _PCmerge::start(void)
	{
		NULL__(m_pT, OK_ERR_NULLPTR);
		return m_pT->start(getUpdate, this);
	}

	int _PCmerge::check(void)
	{
		return this->_GeometryBase::check();
	}

	void _PCmerge::update(void)
	{
		while (m_pT->bAlive())
		{
			m_pT->autoFPSfrom();

			updateMerge();

			m_pT->autoFPSto();
		}
	}

	void _PCmerge::updateMerge(void)
	{
		IF_(check() != OK_OK);

		// read all inputs into one ring
		for (_GeometryBase *pPCB : m_vpGB)
		{
			//		m_ring.readSrc(pPCB->getRing());
		}
	}

}
