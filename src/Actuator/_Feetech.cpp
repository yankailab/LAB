/*
 *  Created on: Jan 6, 2024
 *      Author: yankai
 */
#include "_Feetech.h"

namespace kai
{

	_Feetech::_Feetech()
	{
		m_port = "";
		m_baud = 115200;
		m_bOpen = false;
		m_ID = 1;
		m_pA = nullptr;

		m_ieReadStatus.init(50000);
	}

	_Feetech::~_Feetech()
	{
	}

	int _Feetech::init(void *pKiss)
	{
		CHECK_(this->_ActuatorBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("port", &m_port);
		pK->v("baud", &m_baud);
		pK->v("iID", &m_ID);
		pK->v("tIntReadStatus", &m_ieReadStatus.m_tInterval);

		m_pA = &m_vAxis[0];

		return OK_OK;
	}

	int _Feetech::link(void)
	{
		CHECK_(this->_ActuatorBase::link());
		Kiss *pK = (Kiss *)m_pKiss;

		return OK_OK;
	}

	bool _Feetech::open(void)
	{
		IF_T(m_bOpen);
		IF_F(!m_servo.begin(m_baud, m_port.c_str()));

		m_servo.EnableTorque(1, 1);

		m_bOpen = true;
		return true;
	}

	void _Feetech::close(void)
	{
		IF_(!m_bOpen);
		m_servo.end();
		m_bOpen = false;
	}

	int _Feetech::start(void)
	{
		NULL__(m_pT, OK_ERR_NULLPTR);
		return m_pT->start(getUpdate, this);
	}

	int _Feetech::check(void)
	{
		IF__(!m_bOpen, OK_ERR_NOT_READY);
		NULL__(m_pA, OK_ERR_NULLPTR);

		return this->_ActuatorBase::check();
	}

	void _Feetech::update(void)
	{
		while (m_pT->bAlive())
		{
			m_pT->autoFPSfrom();

			if (!m_bOpen)
			{
				if (!open())
				{
					LOG_E("Cannot open");
					m_pT->sleepT(SEC_2_USEC);
					continue;
				}
			}

			updateMove();

			m_pT->autoFPSto();
		}
	}

	void _Feetech::updateMove(void)
	{
		IF_(check() != OK_OK);

		IF_(!setPos());
		// IF_(!setSpeed());
		// IF_(!setAccel());

		// if (m_bf.b(ACT_BF_STOP, true))
		// {
		// 	readStatus()
		// }
	}

	bool _Feetech::setID(uint16_t iSlave)
	{
		IF_F(check() != OK_OK);;

		return true;
	}

	bool _Feetech::setBaudrate(uint32_t baudrate)
	{
		IF_F(check() != OK_OK);;

		return true;
	}

	bool _Feetech::saveData(void)
	{
		IF_F(check() != OK_OK);;

		return true;
	}

	bool _Feetech::setPos(void)
	{
		IF_F(check() != OK_OK);;

		m_servo.WritePosEx(m_ID,
						   m_pA->m_p.m_vTarget,
						   m_pA->m_s.m_vTarget,
						   m_pA->m_a.m_vTarget);

		return true;
	}

	bool _Feetech::setSpeed(void)
	{
		IF_F(check() != OK_OK);;

		uint16_t s = m_pA->m_s.m_vTarget;

		return true;
	}

	bool _Feetech::setAccel(void)
	{
		IF_F(check() != OK_OK);;

		uint16_t a = m_pA->m_a.m_vTarget;

		return true;
	}

	bool _Feetech::readStatus(void)
	{
		IF_F(check() != OK_OK);;
		IF_T(!m_ieReadStatus.update(m_pT->getTfrom()));

		// uint16_t pB[2];
		// int r = m_pIO->readRegisters(m_ID, m_addr.m_readStat, 2, pB);
		// IF_F(r != 2);
		// m_pT->sleepT(m_cmdInt);

		// //	int p = MAKE32(pB[0], pB[1]);
		// int16_t p = pB[1];
		// m_pA->m_p.m_v = p;

		return true;
	}

	void _Feetech::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_ActuatorBase::console(pConsole);

		_Console *pC = ((_Console *)pConsole);
		string str = "";

		pC->addMsg(str);
	}

}
