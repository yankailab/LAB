#include "_PWMio.h"

namespace kai
{
	_PWMio::_PWMio()
	{
		m_nCr = 8;
		m_nCw = 8;
	}

	_PWMio::~_PWMio()
	{
	}

	int _PWMio::init(void *pKiss)
	{
		CHECK_(this->_ProtocolBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("nCr", &m_nCr);
		pK->v("nCw", &m_nCw);

		vector<int> vPWM;
		pK->a("vPWM", &vPWM);
		for (int i = 0; i < vPWM.size(); i++)
		{
			if (i >= m_nCw)
				break;
			m_pCw[i].set(vPWM[i]);
		}

		return OK_OK;
	}

	int _PWMio::start(void)
	{
		NULL__(m_pT, OK_ERR_NULLPTR);
		NULL__(m_pTr, OK_ERR_NULLPTR);
		CHECK_(m_pT->start(getUpdateW, this));
		return m_pTr->start(getUpdateR, this);
	}

	void _PWMio::updateW(void)
	{
		while (m_pT->bAlive())
		{
			m_pT->autoFPS();

			send();

		}
	}

	void _PWMio::send(void)
	{
		IF_(check() != OK_OK);

		uint8_t pB[256];
		pB[0] = PB_BEGIN;  // start mark
		pB[1] = 0;		   // cmd
		pB[2] = m_nCr * 2; // payload len

		int j = 3;
		for (int i = 0; i < m_nCw; i++)
		{
			uint16_t v = m_pCw[i].raw();
			pB[j++] = ((uint8_t)(v & 0xFF));
			pB[j++] = ((uint8_t)((v >> 8) & 0xFF));
		}

		m_pIO->write(pB, PB_N_HDR + m_nCw * 2);
	}

	void _PWMio::updateR(void)
	{
		PROTOCOL_CMD rCMD;

		while (m_pTr->bAlive())
		{
			IF_CONT(!readCMD(&rCMD));

			handleCMD(rCMD);
			rCMD.clear();
			m_nCMDrecv++;
		}
	}

	void _PWMio::handleCMD(const PROTOCOL_CMD& cmd)
	{
		switch (cmd.m_cmd)
		{
		case ARDU_CMD_HB:
			for (int i = 0; i < m_nCr; i++)
			{
				int iB = i * 2;
				if (iB >= cmd.m_nPayload)
					break;

				uint16_t v = *((uint16_t *)(&cmd.m_pB[PB_N_HDR + iB]));
				m_pCr[i].set(v);
			}
			break;
		default:
			break;
		}
	}

	void _PWMio::set(int iChan, uint16_t v)
	{
		IF_(iChan > m_nCw);
		m_pCw[iChan].set(v);
	}

	uint16_t _PWMio::getRaw(int iChan)
	{
		IF__(iChan > m_nCr, 0);
		return m_pCr[iChan].raw();
	}

	void _PWMio::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_ProtocolBase::console(pConsole);

		_Console *pC = (_Console *)pConsole;
		if (!m_pIO->bOpen())
		{
			pC->addMsg("Not Connected", 1);
			return;
		}

		string m = "ChanR: ";
		for (int i = 0; i < m_nCr; i++)
			m += i2str(m_pCr[i].raw()) + " | ";

		m += "ChanW: ";
		for (int i = 0; i < m_nCw; i++)
			m += i2str(m_pCw[i].raw()) + " | ";

		m += " nCMD=" + i2str(m_nCMDrecv);

		pC->addMsg(m, 1);
	}

}
