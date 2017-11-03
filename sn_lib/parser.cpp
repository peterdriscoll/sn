#include "parser.h"

Parser::Parser()
{

}

Parser::~Parser()
{

}

SN::SN_Variable Parser::Expression()
{
	SN::SN_DECLARE( E );
	SN::SN_DECLARE( S );
	SN::SN_DECLARE( T );

	{
		SN::SN_DECLARE( x );
		SN::SN_DECLARE( y );
		SN::SN_DECLARE( z );

		// E x = var y, z
		//	  in S (T y) z where x = y + z 

		ASSERTM( ( ( E(x) == SN::SN_Local(y, SN::SN_Local(z, S(T(y))(z) ) ) ) && (x == y + z) ).PartialAssert(),
					 "Definition E x = var y, z in S (T y) z where x = y + z failed" );
	}

	{
		SN::SN_DECLARE( e );
		SN::SN_DECLARE( x );
		SN::SN_DECLARE( y );
		SN::SN_DECLARE( z );

		// S e x = var y, z
		//	in	(e {|} E z where x = "|" + z) 
		//    ||	e where x = ""

		ASSERTM( ( ( S(e)(x) == SN::SN_Local(y, SN::SN_Local(z, (x == SN::SN_String("|") + z).If(SN::SN_Meta(1, SN::SN_Meta(-1, e) || SN::SN_Meta(-1, E(z))), (x == SN::SN_String("")).If(e, SN::SN_Error(""))) ) ) ) ).PartialAssert(),
			         "S e x = var y, z in (e {|} E z where x = \"|\" + z) || e where x = \"\"" );
	}

	return E;
}
