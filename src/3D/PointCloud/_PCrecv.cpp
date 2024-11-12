/*
 * _PCrecv.cpp
 *
 *  Created on: Oct 8, 2020
 *      Author: yankai
 */

#include "_PCrecv.h"

namespace kai
{

	_PCrecv::_PCrecv()
	{
		m_pIO = nullptr;
		m_nCMDrecv = 0;

		m_nRead = 0;
		m_iRead = 0;
	}

	_PCrecv::~_PCrecv()
	{
	}

	int _PCrecv::init(void *pKiss)
	{
		CHECK_(_PCstream::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		return true;
	}

    int _PCrecv::link(void)
    {
        CHECK_(this->_PCstream::link());
		Kiss *pK = (Kiss *)m_pKiss;
		string n;

		n = "";
		pK->v("_IObase", &n);
		m_pIO = (_IObase *)(pK->findModule(n));
		NULL__(m_pIO, OK_ERR_NOT_FOUND);

        return OK_OK;
    }

	int _PCrecv::start(void)
	{
		NULL__(m_pT, OK_ERR_NULLPTR);
		return m_pT->start(getUpdate, this);
	}

	int _PCrecv::check(void)
	{
		NULL__(m_pIO, OK_ERR_NULLPTR);
		IF__(!m_pIO->bOpen(), OK_ERR_NOT_READY);

		return this->_PCstream::check();
	}

	void _PCrecv::update(void)
	{
		PROTOCOL_CMD rCMD;

		while (m_pT->bAlive())
		{
			IF_CONT(!readCMD(&rCMD));

			handleCMD(rCMD);
			rCMD.clear();
			m_nCMDrecv++;
		}
	}

	bool _PCrecv::readCMD(PROTOCOL_CMD* pCmd)
	{
		IF_F(check() != OK_OK);
		NULL_F(pCmd);

		if (m_nRead == 0)
		{
			m_nRead = m_pIO->read(m_pBuf, PB_N_BUF);
			IF_F(m_nRead <= 0);
			m_iRead = 0;
		}

		while (m_iRead < m_nRead)
		{
			bool r = pCmd->input(m_pBuf[m_iRead++]);
			if (m_iRead == m_nRead)
			{
				m_iRead = 0;
				m_nRead = 0;
			}

			IF__(r, true);
		}

		return false;
	}

	void _PCrecv::handleCMD(const PROTOCOL_CMD& cmd)
	{
		switch (cmd.m_pB[1])
		{
		case PC_STREAM:
		{
			decodeStream(cmd);
			break;
		}
		case PC_FRAME_END:
		{
			break;
		}
		default:
			break;
		}
	}

	void _PCrecv::decodeStream(const PROTOCOL_CMD &cmd)
	{
		const double PC_SCALE_INV = 0.001;
		int16_t x, y, z;

		for (int i = PC_N_HDR; i < cmd.m_nPayload + PC_N_HDR; i += 12)
		{
			IF_(i + 12 > PC_N_BUF);

			x = unpack_int16(&cmd.m_pB[i], false);
			y = unpack_int16(&cmd.m_pB[i + 2], false);
			z = unpack_int16(&cmd.m_pB[i + 4], false);
			Eigen::Vector3d vP(((double)x) * PC_SCALE_INV,
							   ((double)y) * PC_SCALE_INV,
							   ((double)z) * PC_SCALE_INV);

			x = unpack_int16(&cmd.m_pB[i + 6], false);
			y = unpack_int16(&cmd.m_pB[i + 8], false);
			z = unpack_int16(&cmd.m_pB[i + 10], false);
			Eigen::Vector3f vC(((float)x) * PC_SCALE_INV,
							   ((float)y) * PC_SCALE_INV,
							   ((float)z) * PC_SCALE_INV);

			add(vP, vC, m_pT->getTfrom());
		}
	}

}
