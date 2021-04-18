#include "threadpoolImpl.h"
#include "compileFile/compileFile.h"
#include "system/logger.h"
#include "tools/strings.h"

namespace threads
{	

	CThread::CThread(CThreadPool &a_threadPool, const size_t a_iId) :
			m_threadPool(a_threadPool), m_iId(a_iId)
	{
		logger::add(logger::EType::eDebugThreads, "CThread constructor : " + tools::strings::itos(m_iId));
	}

	void CThread::setIsFinished()
	{
		setState(EState::eHasNoJob);
	}

	EState CThread::getState() const
	{
		// no guard here. This method is to be called from CThreadPool::waitXXX() methods, which handle that.
		logger::add(logger::EType::eDebugThreads, "CThread getState : " + tools::strings::itos(m_iId) + " " + tools::strings::itos(static_cast<int>(m_eState)));
		return m_eState;
	}

	void CThread::setJob(std::thread a_thread)
	{
		logger::add(logger::EType::eDebugThreads, "CThread setJob : " + tools::strings::itos(m_iId));
		join();
		m_thread = std::move(a_thread);
		setState(EState::eRunning);
	}
	void CThread::join()
	{
		logger::add(logger::EType::eDebugThreads, "CThread join : " + tools::strings::itos(m_iId));
		if (m_thread.joinable())
			m_thread.join();
	}
	void CThread::setState(const EState a_eState)
	{
		logger::add(logger::EType::eDebugThreads, "CThread setState : " + tools::strings::itos(m_iId) + " " + tools::strings::itos(static_cast<int>(a_eState)));
		{
			std::lock_guard lockGuard(m_threadPool.getMutex());
			m_eState = a_eState;
		}
		m_threadPool.getConditionVariable().notify_one();
	}	


	// CThreadPool

	CThreadPool::CThreadPool()
	{	
		const size_t iThreads = std::thread::hardware_concurrency() - 1;
		for (size_t i = 0; i < iThreads; i++)
		{
			m_threads.emplace_back(CThread(*this, i + 1));
		}
	}

	std::mutex &CThreadPool::getMutex()
	{
		return m_mutex;
	}
	std::condition_variable &CThreadPool::getConditionVariable()
	{
		return m_conditionVariable;
	}

	void CThreadPool::waitForOne()
	{
		logger::add(logger::EType::eDebugThreads, "CThreadPool wait");
		std::unique_lock<std::mutex> lock(m_mutex);
		{
			THREADS &threads = m_threads; // access to m_threads for the lambda
			auto fncWait = [&threads]() -> bool
			{
				for (auto &thread : threads)
				{
					if (thread.getState() != EState::eRunning)
						return true;
				}
				return false;
			};

			m_conditionVariable.wait(lock, fncWait);			
		}
		logger::add(logger::EType::eDebugThreads, "CThreadPool wait end");
	}


	void CThreadPool::addJob(const CParameters &a_parameters, const compileFile::CCompileFileInfo a_CompileFileInfo, compileFile::INCLUDES_TO_IGNORE &a_includesToIgnore)
	{
		logger::add(logger::EType::eDebugThreads, "CThreadPool addJob " + a_CompileFileInfo.getCompileFile());

		waitForOne();

		CThread *pThread = nullptr;
		{
			std::lock_guard lockGuard(m_mutex);
			for (auto &thread : m_threads)
			{
				if (thread.getState() != EState::eRunning)
				{
					pThread = &thread;
					break;
				}
			}
		}
		if (pThread)
		{
			std::thread threadLoc(&compileFile::checkCompileFile, std::ref(*pThread), std::ref(a_parameters), a_CompileFileInfo, std::ref(a_includesToIgnore));
			pThread->setJob(std::move(threadLoc));
		}

	}

	void CThreadPool::waitForAll()
	{
		logger::add(logger::EType::eDebugThreads, "CThreadPool waitForAll ");
		for (auto &thread : m_threads)
		{
			thread.join();
		}
		logger::add(logger::EType::eDebugThreads, "CThreadPool waitForAll end");
	}


	std::unique_ptr<IThreadPool> createThreadPool()
	{
		return std::make_unique<CThreadPool>();
	}
	
}