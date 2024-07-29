#ifndef OpenKAI_src_App_Swarm__SwarmCtrlUI_H_
#define OpenKAI_src_App_Swarm__SwarmCtrlUI_H_

#include "../Protocol/_JSONbase.h"
#include "_SwarmCtrl.h"

namespace kai
{

	class _SwarmCtrlUI : public _JSONbase
	{
	public:
		_SwarmCtrlUI();
		~_SwarmCtrlUI();

		virtual int init(void *pKiss);
		virtual int link(void);
		virtual int start(void);
		virtual int check(void);
		virtual void console(void *pConsole);

	protected:
		virtual void send(void);
		virtual void sendHeartbeat(void);
		virtual void sendNodeUpdate(void);
		virtual void sendNodeClearAll(void);

		// msg handlers
		void handleMsg(string &str);
		void setState(picojson::object &o);

	private:
		void updateW(void);
		static void *getUpdateW(void *This)
		{
			((_SwarmCtrlUI *)This)->updateW();
			return NULL;
		}

		void updateR(void);
		static void *getUpdateR(void *This)
		{
			((_SwarmCtrlUI *)This)->updateR();
			return NULL;
		}

	public:
		_Thread *m_Tr;
		_SwarmCtrl* m_pCtrl;
		_SwarmSearch *m_pSwarm;
		
		INTERVAL_EVENT m_ieSendNodeUpdate;
		INTERVAL_EVENT m_ieSendNodeClearAll;

	};

}
#endif
