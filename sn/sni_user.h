#pragma once

#include "pgc_user.h"
#include "sn_operatorvariables.h"
#include <vector>
#include <unordered_map>

// SNI_User is a class that represents an individual users attachment via the debugger interface to SN, possibly implemented as a session id.
// It holds a main thread and 0..many worker threads.
// The class also holds the SNI_DelayedProcessor.
namespace SNI
{
	class SNI_Transaction;
	class SNI_DelayedProcessor;
	class SNI_Thread;

	class SNI_User : public PGC::PGC_User
	{
	private:
		static void UserThreadFunc(SNI_User* p_User);
	public:
		template <typename T, typename R, typename... Args>
		R* GetOrCreatePointer(Args&&... args) {
			size_t key = typeid(T).hash_code();
			auto it = m_Pointers.find(key);
			if (it != m_Pointers.end()) {
				return static_cast<R*>(it->second);
			}
			// Create, store, and return
			R* instance = new R(std::forward<Args>(args)...);
			m_Pointers[key] = instance;
			return instance;
		}

	private:
		std::unordered_map<size_t, void*> m_Pointers;
	public:
		static SNI_User* GetCurrentUser();

		explicit SNI_User(OnErrorHandler *p_ErrorHandler = &PGC_User::DefaultErrorHandler);
		virtual ~SNI_User();

		SNI_DelayedProcessor* GetDelayedProcessor();

		void AddThread(SNI_Thread* p_Thread);

		SN::SN_OperatorVariables& GetOperators();
		void LogicSetup();

		SNI_Thread* GetThreadByNumber(size_t p_ThreadNum);
		size_t GetNumThreads();
		void WriteStepCounts(ostream& p_Stream);
		std::string StepCountJS();
		void WriteStepCountListJS(ostream& p_Stream);
	private:
		SNI_Transaction* m_Transaction;
		SNI_DelayedProcessor* m_DelayedProcessor;

		// * Operator Variables

		SN::SN_OperatorVariables *m_Operators;

		SNI_Thread* m_MainThread;

		vector<SNI_Thread*> m_ThreadList;
	};
}
