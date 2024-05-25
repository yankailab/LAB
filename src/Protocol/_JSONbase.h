#ifndef OpenKAI_src_Protocol__JSONbase_H_
#define OpenKAI_src_Protocol__JSONbase_H_

#include "../Base/_ModuleBase.h"
#include "../IO/_WebSocket.h"
#include "../UI/_Console.h"
#include <openssl/md5.h>

using namespace picojson;

namespace kai
{

	class _JSONbase : public _ModuleBase
	{
	public:
		_JSONbase();
		~_JSONbase();

		virtual bool init(void *pKiss);
		virtual bool link(void);
		virtual bool start(void);
		virtual int check(void);
		virtual void console(void *pConsole);

	protected:
		virtual void send(void);
		virtual bool sendMsg(picojson::object &o);
		virtual void sendHeartbeat(void);

		virtual bool recv(void);
		virtual void handleMsg(const string &str);
		virtual void md5(const string &str, string *pDigest);
		virtual bool str2JSON(const string &str, picojson::value *pJson);

	private:
		void updateW(void);
		static void *getUpdateW(void *This)
		{
			((_JSONbase *)This)->updateW();
			return NULL;
		}

		void updateR(void);
		static void *getUpdateR(void *This)
		{
			((_JSONbase *)This)->updateR();
			return NULL;
		}

	protected:
		_Thread *m_pTr;
		_IObase *m_pIO;

		string m_msgFinishSend;
		string m_msgFinishRecv;
		string m_strB;

		INTERVAL_EVENT m_ieSendHB;
	};

}
#endif
