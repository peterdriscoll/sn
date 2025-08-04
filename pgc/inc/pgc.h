#pragma once

#include "pgc_promotemacros.h"
#include "pgc_base.h"
#include "pgc_transaction.h"
#include "pgc_stacktransaction.h"
#include "pgc_task.h"
#include "pgc_user.h"
#include "ref.h"
#include "memberref.h"
#include "sref.h"

namespace PGCX
{
	typedef PGC::PGC_Base Base;
	typedef PGC::PGC_Transaction PGC_Transaction;
	typedef PGC::PGC_StackTransaction StackTransaction;
	typedef PGC::PGC_Task Task;
	typedef PGC::PGC_User PGC_User;
	typedef PGC::PGC_Promotion Promotion;

	// Smart pointer-style wrappers
	template <class T>
	using Ref = PGC::Ref<T>;

	template <class T>
	using SRef = PGC::SRef<T>;

	template <class T>
	using MemberRef = PGC::MemberRef<T>;
}
