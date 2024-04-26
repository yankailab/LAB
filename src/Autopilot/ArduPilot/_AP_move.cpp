#include "../ArduPilot/_AP_move.h"

namespace kai
{

	_AP_move::_AP_move()
	{
		m_pAP = NULL;
	}

	_AP_move::~_AP_move()
	{
	}

	bool _AP_move::init(void *pKiss)
	{
		IF_F(!this->_ModuleBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;
    	

		return true;
	}

	bool _AP_move::link(void)
	{
		IF_F(!this->_ModuleBase::link());

		Kiss *pK = (Kiss *)m_pKiss;
		string n;

		n = "";
		pK->v("_AP_base", &n);
		m_pAP = (_AP_base *)(pK->getInst(n));
		IF_Fl(!m_pAP, n + ": not found");

		return true;
	}

	bool _AP_move::start(void)
	{
		NULL_F(m_pT);
		return m_pT->start(getUpdate, this);
	}

	int _AP_move::check(void)
	{
		NULL__(m_pAP, -1);
		NULL__(m_pAP->m_pMav, -1);

		return this->_ModuleBase::check();
	}

	void _AP_move::update(void)
	{
		while (m_pT->bAlive())
		{
			m_pT->autoFPSfrom();

			m_pT->autoFPSto();
		}
	}

	void _AP_move::stop(void)
	{
		IF_(check() < 0);

		mavlink_set_position_target_local_ned_t spt;
		spt.coordinate_frame = MAV_FRAME_BODY_OFFSET_NED;
		spt.x = 0.0;
		spt.y = 0.0;
		spt.z = 0.0;
		spt.vx = 0;
		spt.vy = 0;
		spt.vz = 0;
		spt.yaw = 0.0;
		spt.yaw_rate = 0.0;
		spt.type_mask = 0b0000010111000111;
		m_pAP->m_pMav->setPositionTargetLocalNED(spt);
	}

	void _AP_move::setHdg(float y, float r, bool bYaw, bool bYawRate, uint8_t frame)
	{
		IF_(check() < 0);

		mavlink_set_position_target_local_ned_t spt;
		spt.coordinate_frame = frame;
		spt.x = 0.0;
		spt.y = 0.0;
		spt.z = 0.0;
		spt.vx = 0;
		spt.vy = 0;
		spt.vz = 0;
		spt.yaw = y;
		spt.yaw_rate = r;
		spt.type_mask = 0b0000000111000111; // AVP
		if (!bYaw)
			spt.type_mask |= IGN_YAW;
		if (!bYawRate)
			spt.type_mask |= IGN_YAW_RATE;

		m_pAP->m_pMav->setPositionTargetLocalNED(spt);
	}

	void _AP_move::setVlocal(const vFloat4 &vSpd, bool bYaw, bool bYawRate, uint8_t frame)
	{
		IF_(check() < 0);

		mavlink_set_position_target_local_ned_t spt;
		spt.coordinate_frame = frame;
		spt.vx = vSpd.x; // forward
		spt.vy = vSpd.y; // right
		spt.vz = vSpd.z; // down
		spt.yaw = 0.0;
		spt.yaw_rate = vSpd.w;
		spt.type_mask = 0b0000000111000111; // AVP
		if (!bYaw)
			spt.type_mask |= IGN_YAW;
		if (!bYawRate)
			spt.type_mask |= IGN_YAW_RATE;

		m_pAP->m_pMav->setPositionTargetLocalNED(spt);
	}

	void _AP_move::setPlocal(const vDouble4 &vP, bool bYaw, bool bYawRate, uint8_t frame)
	{
		IF_(check() < 0);

		mavlink_set_position_target_local_ned_t spt;
		spt.coordinate_frame = frame;
		spt.x = vP.x; // forward
		spt.y = vP.y; // right
		spt.z = vP.z; // down
		spt.yaw = 0.0;
		spt.yaw_rate = vP.w;
		spt.type_mask = 0b0000000111111000; // AVP
		if (!bYaw)
			spt.type_mask |= IGN_YAW;
		if (!bYawRate)
			spt.type_mask |= IGN_YAW_RATE;

		m_pAP->m_pMav->setPositionTargetLocalNED(spt);
	}

	void _AP_move::setPglobal(const vDouble4 &vP, bool bYaw, bool bYawRate, uint8_t frame)
	{
		IF_(check() < 0);

		mavlink_set_position_target_global_int_t spt;
		spt.coordinate_frame = frame;
		spt.lat_int = vP.x * 1e7;
		spt.lon_int = vP.y * 1e7;
		spt.alt = vP.z;
		spt.vx = 0.0;
		spt.vy = 0.0;
		spt.vz = 0.0;
		spt.yaw = vP.w;
		spt.type_mask = 0b0000000111111000; // AVP
		if (!bYaw)
			spt.type_mask |= IGN_YAW;
		if (!bYawRate)
			spt.type_mask |= IGN_YAW_RATE;

		m_pAP->m_pMav->setPositionTargetGlobalINT(spt);
	}

	void _AP_move::doReposition(const vDouble4 &vP,
								float speed,
								float radius,
								uint8_t frame)
	{
		IF_(check() < 0);

		mavlink_command_int_t D;
		D.frame = frame;
		D.command = MAV_CMD_DO_REPOSITION;
		D.param1 = speed;
		D.param2 = 0;
		D.param3 = radius;
		D.param4 = vP.w;
		D.x = (int32_t)(vP.x * 1e7);
		D.y = (int32_t)(vP.y * 1e7);
		D.z = vP.z;

		m_pAP->m_pMav->cmdInt(D);
	}

	void _AP_move::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_ModuleBase::console(pConsole);

		_Console *pC = (_Console *)pConsole;
		//		pC->addMsg("Local NED:");
	}

}
