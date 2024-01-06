/*
 * _ADIO_EBYTE.cpp
 *
 *  Created on: Jan 7, 2024
 *      Author: yankai
 */

#include "_ADIO_EBYTE.h"

namespace kai
{
	_ADIO_EBYTE::_ADIO_EBYTE()
	{
		m_pMB = NULL;
	}

	_ADIO_EBYTE::~_ADIO_EBYTE()
	{
		close();
	}

	bool _ADIO_EBYTE::init(void *pKiss)
	{
		IF_F(!this->_ADIObase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		//		pK->v("addr", &m_addr);

		return true;
	}

	bool _ADIO_EBYTE::link(void)
	{
		IF_F(!this->_ADIObase::link());
		Kiss *pK = (Kiss *)m_pKiss;

		string n;
		n = "";
		pK->v("_Modbus", &n);
		m_pMB = (_Modbus *)(pK->getInst(n));

		return true;
	}

	bool _ADIO_EBYTE::open(void)
	{
		return true;
	}

	void _ADIO_EBYTE::close(void)
	{
	}

	int _ADIO_EBYTE::check(void)
	{
		NULL__(m_pMB, -1);

		return this->_ADIObase::check();
	}

	bool _ADIO_EBYTE::start(void)
	{
		NULL_F(m_pT);
		return m_pT->start(getUpdate, this);
	}

	void _ADIO_EBYTE::update(void)
	{
		while (m_pT->bRun())
		{
			if (!bOpen())
			{
				if (!open())
				{
					m_pT->sleepT(SEC_2_USEC);
					continue;
				}
			}

			m_pT->autoFPSfrom();

			updateW();
			updateR();

			m_pT->autoFPSto();
		}
	}

	void _ADIO_EBYTE::updateW(void)
	{
	}

	void _ADIO_EBYTE::updateR(void)
	{
	}

	void _ADIO_EBYTE::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_ADIObase::console(pConsole);

		//		((_Console *)pConsole)->addMsg();
	}

}
