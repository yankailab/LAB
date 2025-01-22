/*
 * _WebSocketServer.cpp
 *
 *  Created on: August 8, 2016
 *      Author: yankai
 */

#include "_WebSocketServer.h"

namespace kai
{
	static _WebSocketServer *g_pWSserver = NULL;

	_WebSocketServer::_WebSocketServer()
	{
		m_pTr = nullptr;
		g_pWSserver = this;
		m_nClientMax = 128;

		m_host = "localhost";
		m_port = 8080;
		m_tOutMs = 1000;
	}

	_WebSocketServer::~_WebSocketServer()
	{
		m_vClient.clear();
	}

	int _WebSocketServer::init(void *pKiss)
	{
		CHECK_(this->_IObase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("host", &m_host);
		pK->v("port", &m_port);
		pK->v("tOutMs", &m_tOutMs);
		pK->v("nClientMax", &m_nClientMax);

		Kiss *pKt = pK->child("thread");
		if (pKt->empty())
		{
			LOG_E("thread not found");
			return OK_ERR_NOT_FOUND;
		}

		m_pTr = new _Thread();
		CHECK_d_l_(m_pTr->init(pKt), DEL(m_pTr), "thread init failed");

		return OK_OK;
	}

	int _WebSocketServer::link(void)
	{
		CHECK_(this->_IObase::link());

		Kiss *pK = (Kiss *)m_pKiss;
		string n;

		return OK_OK;
	}

	int _WebSocketServer::start(void)
	{
		NULL__(m_pT, OK_ERR_NULLPTR);
		NULL__(m_pTr, OK_ERR_NULLPTR);
		CHECK_(m_pT->start(getUpdateW, this));
		return m_pTr->start(getUpdateR, this);
	}

	void _WebSocketServer::updateW(void)
	{
		// signal(SIGPIPE, SIG_IGN);

		while (m_pT->bAlive())
		{
			m_pT->autoFPS();

			for (wsClient c : m_vClient)
			{
				_WebSocket *pWS = c.getWS();
				IF_CONT(!pWS);

				IO_PACKET_FIFO *pPw = pWS->getPacketFIFOw();
				uint8_t pB[WS_N_BUF];
				int nB;
				while ((nB = pPw->getPacket(pB, WS_N_BUF)) > 0)
				{
					ws_sendframe_bin(c.m_wsConn, (char*)pB, nB);

					/**
					 * Alternative functions:
					 *	 ws_sendframe_bcast(8080, (char *)msg, size, type);
					 *   ws_sendframe()
					 *   ws_sendframe_txt()
					 *   ws_sendframe_txt_bcast()
					 *   ws_sendframe_bin()
					 *   ws_sendframe_bin_bcast()
					 */
				}
			}
		}
	}

	void _WebSocketServer::updateR(void)
	{
		ws_server ws;
		ws.host = m_host.c_str();
		ws.port = m_port;
		ws.thread_loop = 0;
		ws.timeout_ms = m_tOutMs;
		ws.evs.onopen = &sCbOpen;
		ws.evs.onclose = &sCbClose;
		ws.evs.onmessage = &sCbMessage;

		ws_socket(&ws);
	}

	int _WebSocketServer::nClient(void)
	{
		return m_vClient.size();
	}

	_WebSocket* _WebSocketServer::getWS(int i)
	{
		wsClient* pC = getClient(i);
		NULL_N(pC);

		return pC->getWS();
	}

	void _WebSocketServer::sCbOpen(ws_cli_conn_t client)
	{
		NULL_(g_pWSserver);
		g_pWSserver->cbOpen(client);
	}

	void _WebSocketServer::sCbClose(ws_cli_conn_t client)
	{
		NULL_(g_pWSserver);
		g_pWSserver->cbClose(client);
	}

	void _WebSocketServer::sCbMessage(ws_cli_conn_t client,
									  const unsigned char *msg, uint64_t size, int type)
	{
		NULL_(g_pWSserver);
		g_pWSserver->cbMessage(client, msg, size, type);
	}

	void _WebSocketServer::cbOpen(ws_cli_conn_t client)
	{
		char *cli, *port;
		cli = ws_getaddress(client);
		port = ws_getport(client);

		IF_(m_vClient.size() >= m_nClientMax);

		_WebSocket* pWS = new _WebSocket();
		pWS->init();

		wsClient c;
		c.init(pWS);
		c.m_wsConn = client;

		m_vClient.push_back(c);

		LOG_I("Connection opened, addr: " + string(cli) + ", port: " + string(port));
	}

	void _WebSocketServer::cbClose(ws_cli_conn_t client)
	{
		char *cli;
		cli = ws_getaddress(client);

		LOG_I("Connection closed, addr: " + string(cli));
	}

	void _WebSocketServer::cbMessage(ws_cli_conn_t client,
									 const unsigned char *msg, uint64_t size, int type)
	{
		char *cli;
		cli = ws_getaddress(client);

		wsClient* pC = getClient(0);
		NULL_(pC);

		_WebSocket* pWS = pC->getWS();
		NULL_(pWS);

		IO_PACKET_FIFO* pFifo = pWS->getPacketFIFOr();
		NULL_(pFifo);

		pFifo->setPacket((uint8_t*)msg, size);

		LOG_I("Received message: " + string((char *)msg) + ", size: " + i2str(size) + ", type: " + i2str(type) + ", from: " + string(cli));
	}

	wsClient* _WebSocketServer::getClient(int i)
	{
		IF_N(m_vClient.size() <= i);

		return &m_vClient[i];
	}

	wsClient* _WebSocketServer::findClient(const string& addr, const string& port)
	{

	}

	void _WebSocketServer::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_IObase::console(pConsole);

		((_Console *)pConsole)->addMsg("nClients: " + i2str(m_vClient.size()), 1);
	}

}
