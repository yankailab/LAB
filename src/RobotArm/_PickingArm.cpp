/*
 * _PickingArm.cpp
 *
 *  Created on: June 30, 2020
 *      Author: yankai
 */

#include "_PickingArm.h"

namespace kai
{

	_PickingArm::_PickingArm()
	{
		m_pA = nullptr;
		m_pG = nullptr;
		m_pD = nullptr;
		m_pU = nullptr;

		m_baseAngle = 0.0;
		m_vP.set(0.5, 0.5, 0.0);
		m_vPtarget.set(0.5, 0.5, 0.0);
		m_vZrange.set(0.0, 0.06, 0.15, 30.0);
		m_zSpeed = 0.2;
		m_zrK = 1.0;

		m_pXpid = NULL;
		m_pYpid = NULL;
		m_pZpid = NULL;

		m_vS.set(0.5);
		m_vR.set(0.5);

		m_vPrecover.clear();
		m_vPdeliver.set(-200, 300.0, 0.0);
		m_vPdescend.set(-200, 300.0, -100.0);

		m_oTstamp = 0;
		m_oTimeout = SEC_2_USEC;
	}

	_PickingArm::~_PickingArm()
	{
	}

	int _PickingArm::init(void *pKiss)
	{
		CHECK_(this->_ModuleBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("vPtarget", &m_vPtarget);
		pK->v("vZrange", &m_vZrange);
		pK->v("zSpeed", &m_zSpeed);
		pK->v("zrK", &m_zrK);
		pK->v("vPrecover", &m_vPrecover);
		pK->v("vPdeliver", &m_vPdeliver);
		pK->v("vPdescend", &m_vPdescend);
		pK->v("oTimeout", &m_oTimeout);

		Kiss *pClass = pK->child("class");
		NULL__(pClass, OK_ERR_NOT_FOUND);

		int i = 0;
		while (1)
		{
			Kiss *pC = pClass->child(i++);
			if (pC->empty())
				break;

			PICKINGARM_CLASS pc;
			pc.init();
			pC->v("iClass", &pc.m_iClass);

			m_vClass.push_back(pc);
		}

		// NULL__(m_pSC, OK_ERR_NOT_FOUND);
		// m_iState.EXTERNAL = m_pSC->getStateIdxByName("EXTERNAL");
		// m_iState.RECOVER = m_pSC->getStateIdxByName("RECOVER");
		// m_iState.FOLLOW = m_pSC->getStateIdxByName("FOLLOW");
		// m_iState.ASCEND = m_pSC->getStateIdxByName("ASCEND");
		// m_iState.DELIVER = m_pSC->getStateIdxByName("DELIVER");
		// m_iState.DESCEND = m_pSC->getStateIdxByName("DESCEND");
		// m_iState.DROP = m_pSC->getStateIdxByName("DROP");
		// IF__(!m_iState.bValid(), OK_ERR_INVALID_VALUE);

		string n;

		n = "";
		pK->v("_ActuatorBase", &n);
		m_pA = (_ActuatorBase *)(pK->findModule(n));
		NULL__(m_pA, OK_ERR_NOT_FOUND);

		n = "";
		pK->v("_StepperGripper", &n);
		m_pG = (_StepperGripper *)pK->findModule(n);
		NULL__(m_pG, OK_ERR_NOT_FOUND);

		n = "";
		pK->v("_Universe", &n);
		m_pU = (_Universe *)(pK->findModule(n));
		NULL__(m_pU, OK_ERR_NOT_FOUND);

		n = "";
		pK->v("_DistSensorBase", &n);
		m_pD = (_DistSensorBase *)(pK->findModule(n));
		NULL__(m_pD, OK_ERR_NOT_FOUND);

		n = "";
		pK->v("PIDx", &n);
		m_pXpid = (PID *)(pK->findModule(n));
		NULL__(m_pXpid, OK_ERR_NOT_FOUND);

		n = "";
		pK->v("PIDy", &n);
		m_pYpid = (PID *)(pK->findModule(n));
		NULL__(m_pYpid, OK_ERR_NOT_FOUND);

		n = "";
		pK->v("PIDz", &n);
		m_pZpid = (PID *)(pK->findModule(n));
		NULL__(m_pZpid, OK_ERR_NOT_FOUND);

		return OK_OK;
	}

	int _PickingArm::start(void)
	{
		NULL__(m_pT, OK_ERR_NULLPTR);
		return m_pT->start(getUpdate, this);
	}

	int _PickingArm::check(void)
	{
		NULL__(m_pA, OK_ERR_NULLPTR);
		NULL__(m_pG, OK_ERR_NULLPTR);
		NULL__(m_pD, OK_ERR_NULLPTR);
		NULL__(m_pU, OK_ERR_NULLPTR);
		NULL__(m_pXpid, OK_ERR_NULLPTR);
		NULL__(m_pYpid, OK_ERR_NULLPTR);
		NULL__(m_pZpid, OK_ERR_NULLPTR);

		return this->_ModuleBase::check();
	}

	void _PickingArm::update(void)
	{
		while (m_pT->bAlive())
		{
			m_pT->autoFPSfrom();

			updateArm();

			m_pT->autoFPSto();
		}
	}

	void _PickingArm::updateArm(void)
	{
		IF_(check() != OK_OK);

		// int iM = m_pSC->getStateIdx();
		// bool bTransit = false;

		// if (iM == m_iState.EXTERNAL)
		// {
		// 	external();
		// }
		// else if (iM == m_iState.RECOVER)
		// {
		// 	bTransit = recover();
		// }
		// else if (iM == m_iState.FOLLOW)
		// {
		// 	bTransit = follow();
		// }
		// else if (iM == m_iState.ASCEND)
		// {
		// 	bTransit = ascend();
		// }
		// else if (iM == m_iState.DELIVER)
		// {
		// 	bTransit = deliver();
		// }
		// else if (iM == m_iState.DESCEND)
		// {
		// 	bTransit = descend();
		// }
		// else if (iM == m_iState.DROP)
		// {
		// 	bTransit = drop();
		// }
		// else
		// {
		// 	hold();
		// }

		// if (bTransit)
		// 	m_pSC->transit();
	}

	void _PickingArm::hold(void)
	{
		m_pA->setStarget(0, 0.5);
		m_pA->setStarget(1, 0.5);
		m_pA->setStarget(2, 0.5);
		m_pA->setStarget(6, 0.5);
		m_pA->setStarget(7, 0.5);
		m_pA->setStarget(8, 0.5);
	}

	void _PickingArm::external(void)
	{
		m_pA->setStarget(0, m_vS.x);
		m_pA->setStarget(1, m_vS.y);
		m_pA->setStarget(2, m_vS.z);
		m_pA->setStarget(6, m_vR.x);
		m_pA->setStarget(7, m_vR.y);
		m_pA->setStarget(8, m_vR.z);
	}

	bool _PickingArm::recover(void)
	{
		m_pA->atomicFrom();
		m_pA->setPtarget(0, m_vPrecover.x);
		m_pA->setPtarget(1, m_vPrecover.y);
		m_pA->setPtarget(2, m_vPrecover.z);
		m_pA->atomicTo();

		IF_F(!m_pA->bComplete(0));
		IF_F(!m_pA->bComplete(1));
		IF_F(!m_pA->bComplete(2));

		return true;
	}

	bool _PickingArm::follow(void)
	{
		m_vP.z = m_pD->d(0);

		if (m_vP.z < m_vZrange.x)
		{
			//invalid reading of Z, go back to origin
			recover();
			return false;
		}
		else if (m_vP.z < m_vZrange.y)
		{
			//stop and gripper closed
			hold();
//			IF__(!m_pG->bGrip());
//			m_pG->grip(false);
			return false;
		}
		else if (m_vP.z < m_vZrange.z)
		{
			//gripper start to close
//			m_pG->grip(false);
			return false;
		}
		else if (m_vP.z < m_vZrange.w)
		{
			//keep on getting closer even no target is seen any more
			speed(vFloat3(0.5, 0.5, 0.5 + m_zSpeed));
			return false;
		}

		//look for target
		_Object *tO = findTarget();

		if (!tO)
		{
			//no target is seen, ascend the arm
			//TODO: set speed
			recover();
			return false;
		}

		m_baseAngle = -m_pA->getP(3);
		float rad = m_baseAngle * DEG_2_RAD;
		float s = sin(rad);
		float c = cos(rad);

		vFloat3 vP = tO->getPos();
		float x = vP.x - m_vPtarget.x;
		float y = vP.y - m_vPtarget.y;
		float r = sqrt(x * x + y * y);
		m_vP.x = x * c - y * s + m_vPtarget.x;
		m_vP.y = x * s + y * c + m_vPtarget.y;

		m_vS.y = 0.5 + m_pXpid->update(m_vP.x, m_vPtarget.x, m_pT->getTfrom());
		m_vS.x = 0.5 + m_pYpid->update(m_vP.y, m_vPtarget.y, m_pT->getTfrom());
		m_vS.z = 0.5 + m_pZpid->update(m_vP.z, m_vPtarget.z, m_pT->getTfrom()) * constrain(1.0 - r * m_zrK, 0.0, 1.0);
		speed(m_vS);

		return false;
	}

	_Object *_PickingArm::findTarget(void)
	{
		_Object *pO;
		_Object *tO = NULL;
		//	float topProb = 0.0;
		float rSqr = FLT_MAX;
		int i = 0;
		while ((pO = m_pU->get(i++)) != NULL)
		{

			vFloat3 p = pO->getPos();
			float x = p.x - m_vPtarget.x;
			float y = p.y - m_vPtarget.y;
			float r = x * x + y * y;
			IF_CONT(r > rSqr);
			//		IF_CONT(pO->getTopClassProb() < topProb);
			IF_CONT(!bTargetClass(pO->getTopClass()));

			tO = pO;
			//		topProb = pO->getTopClassProb();
			rSqr = r;
		}

		if (tO)
		{
			m_o = *tO;
			m_oTstamp = m_pT->getTfrom();
			return &m_o;
		}

		if (m_pT->getTfrom() - m_oTstamp < m_oTimeout)
		{
			return &m_o;
		}

		return NULL;
	}

	bool _PickingArm::bTargetClass(int iClass)
	{
		IF__(m_vClass.empty(), true);

		for (PICKINGARM_CLASS c : m_vClass)
		{
			IF_CONT(c.m_iClass != iClass);
			return true;
		}

		return false;
	}

	void _PickingArm::speed(const vFloat3 &vS)
	{
		m_pA->setStarget(0, vS.x);
		m_pA->setStarget(1, vS.y);
		m_pA->setStarget(2, vS.z);
	}

	bool _PickingArm::ascend(void)
	{
		m_pA->atomicFrom();
		m_pA->setPtarget(0, m_vPrecover.x);
		m_pA->setPtarget(1, m_vPrecover.y);
		m_pA->setPtarget(2, m_vPrecover.z);
		m_pA->atomicTo();

		IF_F(!m_pA->bComplete(0));
		IF_F(!m_pA->bComplete(1));
		IF_F(!m_pA->bComplete(2));

		return true;
	}

	bool _PickingArm::deliver(void)
	{
		m_pA->atomicFrom();
		m_pA->setPtarget(0, m_vPdeliver.x);
		m_pA->setPtarget(1, m_vPdeliver.y);
		m_pA->setPtarget(2, m_vPdeliver.z);
		m_pA->atomicTo();

		IF_F(!m_pA->bComplete(0));
		IF_F(!m_pA->bComplete(1));
		IF_F(!m_pA->bComplete(2));
		return true;
	}

	bool _PickingArm::descend(void)
	{
		m_pA->atomicFrom();
		m_pA->setPtarget(0, m_vPdescend.x);
		m_pA->setPtarget(1, m_vPdescend.y);
		m_pA->setPtarget(2, m_vPdescend.z);
		m_pA->atomicTo();

		IF_F(!m_pA->bComplete(2));
		return true;
	}

	bool _PickingArm::drop(void)
	{
//		m_pG->grip(true);

//		IF_F(!m_pG->bGrip());
		return true;
	}

	void _PickingArm::move(vFloat3 &vM)
	{
		m_vS = vM;
	}

	void _PickingArm::rotate(vFloat3 &vR)
	{
		m_vR = vR;
	}

	void _PickingArm::grip(bool bOpen)
	{
		IF_(check() != OK_OK);

//		m_pG->grip(bOpen);
	}

	void _PickingArm::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_ModuleBase::console(pConsole);

		_Console *pC = (_Console *)pConsole;
		pC->addMsg("vS = (" + f2str(m_vS.x) + ", " + f2str(m_vS.y) + ", " + f2str(m_vS.z) + ")");
		pC->addMsg("vR = (" + f2str(m_vR.x) + ", " + f2str(m_vR.y) + ", " + f2str(m_vR.z) + ")");
		pC->addMsg("vP = (" + f2str(m_vP.x) + ", " + f2str(m_vP.y) + ", " + f2str(m_vP.z) + ")");
		pC->addMsg("vPtarget = (" + f2str(m_vPtarget.x) + ", " + f2str(m_vPtarget.y) + ", " + f2str(m_vPtarget.z) + ")");
		pC->addMsg("baseAngle = " + f2str(m_baseAngle));
	}

	void _PickingArm::draw(void *pFrame)
	{
		NULL_(pFrame);
		this->_ModuleBase::draw(pFrame);
		IF_(check() != OK_OK);

		Frame *pF = (Frame*)pFrame;

		Mat *pM = pF->m();
		IF_(pM->empty());

		Point pC = Point(m_vP.x * pM->cols, m_vP.y * pM->rows);
		circle(*pM, pC, 5.0, Scalar(255, 255, 0), 2);
	}

}
