#include "threadpool.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "projectFile/IThread.h"

namespace threads
{
	enum class EState
	{		
		eRunning,
		eHasNoJob,
	};

	class CThreadPool;

	class CThread : public projectFile::IThread
	{
	public:
		CThread() = delete;		
		// create a object without a job
		CThread(CThreadPool &a_threadPool, const size_t a_iId);
				
		EState getState() const;

		void setJob(std::thread a_thread); // starts a job
		void join();
		void setState(const EState a_eState);
		void setIsFinished() override;
		
	private:
		std::thread m_thread;
		EState m_eState = EState::eHasNoJob;
		CThreadPool &m_threadPool;
		const size_t m_iId;
	};

	using THREADS = std::vector<CThread>;	
	
	class CThreadPool : public IThreadPool
	{
	public:
		CThreadPool();
		std::mutex &getMutex();
		std::condition_variable &getConditionVariable();
		
		void waitForOne();
		void addJob(const CParameters &a_parameters, const std::string a_sCompileFile, compileFile::INCLUDES_TO_IGNORE &a_includesToIgnore) override;
		void waitForAll() override;
		
	private:
		std::mutex m_mutex;
		std::condition_variable m_conditionVariable;
		THREADS m_threads;

	};

}