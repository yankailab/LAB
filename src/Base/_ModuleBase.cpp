/*
 * _ModuleBase.cpp
 *
 *  Created on: Feb 2, 2021
 *      Author: yankai
 */

#include "_ModuleBase.h"

namespace kai
{

    _ModuleBase::_ModuleBase()
    {
        m_pT = NULL;
    }

    _ModuleBase::~_ModuleBase()
    {
        DEL(m_pT);
    }

    bool _ModuleBase::init(void *pKiss)
    {
        IF_F(!this->BASE::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;
        
        Kiss *pKt = pK->child("thread");
        IF_d_T(pKt->empty(), LOG_E("Thread not found"));

        m_pT = new _Thread();
        if (!m_pT->init(pKt))
        {
            DEL(m_pT);
            LOG_E("Thread init failed");
            return false;
        }

        return true;
    }

    bool _ModuleBase::link(void)
    {
        IF_F(!this->BASE::link());
        IF_F(!m_pT->link());

        return true;
    }

    bool _ModuleBase::start(void)
    {
        NULL_F(m_pT);
        return m_pT->start(getUpdate, this);
    }

    int _ModuleBase::check(void)
    {
        NULL__(m_pT, -1);

        return BASE::check();
    }

    void _ModuleBase::update(void)
    {
    }

	bool _ModuleBase::bAlive(void)
    {
        IF_F(check() < 0);

        return m_pT->bAlive();
    }

	bool _ModuleBase::bRun(void)
    {
        IF_F(check() < 0);

        return m_pT->bRun();
    }

	bool _ModuleBase::bStop(void)
    {
        IF_F(check() < 0);

        return m_pT->bStop();
    }

    void _ModuleBase::pause(void)
    {
        IF_(check() < 0);

        m_pT->pause();
    }

    void _ModuleBase::resume(void)
    {
        IF_(check() < 0);

        m_pT->run();
    }

    void _ModuleBase::stop(void)
    {
        IF_(check() < 0);

        m_pT->stop();
    }

	void _ModuleBase::onPause(void)
    {
    }

	void _ModuleBase::onResume(void)
    {
    }

    void _ModuleBase::console(void *pConsole)
    {
        this->BASE::console(pConsole);

        NULL_(m_pT);
        m_pT->console(pConsole);
    }

}
