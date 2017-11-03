#ifndef PGC_H_INCLUDED
#define PGC_H_INCLUDED

#pragma once

#include "pgc_base.h"
#include "pgc_transaction.h"
#include "pgc_stacktransaction.h"
#include "pgc_task.h"
#include "ref.h"

namespace PGCX
{
	typedef PGC::PGC_Base Base;
	typedef PGC::PGC_Transaction Transaction;
	typedef PGC::PGC_StackTransaction StackTransaction;
	typedef PGC::PGC_Task Task;
	typedef PGC::PGC_Promotion Promotion;

	template<typename T>
	using SRef = PGC::SRef<T>;

	template<typename T>
	using Ref = PGC::Ref<T>;
}

#endif // PGC_H_INCLUDED
