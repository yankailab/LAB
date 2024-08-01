#include "_JSONbase.h"

namespace kai
{

    _JSONbase::_JSONbase()
    {
        m_pTr = nullptr;
        m_pIO = nullptr;

        m_msgFinishSend = "";
        m_msgFinishRecv = "EOJ";
    }

    _JSONbase::~_JSONbase()
    {
        DEL(m_pTr);
    }

    int _JSONbase::init(void *pKiss)
    {
        CHECK_(this->_ModuleBase::init(pKiss));
        Kiss *pK = (Kiss *)pKiss;

        pK->v("msgFinishSend", &m_msgFinishSend);
        pK->v("msgFinishRecv", &m_msgFinishRecv);

        int v = SEC_2_USEC;
        pK->v("ieSendHB", &v);
        m_ieSendHB.init(v);

        Kiss *pKt = pK->child("threadR");
        if (pKt->empty())
        {
            LOG_E("threadR not found");
            return OK_ERR_NOT_FOUND;
        }

        m_pTr = new _Thread();
        CHECK_d_l_(m_pTr->init(pKt), DEL(m_pTr), "threadR init failed");

        return true;
    }

	int _JSONbase::link(void)
	{
		CHECK_(this->_ModuleBase::link());
        CHECK_(m_pTr->link());

		Kiss *pK = (Kiss *)m_pKiss;

        string n;
        n = "";
        pK->v("_IObase", &n);
        m_pIO = (_IObase *)(pK->findModule(n));
        NULL__(m_pIO, OK_ERR_NOT_FOUND);

		return OK_OK;
	}
    
    int _JSONbase::start(void)
    {
        NULL__(m_pT, OK_ERR_NULLPTR);
        NULL__(m_pTr, OK_ERR_NULLPTR);
        CHECK_(m_pT->start(getUpdateW, this));
        return m_pTr->start(getUpdateR, this);
    }

    int _JSONbase::check(void)
    {
        NULL__(m_pTr, OK_ERR_NULLPTR);
        NULL__(m_pIO, OK_ERR_NULLPTR);
        IF__(!m_pIO->bOpen(), OK_ERR_NOT_READY);

        return this->_ModuleBase::check();
    }

    void _JSONbase::updateW(void)
    {
        while (m_pT->bAlive())
        {
            if (!m_pIO)
            {
                m_pT->sleepT(SEC_2_USEC);
                continue;
            }

            m_pT->autoFPSfrom();

            send();

            m_pT->autoFPSto();
        }
    }

    void _JSONbase::send(void)
    {
        IF_(check() != OK_OK);

        if (m_ieSendHB.update(m_pT->getTfrom()))
        {
//            sendHeartbeat();
        }
    }

    bool _JSONbase::sendMsg(picojson::object &o)
    {
        string msg = picojson::value(o).serialize() + m_msgFinishSend;

        if (m_pIO->ioType() == io_webSocket)
        {
            _WebSocket *pWS = (_WebSocket *)m_pIO;
            return pWS->write((unsigned char *)msg.c_str(), msg.size(), WS_MODE_TXT);
        }

        return m_pIO->write((unsigned char *)msg.c_str(), msg.size());
    }

    void _JSONbase::sendHeartbeat(void)
    {
        object o;
        JO(o, "id", i2str(1));
        JO(o, "cmd", "heartbeat");
        JO(o, "t", li2str(m_pT->getTfrom()));

        sendMsg(o);
    }

    void _JSONbase::updateR(void)
    {
        while (m_pTr->bAlive())
        {
            m_pTr->autoFPSfrom();

            if (recv())
            {
                handleMsg(m_strB);
                m_strB.clear();
            }

            m_pTr->autoFPSto();
        }
    }

    bool _JSONbase::recv()
    {
        IF_F(check() != OK_OK);

        unsigned char B;
        unsigned int nStrFinish = m_msgFinishRecv.length();

        while (m_pIO->read(&B, 1) > 0)
        {
            m_strB += B;
            IF_CONT(m_strB.length() <= nStrFinish);

            string lstr = m_strB.substr(m_strB.length() - nStrFinish, nStrFinish);
            IF_CONT(lstr != m_msgFinishRecv);

            m_strB.erase(m_strB.length() - nStrFinish, nStrFinish);
            LOG_I("Received: " + m_strB);
            return true;
        }

        return false;
    }

    void _JSONbase::handleMsg(const string &str)
    {
        value json;
        IF_(!str2JSON(str, &json));

        object &jo = json.get<object>();
        string cmd = jo["cmd"].get<string>();
    }

    bool _JSONbase::str2JSON(const string &str, picojson::value *pJson)
    {
        NULL_F(pJson);

        string err;
        const char *jsonstr = str.c_str();
        parse(*pJson, jsonstr, jsonstr + strlen(jsonstr), &err);
        IF_F(!pJson->is<object>());

        return true;
    }

    void _JSONbase::md5(const string &str, string *pDigest)
    {
        unsigned char digest[MD5_DIGEST_LENGTH];
        MD5((const unsigned char *)str.c_str(), str.length(), digest);

        string strD((char *)digest);
        *pDigest = strD;
        LOG_I("md5: " + *pDigest);
    }

    void _JSONbase::console(void *pConsole)
    {
        NULL_(pConsole);
        this->_ModuleBase::console(pConsole);

        string msg;
        if (m_pIO->bOpen())
            msg = "Connected";
        else
            msg = "Not connected";

        ((_Console *)pConsole)->addMsg(msg, 0);
    }

}
