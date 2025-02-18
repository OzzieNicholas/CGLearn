#include "sketch/RPC/RPCManager.h"

#include <Windows.h>
#include <stack>
#include <ppl.h>
#include <ppltasks.h>

#include "Engine/Util/StrUtil.h"
#include "Engine/Util/FileUtil.h"
#include "Engine/Util/SysUtil.h"
#include "Engine/Util/TCPServer.h"
#include "Engine/Libs/rapidJson/include/rapidjson/document.h"
#include "Engine/Libs/rapidJson/include/rapidjson/error/en.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/TeklaLis.h"

#include "sketch/STCore.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/RPC/RPCProcessor.h"

using namespace concurrency;
using namespace Engine;

namespace sketch
{
	class RPCManagerImpl : public RPCManager
	{
	public:
		RPCManagerImpl() :
			m_pServer(nullptr),
			m_cmdId(0)
		{

		}

		virtual ~RPCManagerImpl(void) {
			if (m_pServer) {
				delete m_pServer;
				m_pServer = nullptr;
			}
		}

		virtual bool init() override {
			m_pServer = TCPServer::create();
			int nTry = 0;
			while (nTry < 5) {
				if (m_pServer->start("127.0.0.1", 8498, true))
					break;

				Log::log("", "[Error][RPC]Failed to start server, retrying...\n");
				Sleep(1000);
				nTry++;
			}

			m_pServer->setReceiveTimeOut(60000);

			m_mainThread = concurrency::create_task([&] {
				_mainThreadFunc();
				});

			return true;
		}

		virtual bool onDocPrepare(const Engine::Path& projPath) override {
			return true;
		}

		virtual bool onDocNew(const Engine::Path& projPath) override {
			return true;
		}

		virtual bool onDocLoad(const Engine::Path& projPath) override {
			return true;
		}

		virtual bool onDocLoaded(const Engine::Path& projPath) override {
			return true;
		}

		virtual bool onDocClose(void) override {
			this->update(0); //process cmds
			return true;
		}

		virtual bool onAppClose(void) override {
			return true;
		}

		virtual void update(unsigned long long tm) override {
			m_mtx.lock();
			for (auto& it : m_executeCmds) {
				auto& tuple = it.second;

				auto& cmd = std::get<0>(tuple);
				auto itp = m_processors.find(cmd);
				if (itp == m_processors.end())
					continue;

				auto pRetObj = std::get<2>(tuple);
				for (auto pProc : itp->second) {
					if (pProc && pProc->onRPCCommand(cmd, *pRetObj))
						break;
				}

				std::get<1>(tuple)->set();
			}
			m_mtx.unlock();
		}

		virtual void addProcessor(const std::string& cmd, RPCProcessor* pProc) override {
			if (!pProc)
				return;

			m_processors[cmd].insert(pProc);
		}

		virtual void removeProcessor(const std::string& cmd, RPCProcessor* pProc) override {
			m_processors[cmd].erase(pProc);
		}

	protected:
		void _mainThreadFunc() {
			if (!m_pServer->listen()) {
				m_pServer->close();
				return;
			}

			Log::print("[RPCManager] Listening...\n");

			while (true) {
				try {
					//等待客户端连接
					uint64_t client;
					if (!m_pServer->accept(&client)) {
						Sleep(10);
						continue;
					}

					Log::print("[RPCManager] new client\n");

					concurrency::create_task([&] {
						char cmdBuf[128];
						while (true) {
							//接收数据
							int res = m_pServer->receive(client, cmdBuf, sizeof(cmdBuf));
							if (res == 0) { //timeout or error
								break;
							}

							if (res == 5 && memcmp(cmdBuf, "quit", 4) == 0) {
								Log::print("[RPCManager] quit\n");
								break;
							}

							Log::print("[RPCManager] %s\n", cmdBuf);

							//通知主线程远程命令
							long long cmdId = m_cmdId++;
							concurrency::event evt;
							Engine::DObject robj;

							m_mtx.lock();
							m_executeCmds[cmdId] = { cmdBuf, &evt, &robj };
							m_mtx.unlock();

							//等待主线程处理命令，主线程处理完毕将结果传输给客户端
							evt.wait();
							m_mtx.lock();
							m_executeCmds.erase(cmdId);
							m_mtx.unlock();

							unsigned char* pObjBuf = nullptr;
							auto objBufSize = robj.toBinary(&pObjBuf, "");

							char* pSendBuf = new char[objBufSize + sizeof(int)];
							*(int*)pSendBuf = (int)objBufSize;
							memcpy(pSendBuf + sizeof(int), pObjBuf, objBufSize);

							m_pServer->send(client, pSendBuf, *(int*)pSendBuf + sizeof(int));

							delete[] pObjBuf;
							delete[] pSendBuf;
						}
						});
				}
				catch (...) {
					Log::print("[RPCManager] ReStart...\n");
					m_pServer->close();
					m_pServer->start("127.0.0.1", 8498, true);
					m_pServer->setReceiveTimeOut(60000);
					m_pServer->listen();
				}
			}
		}

	protected:
		TCPServer* m_pServer;
		task<void> m_mainThread;
		std::mutex m_mtx;

		std::unordered_map<std::string, std::unordered_set<RPCProcessor*>> m_processors;

		std::atomic<long long> m_cmdId;
		std::unordered_map<long long, std::tuple<std::string, concurrency::event*, Engine::DObject*>> m_executeCmds;
	};

	RPCManager* RPCManager::create() {
		return new RPCManagerImpl();
	}

	RPCManager::~RPCManager(void)
	{

	}
}