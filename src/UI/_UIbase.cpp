/*
 * Window.cpp
 *
 *  Created on: May 24, 2022
 *      Author: Kai Yan
 */

#include "_UIbase.h"

namespace kai
{

	_UIbase::_UIbase()
	{
	}

	_UIbase::~_UIbase()
	{
	}

	bool _UIbase::init(void *pKiss)
	{
		IF_F(!this->_ModuleBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		return true;
	}

	bool _UIbase::link(void)
	{
		IF_F(!this->_ModuleBase::link());
		Kiss *pK = (Kiss *)m_pKiss;

		vector<string> vB;
		pK->a("vBASE", &vB);
		for (string p : vB)
		{
			BASE *pB = (BASE *)(pK->findModule(p));
			IF_CONT(!pB);

			m_vpB.push_back(pB);
		}

		return true;
	}

	bool _UIbase::start(void)
	{
		NULL_F(m_pT);
		return m_pT->start(getUpdate, this);
	}

	void _UIbase::update(void)
	{
		while (m_pT->bAlive())
		{
			m_pT->autoFPSfrom();

			m_pT->autoFPSto();
		}
	}
}
