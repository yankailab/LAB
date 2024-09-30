/*
 *  Created on: May 16, 2019
 *      Author: yankai
 */
#include "_OrientalMotor.h"

namespace kai
{

	_OrientalMotor::_OrientalMotor()
	{
		m_pMB = nullptr;
		m_iSlave = 1;
		m_iData = 0;

		m_ieCheckAlarm.init(100000);
		m_ieSendCMD.init(50000);
		m_ieReadStatus.init(50000);
	}

	_OrientalMotor::~_OrientalMotor()
	{
	}

	int _OrientalMotor::init(void *pKiss)
	{
		CHECK_(this->_ActuatorBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("iSlave", &m_iSlave);
		pK->v("iData", &m_iData);

		pK->v("tIntCheckAlarm", &m_ieCheckAlarm.m_tInterval);
		pK->v("tIntSendCMD", &m_ieSendCMD.m_tInterval);
		pK->v("tIntReadStatus", &m_ieReadStatus.m_tInterval);

		return OK_OK;
	}

	int _OrientalMotor::link(void)
	{
		CHECK_(this->_ActuatorBase::link());
		Kiss *pK = (Kiss *)m_pKiss;

		string n;

		n = "";
		IF__(!pK->v("_Modbus", &n), OK_ERR_NOT_FOUND);
		m_pMB = (_Modbus *)(pK->findModule(n));
		NULL__(m_pMB, OK_ERR_NOT_FOUND);

		return OK_OK;
	}

	int _OrientalMotor::start(void)
	{
		NULL__(m_pT, OK_ERR_NULLPTR);
		return m_pT->start(getUpdate, this);
	}

	void _OrientalMotor::update(void)
	{
		while (m_pT->bAlive())
		{
			m_pT->autoFPSfrom();

			// if (m_bFeedback)
			// {
				checkAlarm();
				readStatus();
//			}

			//		if(!bCmdTimeout())
			//        {
//			if (m_lastCmdType == actCmd_pos)
				updatePos();
//			else if (m_lastCmdType == actCmd_spd)
				updateSpeed();
			//        }

			m_pT->autoFPSto();
		}
	}

	int _OrientalMotor::check(void)
	{
		NULL__(m_pMB, -1);
		IF__(!m_pMB->bOpen(), -1);

		return this->_ActuatorBase::check();
	}

	void _OrientalMotor::checkAlarm(void)
	{
		IF_(check() != OK_OK);
		IF_(!m_ieCheckAlarm.update(m_pT->getTfrom()));

		uint16_t pB[2];
		pB[0] = 1 << 7;
		pB[1] = 0;
		m_pMB->writeRegisters(m_iSlave, 125, 1, pB);
	}

	void _OrientalMotor::updatePos(void)
	{
		IF_(check() != OK_OK);
		IF_(!m_ieSendCMD.update(m_pT->getTfrom()));

		ACTUATOR_CHAN* pChan = getChan();
		NULL_(pChan);

		int32_t step = pChan->pos()->getTarget();
		int32_t speed = pChan->speed()->getTarget();
		int32_t accel = pChan->accel()->getTarget();
		int32_t brake = pChan->brake()->getTarget();
		int32_t current = pChan->current()->getTarget();

		//create the command
		uint16_t pB[18];
		//88
		pB[0] = 0;
		pB[1] = m_iData;
		pB[2] = 0;
		pB[3] = 1;
		//92
		pB[4] = HIGH16(step);
		pB[5] = LOW16(step);
		pB[6] = HIGH16(speed);
		pB[7] = LOW16(speed);

		//96
		pB[8] = HIGH16(accel);
		pB[9] = LOW16(accel);
		pB[10] = HIGH16(brake);
		pB[11] = LOW16(brake);
		pB[12] = HIGH16(current);
		pB[13] = LOW16(current);
		pB[14] = 0;
		pB[15] = 1;
		pB[16] = 0;
		pB[17] = 0;

		if (m_pMB->writeRegisters(m_iSlave, 88, 18, pB) != 18)
			m_ieSendCMD.reset();
	}

	void _OrientalMotor::updateSpeed(void)
	{
		IF_(check() != OK_OK);
		IF_(!m_ieSendCMD.update(m_pT->getTfrom()));

		ACTUATOR_CHAN* pChan = getChan();
		NULL_(pChan);

		int32_t step = 0;
		uint8_t dMode = 1;
		int32_t speed = pChan->speed()->getTarget();

		vFloat2 vRange = pChan->pos()->getRange();
		if (speed > 0)
			step = vRange.y;
		else if (speed < 0)
			step = vRange.x;
		else
			dMode = 3;

		int32_t accel = pChan->accel()->getTarget();
		int32_t brake = pChan->brake()->getTarget();
		int32_t current = pChan->current()->getTarget();

		//create the command
		uint16_t pB[18];
		//88
		pB[0] = 0;
		pB[1] = m_iData;
		pB[2] = 0;
		pB[3] = dMode;
		//92
		pB[4] = HIGH16(step);
		pB[5] = LOW16(step);
		pB[6] = HIGH16(speed);
		pB[7] = LOW16(speed);

		//96
		pB[8] = HIGH16(accel);
		pB[9] = LOW16(accel);
		pB[10] = HIGH16(brake);
		pB[11] = LOW16(brake);
		pB[12] = HIGH16(current);
		pB[13] = LOW16(current);
		pB[14] = 0;
		pB[15] = 1;
		pB[16] = 0;
		pB[17] = 0;

		if (m_pMB->writeRegisters(m_iSlave, 88, 18, pB) != 18)
			m_ieSendCMD.reset();
	}

	void _OrientalMotor::readStatus(void)
	{
		IF_(check() != OK_OK);
		IF_(!m_ieReadStatus.update(m_pT->getTfrom()));

		uint16_t pB[18];
		int nR = 6;
		int r = m_pMB->readRegisters(m_iSlave, 204, nR, pB);
		IF_(r != 6);

		int32_t p = MAKE32(pB[0], pB[1]);
		int32_t s = MAKE32(pB[4], pB[5]);

		ACTUATOR_CHAN* pChan = getChan();
		NULL_(pChan);
		pChan->pos()->set(p);
		pChan->speed()->set(s);

		LOG_I("step: " + f2str(pChan->pos()->get())
		 + ", speed: " + f2str(pChan->speed()->get()));
	}

}
