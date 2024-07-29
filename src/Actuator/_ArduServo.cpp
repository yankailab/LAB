#include "_ArduServo.h"

namespace kai
{
	_ArduServo::_ArduServo()
	{
        m_pTr = nullptr;
		m_pIO = nullptr;
		m_recvMsg.init();
		m_nCMDrecv = 0;
	}

	_ArduServo::~_ArduServo()
	{
        DEL(m_pTr);
	}

	int _ArduServo::init(void *pKiss)
	{
		CHECK_(this->_ActuatorBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;
		
		Kiss *pKt = pK->child("threadR");
        if (pKt->empty())
        {
            LOG_I("ThreadR not found");
        }
        else
        {
            m_pTr = new _Thread();
       		CHECK_d_l_(m_pTr->init(pKt), DEL(m_pTr), "ThreadR init failed");
        }

		return OK_OK;
	}

    int _ArduServo::link(void)
    {
        CHECK_(this->_ActuatorBase::link());
        CHECK_(m_pTr->link());

		Kiss *pK = (Kiss *)m_pKiss;
		string n;
		n = "";
		IF__(!pK->v("_IObase", &n), OK_ERR_NOT_FOUND);
		m_pIO = (_IObase *)(pK->findModule(n));
		NULL__(m_pIO, OK_ERR_NOT_FOUND);

        return OK_OK;
    }

	int _ArduServo::start(void)
	{
		NULL__(m_pT, OK_ERR_NULLPTR);
		NULL__(m_pTr, OK_ERR_NULLPTR);
		CHECK_(m_pT->start(getUpdate, this));
		return m_pTr->start(getUpdateR, this);
	}

	void _ArduServo::update(void)
	{
		while (m_pT->bAlive())
		{
			m_pT->autoFPSfrom();

			sendCMD();

			m_pT->autoFPSto();
		}
	}

	void _ArduServo::sendCMD(void)
	{
		NULL_(m_pIO);
		IF_(!m_pIO->bOpen());

		int i;
		uint16_t pChan[16];
		int nC = m_vAxis.size();
		for (i = 0; i < nC; i++)
		{
			ACTUATOR_AXIS *pA = &m_vAxis[i];
			pChan[i] = 1500;
			if (pA->m_p.bInRange(pA->m_p.m_vTarget))
				pChan[i] = pA->m_p.m_vTarget;

			pA->m_p.m_v = pA->m_p.m_vTarget;
		}

		m_pB[0] = ARDUSV_BEGIN;
		m_pB[1] = ARDU_CMD_PWM;
		m_pB[2] = nC * 2;
		int j = 3;
		for (int i = 0; i < nC; i++)
		{
			uint16_t v = pChan[i];
			m_pB[j++] = ((uint8_t)(v & 0xFF));
			m_pB[j++] = ((uint8_t)((v >> 8) & 0xFF));
		}

		m_pIO->write(m_pB, ARDUSV_N_HEADER + nC * 2);
	}

	void _ArduServo::updateR(void)
	{
		while (m_pTr->bAlive())
		{
			m_pTr->autoFPSfrom();

			while (readCMD())
			{
				handleCMD();
				m_recvMsg.init();
				m_nCMDrecv++;
			}

			m_pTr->autoFPSto();
		}
	}

	bool _ArduServo::readCMD(void)
	{
		NULL_F(m_pIO);
		IF_F(!m_pIO->bOpen());

		uint8_t inByte;
		int byteRead;

		while ((byteRead = m_pIO->read(&inByte, 1)) > 0)
		{
			if (m_recvMsg.m_cmd != 0)
			{
				m_recvMsg.m_pB[m_recvMsg.m_iB] = inByte;
				m_recvMsg.m_iB++;

				if (m_recvMsg.m_iB == 3)
				{
					m_recvMsg.m_nPayload = inByte;
				}
				else if (m_recvMsg.m_iB == m_recvMsg.m_nPayload + ARDUSV_N_HEADER)
				{
					return true;
				}
			}
			else if (inByte == ARDUSV_BEGIN)
			{
				m_recvMsg.m_cmd = inByte;
				m_recvMsg.m_pB[0] = inByte;
				m_recvMsg.m_iB = 1;
				m_recvMsg.m_nPayload = 0;
			}
		}

		return false;
	}

	void _ArduServo::handleCMD(void)
	{
		uint16_t pwm1;
		uint16_t pwm2;

		switch (m_recvMsg.m_pB[1])
		{
		case ARDU_CMD_STATUS:
			pwm1 = (uint16_t)unpack_int16(&m_recvMsg.m_pB[3], false);
			pwm2 = (uint16_t)unpack_int16(&m_recvMsg.m_pB[5], false);

			LOG_I("pwm1=" + i2str(pwm1) + ", pwm2=" + i2str(pwm2));
			break;
		default:
			break;
		}

		m_recvMsg.init();
	}

	void _ArduServo::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_ActuatorBase::console(pConsole);

		_Console *pC = (_Console *)pConsole;
		if (!m_pIO->bOpen())
		{
			pC->addMsg("Not Connected", 1);
			return;
		}

		pC->addMsg("nCMD = " + i2str(m_nCMDrecv), 1);
	}

}
