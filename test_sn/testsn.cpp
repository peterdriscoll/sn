#include "testsn.h"
#include "cppunit/nova_extensions/hacksforcppunitstdnamespace.h"
#include "sn.h"
#include "cppunit/TextOutputter.h"
#include "../pgc/inc/pgc.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TestSN);

void 
TestSN::setUp()
{
    PGC_Transaction::ResetNetMemoryUsed();
    PGC_Transaction::ResetGrossMemoryUsed();
}


void 
TestSN::tearDown()
{
    CPPUNIT_ASSERT( PGC_Transaction::TotalNetMemoryUsed() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionUsedMemory() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionFreeMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
}

//	TestStringValue
//		Test operator equals on program model string value gives the program model function call.
//
//		In the program model, operations dont return values.  Instead they return program model objects that represent the calculation of the values.
//		The function Equivalent determines if the project model produced by == is the application of SN::SN_Equals as a function call on the two parameters.
//
//		SN::SN_Equals is a SN::SN_FunctionDef object that defines the function.
void 
TestSN::TestStringEquivalent()
{
    CPPUNIT_ASSERT( PGC_Transaction::TotalNetMemoryUsed() == 0 );
    {
        PGC_Transaction transaction;
        
        CPPUNIT_ASSERT( (SN::SN_String("dog") == SN::SN_String("dog")).Equivalent(SN::SN_Function( SN::SN_Function(SN::Equals, SN::SN_String("dog")), SN::SN_String("dog") ) ));
        CPPUNIT_ASSERT( !(SN::SN_String("dog") == SN::SN_String("dog")).Equivalent(SN::SN_Function( SN::SN_Function(SN::Equals, SN::SN_String("dog")), SN::SN_String("cat") ) ));
        CPPUNIT_ASSERT( (SN::SN_String("dog") + SN::SN_String("cat")).Equivalent(SN::SN_Function( SN::SN_Function(SN::Add, SN::SN_String("dog")), SN::SN_String("cat") ) ));
        CPPUNIT_ASSERT( (SN::SN_String("dog") == SN::SN_String("dog")).Equivalent(SN::Equals(SN::SN_String("dog"))(SN::SN_String("dog")) ));
        CPPUNIT_ASSERT( !(SN::SN_String("dog") == SN::SN_String("dog")).Equivalent(SN::Equals(SN::SN_String("dog"))(SN::SN_String("cat")) ));
        CPPUNIT_ASSERT( (SN::SN_String("dog") + SN::SN_String("cat")).Equivalent(SN::Add(SN::SN_String("dog"))(SN::SN_String("cat")) ));
    }
    
    CPPUNIT_ASSERT( PGC_Transaction::TotalNetMemoryUsed() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionUsedMemory() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionFreeMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
}

void 
TestSN::TestStringEvaluate()
{
    {
        PGC_Transaction transaction;
        
        CPPUNIT_ASSERT( (SN::SN_String("dog") == SN::SN_String("dog")).Evaluate().GetBool() );
        CPPUNIT_ASSERT( !(SN::SN_String("dog") == SN::SN_String("cat")).Evaluate().GetBool() );
        
        SN::SN_DECLARE_VALUE( z, SN::SN_String("dog") );
        //	CPPUNIT_ASSERT( (SN::SN_String("dog") + SN::SN_String("cat") == SN::SN_String("dogcat")).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::SN_String("dog") == z).Evaluate().GetBool() );
    }
}

void 
TestSN::TestStringPartialEvaluate()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( y );
        SN::SN_DECLARE( x );
        SN::SN_DECLARE_VALUE( z, SN::SN_String("dog") );
        
        
        CPPUNIT_ASSERT( (SN::SN_String("dog") == y).PartialEvaluate().Equivalent(SN::SN_String("dog") == y) );
        CPPUNIT_ASSERT( !(SN::SN_String("dog") == y).PartialEvaluate().Equivalent(SN::SN_String("dog") == x) );
        CPPUNIT_ASSERT( (SN::SN_String("dog") == SN::SN_String("dog")).PartialEvaluate().Equivalent(SN::SN_Bool(true)) );
        CPPUNIT_ASSERT( (SN::SN_String("dog") == z).PartialEvaluate().GetBool() );
        
        CPPUNIT_ASSERT( (SN::SN_String("dog") + y == SN::SN_String("dogcat")).PartialEvaluate().Equivalent(SN::SN_Function( SN::SN_Function(SN::Equals, SN::SN_Function( SN::SN_Function(SN::Add, SN::SN_String("dog")), y)), SN::SN_String("dogcat")) ));
        CPPUNIT_ASSERT( (SN::SN_String("dog") + z == SN::SN_String("dogdog")).PartialEvaluate().GetBool() );
        
        CPPUNIT_ASSERT( (SN::SN_String("dog") + z == "dogdog").PartialEvaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::SN_String("dog") + "dog" == "dogdog").PartialEvaluate().GetBool() );
    }
}

void
TestSN::TestStringAssert()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( y );
        
        CPPUNIT_ASSERT( (SN::SN_String("dog") == y ).Assert() );
        CPPUNIT_ASSERT( (SN::SN_String("dog") == y).Evaluate().GetBool() );
        
        SN::SN_DECLARE( z );
        
        CPPUNIT_ASSERT( (SN::SN_String("dog") + z == "dogcat").Assert() );
        SN::SN_Value cat_value = z.GetValue();
        string s_cat_value = cat_value.GetString();
        CPPUNIT_ASSERT( s_cat_value == "cat");
        CPPUNIT_ASSERT( (SN::SN_String("cat") == z).Evaluate().GetBool() );
    }
}

void
TestSN::TestStringPartialAssert()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( y );
        SN::SN_DECLARE( z );
        
        CPPUNIT_ASSERT( (y == z ).PartialAssert() );
        CPPUNIT_ASSERT( y.PartialEvaluate().Equivalent(z.PartialEvaluate()) );
        CPPUNIT_ASSERT( z.PartialEvaluate().Equivalent(y.PartialEvaluate()) );
        CPPUNIT_ASSERT( (SN::SN_String("dog") == y).PartialEvaluate().Equivalent((SN::SN_String("dog") == z).PartialEvaluate()) );
        CPPUNIT_ASSERT( ( z == "dog" ).Assert() );
        //	CPPUNIT_ASSERT( y.Evaluate().Equivalent(SN::SN_String("dog")) );
    }
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( a );
        SN::SN_DECLARE( b );
        CPPUNIT_ASSERT( (SN::SN_String("dog") + a == b).PartialAssert() );
        //	CPPUNIT_ASSERT( a.GetValue().Equivalent(b.SubtractLeft(SN::SN_String("dog"))) );
        
        CPPUNIT_ASSERT( ( b == "dogcat" ).Assert() );
        CPPUNIT_ASSERT( ( a.Evaluate().Equivalent(SN::SN_String("cat")) ) );
    }
}

void 
TestSN::TestBoolEquivalent()
{
    {
        PGC_Transaction transaction;
        
        CPPUNIT_ASSERT( (SN::SN_Bool(true) == SN::SN_Bool(true)).Equivalent(SN::SN_Function( SN::SN_Function(SN::Equals, SN::SN_Bool(true)), SN::SN_Bool(true) ) ));
        CPPUNIT_ASSERT( !(SN::SN_Bool(true) == SN::SN_Bool(true)).Equivalent(SN::SN_Function( SN::SN_Function(SN::Equals, SN::SN_Bool(true)), SN::SN_Bool(false) ) ));
        CPPUNIT_ASSERT( (SN::SN_Bool(true) && SN::SN_Bool(false)).Equivalent(SN::SN_Function( SN::SN_Function(SN::And, SN::SN_Bool(true)), SN::SN_Bool(false) ) ));
        CPPUNIT_ASSERT( (SN::SN_Bool(false) || SN::SN_Bool(false)).Equivalent(SN::SN_Function( SN::SN_Function(SN::Or, SN::SN_Bool(false)), SN::SN_Bool(false) ) ));
        CPPUNIT_ASSERT( (!SN::False).Equivalent(SN::SN_Function(SN::Not, SN::False) ));
        CPPUNIT_ASSERT( SN::False.If(SN::False, SN::True).Equivalent(SN::SN_Function( SN::SN_Function( SN::SN_Function(SN::If, SN::False), SN::False), SN::True ) ));
    }
}

void 
TestSN::TestBoolEvaluate()
{
    {
        PGC_Transaction transaction;
        
        CPPUNIT_ASSERT( (SN::SN_Bool(true) == SN::SN_Bool(true)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( !(SN::SN_Bool(true) == SN::SN_Bool(false)).Evaluate().GetBool() );
    }
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE_VALUE( z, SN::SN_Bool(true) );
        CPPUNIT_ASSERT( (SN::SN_Bool(true) == z).Evaluate().GetBool() );
        
        SN::SN_DECLARE_VALUE( k, SN::True );
        SN::SN_DECLARE_VALUE( l, SN::False );
        SN::SN_DECLARE_VALUE( m, SN::True );
        CPPUNIT_ASSERT( (k.If(l, m) == SN::False).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::True.If("dog", "cat") == "dog").Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::False.If("dog", "cat") == "cat").Evaluate().GetBool() );
    }
    {
        PGC_Transaction transaction;
        
        //Truth table AND
        CPPUNIT_ASSERT( !(SN::SN_Bool(false) && SN::SN_Bool(false)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( !(SN::SN_Bool(false) && SN::SN_Bool(true)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( !(SN::SN_Bool(true) && SN::SN_Bool(false)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::SN_Bool(true) && SN::SN_Bool(true)).Evaluate().GetBool() );
    }
    {
        PGC_Transaction transaction;
        
        // Truth table OR
        CPPUNIT_ASSERT( !(SN::SN_Bool(false) || SN::SN_Bool(false)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::SN_Bool(false) || SN::SN_Bool(true)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::SN_Bool(true) || SN::SN_Bool(false)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::SN_Bool(true) || SN::SN_Bool(true)).Evaluate().GetBool() );
    }
    {
        PGC_Transaction transaction;
        
        // Truth table AND
        CPPUNIT_ASSERT( ((SN::SN_Bool(false) && SN::SN_Bool(false)) == SN::SN_Bool(false)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ((SN::SN_Bool(false) && SN::SN_Bool(true)) == SN::SN_Bool(false)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ((SN::SN_Bool(true) && SN::SN_Bool(false) )== SN::SN_Bool(false)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ((SN::SN_Bool(true) && SN::SN_Bool(true)) == SN::SN_Bool(true)).Evaluate().GetBool() );
    }
    {
        PGC_Transaction transaction;
        
        // Truth table OR
        CPPUNIT_ASSERT( ((SN::SN_Bool(false) || SN::SN_Bool(false)) == SN::SN_Bool(false)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ((SN::SN_Bool(false) || SN::SN_Bool(true)) == SN::SN_Bool(true)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ((SN::SN_Bool(true) || SN::SN_Bool(false)) == SN::SN_Bool(true)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ((SN::SN_Bool(true) || SN::SN_Bool(true)) == SN::SN_Bool(true)).Evaluate().GetBool() );
    }
    {
        PGC_Transaction transaction;
        
        // Truth table if A then B else C
        CPPUNIT_ASSERT( (SN::False.If(SN::False, SN::False) == SN::False).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::False.If(SN::False, SN::True) == SN::True).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::False.If(SN::True, SN::False) == SN::False).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::False.If(SN::True, SN::True) == SN::True).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::True.If(SN::False, SN::False) == SN::False).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::True.If(SN::False, SN::True) == SN::False).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::True.If(SN::True, SN::False) == SN::True).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::True.If(SN::True, SN::True) == SN::True).Evaluate().GetBool() );
    }
}

void 
TestSN::TestBoolPartialEvaluate()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( y );
        SN::SN_DECLARE( x );
        SN::SN_DECLARE_VALUE( z, SN::SN_Bool(true) );
        
        
        CPPUNIT_ASSERT( (SN::SN_Bool(true) == y).PartialEvaluate().Equivalent(SN::SN_Bool(true) == y) );
        CPPUNIT_ASSERT( !(SN::SN_Bool(true) == y).PartialEvaluate().Equivalent(SN::SN_Bool(true) == x) );
        CPPUNIT_ASSERT( (SN::SN_Bool(true) == SN::SN_Bool(true)).PartialEvaluate().Equivalent(SN::SN_Bool(true)) );
        CPPUNIT_ASSERT( (SN::SN_Bool(true) == z).PartialEvaluate().GetBool() );
        
        CPPUNIT_ASSERT( (SN::SN_Bool(false) || y).PartialEvaluate().Equivalent( y ) );
        CPPUNIT_ASSERT( (y || SN::SN_Bool(false) ).PartialEvaluate().Equivalent( y ) );
        CPPUNIT_ASSERT( (SN::SN_Bool(true) || y).PartialEvaluate().Equivalent( SN::SN_Bool(true) ) );
        CPPUNIT_ASSERT( (y || SN::SN_Bool(true) ).PartialEvaluate().Equivalent( SN::SN_Bool(true) ) );
        
        CPPUNIT_ASSERT( (SN::SN_Bool(true) && y).PartialEvaluate().Equivalent( y ) );
        CPPUNIT_ASSERT( (y && SN::SN_Bool(true) ).PartialEvaluate().Equivalent( y ) );
        CPPUNIT_ASSERT( (SN::SN_Bool(false) && y).PartialEvaluate().Equivalent( SN::SN_Bool(false) ) );
        CPPUNIT_ASSERT( (y && SN::SN_Bool(false) ).PartialEvaluate().Equivalent( SN::SN_Bool(false) ) );
        
        CPPUNIT_ASSERT( (SN::SN_Bool(false) || z).PartialEvaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::SN_Bool(true) && true).PartialEvaluate().GetBool() );
        
        CPPUNIT_ASSERT( (SN::SN_Bool(true) && true).PartialEvaluate().GetBool() );
        
        SN::SN_DECLARE( i );
        CPPUNIT_ASSERT( (i == i).PartialEvaluate().GetBool() );
        
        // If
        
        SN::SN_DECLARE( k );
        SN::SN_DECLARE( l);
        
        CPPUNIT_ASSERT( (k.If(SN::False, SN::True) == l).PartialEvaluate().Equivalent((!k == l).PartialEvaluate()) );
        CPPUNIT_ASSERT( (k.If(SN::True, SN::False) == l).PartialEvaluate().Equivalent((k == l).PartialEvaluate()) );
        CPPUNIT_ASSERT( k.If(SN::True, SN::True).PartialEvaluate().Equivalent(SN::True) );
        CPPUNIT_ASSERT( k.If(SN::False, SN::False).PartialEvaluate().Equivalent(SN::False) );
        CPPUNIT_ASSERT( SN::True.If(k, l).PartialEvaluate().Equivalent(k) );
        CPPUNIT_ASSERT( SN::False.If(k, l).PartialEvaluate().Equivalent(l) );
        
        CPPUNIT_ASSERT( SN::True.If(SN::True, SN::False).PartialEvaluate().Equivalent(SN::True) );
        
        // Not
        CPPUNIT_ASSERT( (!!k).PartialEvaluate().Equivalent(k) );
        CPPUNIT_ASSERT( (!(k.If(SN::False, SN::True))).PartialEvaluate().Equivalent(k) );
    }
}

void
TestSN::TestBoolAssert()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( y );
        
        CPPUNIT_ASSERT( (SN::SN_Bool(true) == y ).Assert() );
        CPPUNIT_ASSERT( (SN::SN_Bool(true) == y).Evaluate().GetBool() );
        
        SN::SN_DECLARE( z );
        
        CPPUNIT_ASSERT( (z || SN::SN_Bool(false) == SN::SN_Bool(true)).Assert() );
        
        CPPUNIT_ASSERT( (SN::SN_Bool(true) == z).Evaluate().GetBool() );
        CPPUNIT_ASSERT( !(SN::SN_Bool(false) && z).PartialEvaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::SN_Bool(true) && SN::SN_Bool(true)).PartialEvaluate().GetBool() );
        
        SN::SN_DECLARE( k1 );
        SN::SN_DECLARE_VALUE( l1, SN::False );
        
        CPPUNIT_ASSERT( (k1.If(SN::False, SN::True) == l1).Assert() );
        CPPUNIT_ASSERT( (k1 == !l1).Evaluate().GetBool() );
        
        SN::SN_DECLARE( k2 );
        SN::SN_DECLARE_VALUE( l2, SN::True );
        
        CPPUNIT_ASSERT( (k2.If(SN::False, SN::True) == l2).Assert() );
        CPPUNIT_ASSERT( (k2 == !l2).Evaluate().GetBool() );
        
        SN::SN_DECLARE( m );
        SN::SN_DECLARE( n );
        SN::SN_DECLARE( o );
        
        CPPUNIT_ASSERT( (o.If(SN::False, SN::True) == SN::False).Assert() );
        CPPUNIT_ASSERT( o.Evaluate().GetBool() );
        CPPUNIT_ASSERT( (o.If(m, n) == SN::True).Assert() );
        CPPUNIT_ASSERT( m.Evaluate().GetBool() );
        CPPUNIT_ASSERT( ((!o).If(m, n) == m).Assert() );
        CPPUNIT_ASSERT( n.Evaluate().GetBool() );
    }
}

void
TestSN::TestBoolPartialAssert()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( y );
        SN::SN_DECLARE( z );
        
        CPPUNIT_ASSERT( (y == z ).PartialAssert() );
        CPPUNIT_ASSERT( y.PartialEvaluate().Equivalent(z.PartialEvaluate()) );
        CPPUNIT_ASSERT( z.PartialEvaluate().Equivalent(y.PartialEvaluate()) );
        CPPUNIT_ASSERT( (SN::SN_Bool(true) == y).PartialEvaluate().Equivalent((SN::SN_Bool(true) == z).PartialEvaluate()) );
        CPPUNIT_ASSERT( ( z == SN::SN_Bool(true) ).Assert() );
        CPPUNIT_ASSERT( y.Evaluate().Equivalent(SN::True) );
        
        SN::SN_DECLARE( a );
        SN::SN_DECLARE( b );
        CPPUNIT_ASSERT( (SN::False || a == b).PartialAssert() );
        
        CPPUNIT_ASSERT( ( b == SN::True ).Assert() );
        CPPUNIT_ASSERT( ( a.Evaluate().Equivalent(SN::True) ) );
    }
}

void 
TestSN::TestDoubleEquivalent()
{
    {
        PGC_Transaction transaction;
        
        CPPUNIT_ASSERT( ((SN::SN_Long(1) + SN::SN_Long(2)) == SN::SN_Long(3)).Evaluate().GetBool() );
        
        CPPUNIT_ASSERT( ( SN::SN_Double(5.5) == SN::SN_Double(7.8) ).Equivalent( SN::Equals(SN::SN_Double(5.5))(SN::SN_Double(7.8)) ) );
        CPPUNIT_ASSERT( !( SN::SN_Double(9.0) == SN::SN_Double(9.5) ).Equivalent( SN::Equals(SN::SN_Double(9.0))(SN::SN_Double(9.7)) ) );
        CPPUNIT_ASSERT( ( SN::SN_Double(8.5) + SN::SN_Double(4.5) ).Equivalent( SN::Add(SN::SN_Double(8.5))(SN::SN_Double(4.5)) ) );
        CPPUNIT_ASSERT( ( SN::SN_Double(8.5) - SN::SN_Double(4.5) ).Equivalent( SN::Subtract(SN::SN_Double(8.5))(SN::SN_Double(4.5)) ) );
        CPPUNIT_ASSERT( ( SN::SN_Double(8.5) * SN::SN_Double(4.5) ).Equivalent( SN::Multiply(SN::SN_Double(8.5))(SN::SN_Double(4.5)) ) );
        CPPUNIT_ASSERT( ( SN::SN_Double(8.5) / SN::SN_Double(4.5) ).Equivalent( SN::Divide(SN::SN_Double(8.5))(SN::SN_Double(4.5)) ) );
    }
}

void 
TestSN::TestDoubleEvaluate()
{
    {
        PGC_Transaction transaction;
        
        CPPUNIT_ASSERT( (SN::SN_Double(3.3) == SN::SN_Double(3.3)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( !(SN::SN_Double(5.43) == SN::SN_Double(6)).Evaluate().GetBool() );
        
        SN::SN_DECLARE_VALUE( z, SN::SN_Double(2.2) );
        CPPUNIT_ASSERT( (SN::SN_Double(3.5) + SN::SN_Double(5.6) == SN::SN_Double(9.1)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::SN_Double(2.2) == z).Evaluate().GetBool() );
        
        SN::SN_DECLARE_VALUE( x, SN::SN_Double(3) );
        CPPUNIT_ASSERT( (z * x == SN::SN_Double(6.6)).Evaluate().GetBool() );
        
        SN::SN_DECLARE_VALUE( y, SN::SN_Double(2) );
        CPPUNIT_ASSERT( ( (z * x)/y == SN::SN_Double(3.3)).Evaluate().GetBool() );
    }
}

void 
TestSN::TestDoublePartialEvaluate()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( y );
        SN::SN_DECLARE( x );
        SN::SN_DECLARE_VALUE( z, SN::SN_Double(3.1415) );
        
        
        CPPUNIT_ASSERT( (SN::SN_Double(3.1415) == y).PartialEvaluate().Equivalent(SN::SN_Double(3.1415) == y) );
        CPPUNIT_ASSERT( !(SN::SN_Double(9.7) == y).PartialEvaluate().Equivalent(SN::SN_Double(9.7) == x) );
        CPPUNIT_ASSERT( (SN::SN_Double(9.7) == SN::SN_Double(9.7)).PartialEvaluate().Equivalent(SN::SN_Bool(true)) );
        CPPUNIT_ASSERT( (SN::SN_Double(3.1415) == z).PartialEvaluate().GetBool() );
        
        CPPUNIT_ASSERT( (SN::SN_Double(5.5) + y == SN::SN_Double(10)).PartialEvaluate().Equivalent(SN::Equals(SN::Add(SN::SN_Double(5.5))(y) )( SN::SN_Double(10) ) ));
        CPPUNIT_ASSERT( (SN::SN_Double(8.0) + z == SN::SN_Double(11.1415)).PartialEvaluate().GetBool() );
        
        CPPUNIT_ASSERT( (SN::SN_Double(3.1415) + z == SN::SN_Double(3.1415)*SN::SN_Double(2.0) ).PartialEvaluate().GetBool() );
        CPPUNIT_ASSERT( (SN::SN_Double(4.5) + 4.5 == 9.0).PartialEvaluate().GetBool() );
    }
}

void
TestSN::TestDoubleAssert()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( y );
        
        CPPUNIT_ASSERT( (SN::SN_Double(45.6) == y ).Assert() );
        CPPUNIT_ASSERT( (SN::SN_Double(45.6) == y).Evaluate().GetBool() );
        
        SN::SN_DECLARE( z );
        
        CPPUNIT_ASSERT( (SN::SN_Double(45.6) + z == 100.0).Assert() );
        
        CPPUNIT_ASSERT( (SN::SN_Double(54.4) == z).Evaluate().GetBool() );
    }
}

void
TestSN::TestLongEvaluate()
{
    {
        PGC_Transaction transaction;
        
        CPPUNIT_ASSERT( ( SN::SN_Long(4) * SN::SN_Double(1.25) == SN::SN_Double(5) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( SN::SN_Long(5)/SN::SN_Long(4) == SN::SN_Double(1.25) ).Evaluate().GetBool() );
    }
}

void
TestSN::TestLongAssert()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( x );
        SN::SN_DECLARE( y );
        
        CPPUNIT_ASSERT( ( x * SN::SN_Double(1.25) == SN::SN_Double(5) ).Assert() );
        CPPUNIT_ASSERT( ( y / SN::SN_Long(4) == SN::SN_Double(1.25) ).Assert() );
        CPPUNIT_ASSERT( x.Equivalent( SN::SN_Double(4) ) );
        CPPUNIT_ASSERT( y.Equivalent( SN::SN_Double(5) ) );
    }
}

void
TestSN::TestFunctionDefinition()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( RemovePrefix );
        SN::SN_DECLARE( p );
        SN::SN_DECLARE( x );
        
        CPPUNIT_ASSERT( (SN::SN_Define(RemovePrefix) == SN::SN_Lambda(p, SN::SN_Lambda(x, x.SubtractLeft(p) ) ) ).PartialAssert() );
        CPPUNIT_ASSERT( RemovePrefix.DisplaySN() == "RemovePrefix [= @p.@x.SubtractLeft x p =]");
        std::cout << std::endl << "Variable " << RemovePrefix.DisplaySN() << std::endl;
        CPPUNIT_ASSERT( RemovePrefix.PartialEvaluate().Equivalent( SN::SN_Lambda(p, SN::SN_Lambda(x, x.SubtractLeft(p) ) ).PartialEvaluate() ) );
        CPPUNIT_ASSERT( (RemovePrefix(SN::SN_String("Atl"))(SN::SN_String("AtlDog")) == SN::SN_String("Dog") ).Evaluate().GetBool() );
    }
}

void
TestSN::TestRecursiveFunctionDefinition()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( Factorial );
        SN::SN_DECLARE( n );
        
        CPPUNIT_ASSERT( (SN::SN_Define(Factorial) == SN::SN_Lambda(n, (n == SN::SN_Long(0)).If(SN::SN_Long(1), n * Factorial(n-SN::SN_Long(1))))).PartialAssert() );
        std::cout << std::endl << "Function " << Factorial.DisplaySN() << std::endl;
        CPPUNIT_ASSERT( (Factorial(SN::SN_Long(3)) == SN::SN_Long(6)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (Factorial(SN::SN_Long(10)) == SN::SN_Long(3628800)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (Factorial(SN::SN_Long(12)) == SN::SN_Long(479001600)).Evaluate().GetBool() );        
        
        SN::SN_DECLARE( Fact );
        SN::SN_DECLARE( m );
        
        CPPUNIT_ASSERT( (SN::SN_Define(Fact)(m) == (m == SN::SN_Long(0)).If(SN::SN_Long(1), m * Fact(m-SN::SN_Long(1)))).PartialAssert() );
        std::cout << std::endl << "Function " << Fact.DisplaySN() << std::endl;
        CPPUNIT_ASSERT( (Fact(SN::SN_Long(3)) == SN::SN_Long(6)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (Fact(SN::SN_Long(10)) == SN::SN_Long(3628800)).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (Fact(SN::SN_Long(12)) == SN::SN_Long(479001600)).Evaluate().GetBool() );
    }
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( Fact );
        SN::SN_DECLARE( k );
        SN::SN_DECLARE( n );
        // Fact 0 == 1 && Fact k : > 0 == k * Fact k-1 : < 0
        CPPUNIT_ASSERT( ( ( Fact(SN::SN_Long(0)) == SN::SN_Long(1) ) && ( Fact(k, SN::SN_Lambda(n, n > SN::SN_Long(0))) == k * Fact(k-SN::SN_Long(1), SN::SN_Lambda(n, n < SN::SN_Long(0))) ) ).PartialAssert() );
    }
}

void
TestSN::TestStringRefDefinition()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( x );
        SN::SN_DECLARE( y );
        CPPUNIT_ASSERT((SN::SN_String("dogcat") == x + y).Assert());
        CPPUNIT_ASSERT((x == "dog").Assert());
        CPPUNIT_ASSERT( y.Evaluate().Equivalent(SN::SN_String("cat")));
        
        SN::SN_DECLARE( a );
        SN::SN_DECLARE( b );
        SN::SN_DECLARE( c );
        SN::SN_DECLARE( d );
        CPPUNIT_ASSERT((SN::SN_String("ratdogcat") == a + b).Assert());
        CPPUNIT_ASSERT((a == d + c).Assert());
        CPPUNIT_ASSERT((d == SN::SN_String("rat")).Assert());
        CPPUNIT_ASSERT((c == SN::SN_String("dog")).Assert());
        CPPUNIT_ASSERT( b.Evaluate().Equivalent(SN::SN_String("cat")));
    }
}

void
TestSN::TestParsingExamples()
{
/*
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( B_Digit );
        SN::SN_DECLARE( d );

        CPPUNIT_ASSERT( ( SN::SN_Define(B_Digit)(d) == (d == SN::SN_String("0")) ).PartialAssert() );

        std::cout << std::endl << "Function " << B_Digit.DisplaySN() << std::endl;
        CPPUNIT_ASSERT( (  B_Digit( SN::SN_String("0") ).Assert() ) );
    }
*/
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( B_Digit );
        SN::SN_DECLARE( d );

        CPPUNIT_ASSERT( ( SN::SN_Define(B_Digit)(d) == (d == (  SN::SN_String("0") || SN::SN_String("1") || SN::SN_String("2") || SN::SN_String("3") ||SN::SN_String("4")
                                             || SN::SN_String("5") || SN::SN_String("6") || SN::SN_String("7") || SN::SN_String("8") ||SN::SN_String("9") ) ) ).PartialAssert() );

        std::cout << std::endl << "Function " << B_Digit.DisplaySN() << std::endl;
        CPPUNIT_ASSERT( (  B_Digit( SN::SN_String("0") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit( SN::SN_String("1") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit( SN::SN_String("2") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit( SN::SN_String("3") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit( SN::SN_String("4") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit( SN::SN_String("5") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit( SN::SN_String("6") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit( SN::SN_String("7") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit( SN::SN_String("8") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit( SN::SN_String("9") ).Assert() ) );

        CPPUNIT_ASSERT( !(  B_Digit( SN::SN_String("X") ).Assert() ) );

/*
        // These return a value set of bools.  Should this reduce to a boolean value???

        CPPUNIT_ASSERT( ( B_Digit( SN::SN_String("0") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit( SN::SN_String("1") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit( SN::SN_String("2") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit( SN::SN_String("3") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit( SN::SN_String("4") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit( SN::SN_String("5") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit( SN::SN_String("6") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit( SN::SN_String("7") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit( SN::SN_String("8") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit( SN::SN_String("9") ) ).Evaluate().GetBool() );

        CPPUNIT_ASSERT( ( !B_Digit( SN::SN_String("X") ) ).Evaluate().GetBool() );
*/
    }

    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( B_Digit2 );
        SN::SN_DECLARE( d );

        CPPUNIT_ASSERT( ( SN::SN_Define(B_Digit2)(d) == ((  SN::SN_String("0") || SN::SN_String("1") || SN::SN_String("2") || SN::SN_String("3") ||SN::SN_String("4")
                                             || SN::SN_String("5") || SN::SN_String("6") || SN::SN_String("7") || SN::SN_String("8") ||SN::SN_String("9") ).BuildSet().In(d) ) ).PartialAssert() );

        std::cout << std::endl << "Function " << B_Digit2.DisplaySN() << std::endl;
        CPPUNIT_ASSERT( (  B_Digit2( SN::SN_String("0") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit2( SN::SN_String("1") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit2( SN::SN_String("2") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit2( SN::SN_String("3") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit2( SN::SN_String("4") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit2( SN::SN_String("5") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit2( SN::SN_String("6") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit2( SN::SN_String("7") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit2( SN::SN_String("8") ).Assert() ) );
        CPPUNIT_ASSERT( (  B_Digit2( SN::SN_String("9") ).Assert() ) );

        CPPUNIT_ASSERT( !(  B_Digit2( SN::SN_String("X") ).Assert() ) );

        CPPUNIT_ASSERT( ( B_Digit2( SN::SN_String("0") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit2( SN::SN_String("1") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit2( SN::SN_String("2") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit2( SN::SN_String("3") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit2( SN::SN_String("4") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit2( SN::SN_String("5") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit2( SN::SN_String("6") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit2( SN::SN_String("7") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit2( SN::SN_String("8") ) ).Evaluate().GetBool() );
        CPPUNIT_ASSERT( ( B_Digit2( SN::SN_String("9") ) ).Evaluate().GetBool() );

        CPPUNIT_ASSERT( ( !B_Digit2( SN::SN_String("X") ) ).Evaluate().GetBool() );
    }
}

void
TestSN::TestParser()
{
}

void
TestSN::TestMetaEvaluate()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE_VALUE( dog, SN::SN_String("Vincent ") );
        SN::SN_DECLARE_VALUE( cat, SN::SN_String("Emma ") );
        
        CPPUNIT_ASSERT( SN::SN_Meta(1, SN::SN_String("Vincent ") + SN::SN_String("Emma ")).Evaluate().Equivalent( SN::SN_Meta(1, SN::SN_Function(SN::SN_Function(SN::Add, SN::SN_String("Vincent ")), SN::SN_String("Emma ")) ) ) );
        CPPUNIT_ASSERT( SN::SN_Meta(1, dog + cat).Evaluate().Equivalent( SN::SN_Meta(1, SN::SN_Function(SN::SN_Function(SN::Add, dog), cat) ) ) );
        
        std::cout << std::endl << SN::SN_Meta(1, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)).Evaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(1, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)).Evaluate().Equivalent( SN::SN_Meta(1, SN::SN_String("Vincent ") + SN::SN_String("Emma ") ) ) );
        
        std::cout << std::endl << SN::SN_Meta(-1, SN::SN_Meta(1, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)).Evaluate()).Evaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(-1, SN::SN_Meta(1, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)).Evaluate()).Evaluate().Equivalent( SN::SN_String("Vincent Emma ") ) );
        
        std::cout << std::endl  << SN::SN_Meta(1, SN::SN_Meta(-1, dog)).Evaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(1, SN::SN_Meta(-1, dog)).Evaluate().Equivalent( SN::SN_Meta(1, dog.Evaluate() ) ) );
        
        SN::SN_DECLARE_VALUE( e, SN::SN_Meta(2, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)) );
        
        std::cout << std::endl  << e.Evaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( e.Evaluate().Equivalent( SN::SN_Meta(2, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)) ) );
        
        std::cout << std::endl  << SN::SN_Meta(-1, e).Evaluate().DisplaySN() << std::endl;
        
        
        CPPUNIT_ASSERT( SN::SN_Meta(-1, e).Evaluate().Equivalent( SN::SN_Meta(1, SN::SN_String("Vincent ") + SN::SN_String("Emma ")) ) );
        CPPUNIT_ASSERT( e.Evaluate(-1).Equivalent( SN::SN_Meta(1, SN::SN_String("Vincent ") + SN::SN_String("Emma ")) ) );
        
        std::cout << std::endl  << SN::SN_Meta(-1, SN::SN_Meta(-1, e).Evaluate()).Evaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(-1, SN::SN_Meta(-1, e).Evaluate()).Evaluate().Equivalent( SN::SN_String("Vincent Emma ") ) );
        CPPUNIT_ASSERT( e.Evaluate(-1).Evaluate(-1).Equivalent( SN::SN_String("Vincent Emma ") ) );
        
        CPPUNIT_ASSERT( SN::SN_Meta(1, SN::SN_Meta(-1, dog)).Evaluate().Equivalent(  SN::SN_Meta(1, dog.Evaluate()) ) );
    }
}

void
TestSN::TestMetaPartialEvaluateOnValues()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE_VALUE( dog, SN::SN_String("Vincent ") );
        SN::SN_DECLARE_VALUE( cat, SN::SN_String("Emma ") );
        
        CPPUNIT_ASSERT( SN::SN_Meta(1, SN::SN_String("Vincent ") + SN::SN_String("Emma ")).PartialEvaluate().Equivalent( SN::SN_Meta(1, SN::SN_Function(SN::SN_Function(SN::Add, SN::SN_String("Vincent ")), SN::SN_String("Emma ")) ) ) );
        
        std::cout << std::endl << SN::SN_Meta(1, dog + cat).PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(1, dog + cat).PartialEvaluate().Equivalent( SN::SN_Meta(1, SN::SN_Function(SN::SN_Function(SN::Add, dog), cat) ) ) );
        
        std::cout << std::endl << SN::SN_Meta(1, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)).PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(1, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)).PartialEvaluate().Equivalent( SN::SN_Meta(1, SN::SN_String("Vincent ") + SN::SN_String("Emma ") ) ) );
        
        std::cout << std::endl << SN::SN_Meta(-1, SN::SN_Meta(1, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)).PartialEvaluate()).PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(-1, SN::SN_Meta(1, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)).PartialEvaluate()).PartialEvaluate().Equivalent( SN::SN_String("Vincent Emma ") ) );
        
        std::cout << std::endl  << SN::SN_Meta(1, SN::SN_Meta(-1, dog)).PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(1, SN::SN_Meta(-1, dog)).PartialEvaluate().Equivalent( SN::SN_Meta(1, dog.PartialEvaluate() ) ) );
        
        SN::SN_DECLARE_VALUE( e, SN::SN_Meta(2, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)) );
        
        std::cout << std::endl  << e.PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( e.PartialEvaluate().Equivalent( SN::SN_Meta(2, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)) ) );
        
        std::cout << std::endl  << SN::SN_Meta(-1, e).PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(-1, e).PartialEvaluate().Equivalent( SN::SN_Meta(1, SN::SN_String("Vincent ") + SN::SN_String("Emma ")) ) );
        CPPUNIT_ASSERT( e.PartialEvaluate(-1).Equivalent( SN::SN_Meta(1, SN::SN_String("Vincent ") + SN::SN_String("Emma ")) ) );
        
        std::cout << std::endl  << SN::SN_Meta(-1, SN::SN_Meta(-1, e).PartialEvaluate()).PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(-1, SN::SN_Meta(-1, e).PartialEvaluate()).PartialEvaluate().Equivalent( SN::SN_String("Vincent Emma ") ) );
        CPPUNIT_ASSERT( e.PartialEvaluate(-1).PartialEvaluate(-1).Equivalent( SN::SN_String("Vincent Emma ") ) );
    }
}

void 
TestSN::TestMetaPartialEvaluate()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( dog );
        SN::SN_DECLARE( cat );
        
        CPPUNIT_ASSERT( SN::SN_Meta(1, SN::SN_String("Vincent ") + SN::SN_String("Emma ")).PartialEvaluate().Equivalent( SN::SN_Meta(1, SN::SN_Function(SN::SN_Function(SN::Add, SN::SN_String("Vincent ")), SN::SN_String("Emma ")) ) ) );
        
        std::cout << std::endl << SN::SN_Meta(1, dog + cat).PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(1, dog + cat).PartialEvaluate().Equivalent( SN::SN_Meta(1, SN::SN_Function(SN::SN_Function(SN::Add, dog), cat) ) ) );
        
        std::cout << std::endl << SN::SN_Meta(1, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)).PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(1, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)).PartialEvaluate().Equivalent( SN::SN_Meta(1, dog + cat ) ) );
        
        std::cout << std::endl << SN::SN_Meta(-1, SN::SN_Meta(1, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)).PartialEvaluate()).PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(-1, SN::SN_Meta(1, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)).PartialEvaluate()).PartialEvaluate().Equivalent( dog + cat ) );
        
        std::cout << std::endl  << SN::SN_Meta(1, SN::SN_Meta(-1, dog)).PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(1, SN::SN_Meta(-1, dog)).PartialEvaluate().Equivalent( SN::SN_Meta(1, dog.PartialEvaluate() ) ) );
        
        SN::SN_DECLARE_VALUE( e, SN::SN_Meta(2, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)) );
        
        std::cout << std::endl  << e.PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( e.PartialEvaluate().Equivalent( SN::SN_Meta(2, SN::SN_Meta(-1, dog) + SN::SN_Meta(-1, cat)) ) );
        
        std::cout << std::endl  << SN::SN_Meta(-1, e).PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(-1, e).PartialEvaluate().Equivalent( SN::SN_Meta(1, dog + cat) ) );
        CPPUNIT_ASSERT( e.PartialEvaluate(-1).Equivalent( SN::SN_Meta(1, dog + cat) ) );
        
        std::cout << std::endl  << SN::SN_Meta(-1, SN::SN_Meta(-1, e).PartialEvaluate()).PartialEvaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( SN::SN_Meta(-1, SN::SN_Meta(-1, e).PartialEvaluate()).PartialEvaluate().Equivalent( dog + cat ) );
        CPPUNIT_ASSERT( e.PartialEvaluate(-1).PartialEvaluate(-1).Equivalent( dog + cat ) );
    }
}

void
TestSN::TestValueSetEvaluate()
{
    {
        PGC_Transaction transaction;
        
        // || operator applied to values other than bool, creates a value set.
        // ValueSet has Constructors for up to 5 parameters.  This will be mainly for testing. 
        CPPUNIT_ASSERT( (SN::SN_Long(5) || SN::SN_Long(7)).Evaluate().DoRemove(SN::SN_Long(5)).DoRemove(SN::SN_Long(7)).DoIsEmpty() );
        
        CPPUNIT_ASSERT( (SN::SN_String("dog") || SN::SN_String("cat")).Evaluate().DoRemove(SN::SN_String("dog")).DoRemove(SN::SN_String("cat")).DoIsEmpty() );
        
        CPPUNIT_ASSERT( (SN::SN_Char('d') || SN::SN_Char('c')).Evaluate().DoRemove(SN::SN_Char('d')).DoRemove(SN::SN_Char('c')).DoIsEmpty() );
        
        CPPUNIT_ASSERT((((SN::SN_String("catdograt") || SN::SN_String("pigdograt")) - (SN::SN_String("dograt") || SN::SN_String("rat")))).Evaluate().DoRemove(SN::SN_String("cat")).DoRemove(SN::SN_String("pig")).DoRemove(SN::SN_String("catdog")).DoRemove(SN::SN_String("pigdog")).DoIsEmpty()  );
        
        
        CPPUNIT_ASSERT((((SN::SN_String("cat") || SN::SN_String("dog")) + (SN::SN_String("food") || SN::SN_String("awesomefood")))).Evaluate().DoRemove(SN::SN_String("catfood")).DoRemove(SN::SN_String("catawesomefood")).DoRemove(SN::SN_String("dogfood")).DoRemove(SN::SN_String("dogawesomefood")).DoIsEmpty()  );
        
        CPPUNIT_ASSERT( (SN::SN_Double(5) || SN::SN_Double(7)).Evaluate().DoRemove(SN::SN_Double(5)).DoRemove(SN::SN_Double(7)).DoIsEmpty() );
        
        CPPUNIT_ASSERT( ( ((SN::SN_Double(5) || SN::SN_Double(7)) + (SN::SN_Double(8) || SN::SN_Double(9))) ).Evaluate().DoRemove(SN::SN_Double(13)).DoRemove(SN::SN_Double(14)).DoRemove(SN::SN_Double(15)).DoRemove(SN::SN_Double(16)).DoIsEmpty() );
        // The order does not matter
        CPPUNIT_ASSERT( (SN::SN_Long(-4) || SN::SN_Long(4)).Evaluate().DoRemove(SN::SN_Long(4)).DoRemove(SN::SN_Long(-4)).DoIsEmpty() );
        
        // Value sets behave like values.  an operator or function applied to value set, generates the cardesian product of the values, with the function applied.
        CPPUNIT_ASSERT( ( ((SN::SN_Long(5) || SN::SN_Long(7)) + (SN::SN_Long(8) || SN::SN_Long(9))) ).Evaluate().DoRemove(SN::SN_Long(13)).DoRemove(SN::SN_Long(14)).DoRemove(SN::SN_Long(15)).DoRemove(SN::SN_Long(16)).DoIsEmpty() );
        
        //added for subtraction
        CPPUNIT_ASSERT( ( ((SN::SN_Long(9) || SN::SN_Long(11)) - (SN::SN_Long(3) || SN::SN_Long(4))) ).Evaluate().DoRemove(SN::SN_Long(6)).DoRemove(SN::SN_Long(5)).DoRemove(SN::SN_Long(8)).DoRemove(SN::SN_Long(7)).DoIsEmpty() );
        
        CPPUNIT_ASSERT( ( ((SN::SN_Long(1) || SN::SN_Long(2)) * (SN::SN_Long(3) || SN::SN_Long(4))) ).Evaluate().DoRemove(SN::SN_Long(3)).DoRemove(SN::SN_Long(4)).DoRemove(SN::SN_Long(6)).DoRemove(SN::SN_Long(8)).DoIsEmpty() );
        
        CPPUNIT_ASSERT( ( ((SN::SN_Long(5) || SN::SN_Long(10)) / (SN::SN_Long(1) || SN::SN_Long(5))) ).Evaluate().DoRemove(SN::SN_Double(5)).DoRemove(SN::SN_Double(1)).DoRemove(SN::SN_Double(10)).DoRemove(SN::SN_Double(2)).DoIsEmpty() );
        
        // If the values are tagged with worlds from different world sets then all the values from the cardesian product are created. 
        // first    2 || -2 = [2:A, -2:B]
        // second   2 || -2 = [2:C, -2:D]
        // first+second = [2+2:AC, 0:AD, 0:BC, -2:BD]
        CPPUNIT_ASSERT( (((SN::SN_Long(2) || SN::SN_Long(-2)) + (SN::SN_Long(2) || SN::SN_Long(-2)))).Evaluate().DoRemove(SN::SN_Long(4)).DoRemove(SN::SN_Long(0)).DoRemove(SN::SN_Long(-4)).DoIsEmpty() );
        
        // If using the same value set twice then must use the same value each time.
        // This is because the intersection of different worlds from the same value set is always empty.
        // More explictly, each value in a vale set is tagged with a world from a world set.
        //     2 || -2 = [2:A, -2:B]
        // where A and B are worlds. Worlds are sets.
        // A intersection B is empty.
        SN::SN_DECLARE_VALUE( two_or_minus_two, (SN::SN_Long(2) || SN::SN_Long(-2)).Evaluate() );
        
        CPPUNIT_ASSERT( (two_or_minus_two+two_or_minus_two).Evaluate().DoRemove(SN::SN_Long(4)).DoRemove(SN::SN_Long(-4)).DoIsEmpty() );
        
        CPPUNIT_ASSERT( ((SN::SN_Long(2) || SN::SN_Long(-2))+(SN::SN_Long(2) || SN::SN_Long(-2))).Evaluate().DoRemove(SN::SN_Long(4)).DoRemove(SN::SN_Long(0)).DoRemove(SN::SN_Long(-4)).DoIsEmpty() );
        
        CPPUNIT_ASSERT( (two_or_minus_two+two_or_minus_two).BuildSet().In(SN::SN_Long(-4)).Evaluate().GetBool() );
    }
}

void
TestSN::TestValueSetEquivalencyEvaluate()
{
    {
        PGC_Transaction transaction;
        
        CPPUNIT_ASSERT( (((SN::SN_String("Dog")||SN::SN_String("Cat")) + SN::SN_String("Food"))==(SN::SN_String("DogFood")||SN::SN_String("CatFood"))).Evaluate().GetBool());
        
        CPPUNIT_ASSERT( (((SN::SN_String("Dog")||SN::SN_String("Cat")) + SN::SN_String("Food"))==(SN::SN_String("CatFood")||SN::SN_String("DogFood"))).Evaluate().GetBool());
        CPPUNIT_ASSERT( (((SN::SN_String("Dog")||SN::SN_String("Pig")) + SN::SN_String("Food"))==(SN::SN_String("DogFood")||SN::SN_String("PigFood"))).Evaluate().GetBool());
        CPPUNIT_ASSERT( (((SN::SN_String("Dog")||SN::SN_String("Dog")) + SN::SN_String("Food"))==(SN::SN_String("DogFood")||SN::SN_String("PigFood"))).Evaluate().GetBool());
        
        CPPUNIT_ASSERT( (((SN::SN_Long(10)||SN::SN_Long(9)) + SN::SN_Long(1))==(SN::SN_Long(11)||SN::SN_Long(10))).Evaluate().GetBool());
        
        CPPUNIT_ASSERT( (((SN::SN_Double(10)||SN::SN_Double(9)) + SN::SN_Double(1))==(SN::SN_Double(11)||SN::SN_Double(10))).Evaluate().GetBool());
        
        
        // mixed value sets
        CPPUNIT_ASSERT( (SN::SN_String("dog") || SN::SN_Long(9)).Evaluate().DoRemove(SN::SN_String("dog")).DoRemove(SN::SN_Long(9)).DoIsEmpty() );
        
        //NESTED OR CASE
        CPPUNIT_ASSERT( ((SN::SN_String("dog") || SN::SN_Long(9)) ||SN::SN_String("Cat")).Evaluate().DoRemove(SN::SN_String("dog")).DoRemove(SN::SN_Long(9)).DoRemove(SN::SN_String("Cat")).DoIsEmpty() );
    }
}

void
TestSN::TestSetEvaluate()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_Expression a;
        CPPUNIT_ASSERT ( (((SN::SN_Long(2) || SN::SN_Long(-2))*(SN::SN_Long(3) || SN::SN_Long(-3))).BuildSet() == ((SN::SN_Long(6) || SN::SN_Long(-6)).BuildSet())).Evaluate().GetBool() );
        
        CPPUNIT_ASSERT( ((SN::SN_Long(4).SquareRoot()).BuildSet() == (SN::SN_Long(2) || SN::SN_Long(-2)).BuildSet()).Evaluate().GetBool() );
        
        CPPUNIT_ASSERT( ((SN::SN_Long(4)).BuildSet()).Evaluate().DisplaySN() );
        std::cout << std::endl  << ((SN::SN_Long(4)).BuildSet()).Evaluate().DisplaySN() << std::endl;
        
        CPPUNIT_ASSERT( ((SN::SN_Long(2) || SN::SN_Long(-2))+(SN::SN_Long(2) || SN::SN_Long(-2))).Evaluate().DoRemove(SN::SN_Long(4)).DoRemove(SN::SN_Long(0)).DoRemove(SN::SN_Long(-4)).DoIsEmpty() );
        
        SN::SN_DECLARE_VALUE( x, (SN::SN_Long(2) || SN::SN_Long(-2)).Evaluate() );
        
        CPPUNIT_ASSERT( (((x < SN::SN_Long(0)).If(x || SN::SN_Long(5), SN::SN_Long(0))).BuildSet() == (SN::SN_Long(0) || SN::SN_Long(5) || SN::SN_Long(-2)).BuildSet()).Evaluate().GetBool() ); 
    }
}

void
TestSN::TestSetAssert()
{
    {
        PGC_Transaction transaction;
        
        SN::SN_DECLARE( x );
        SN::SN_DECLARE( z );
        CPPUNIT_ASSERT( (x.Square() == SN::SN_Long(4)).Assert() );
        CPPUNIT_ASSERT( ((x.BuildSet()) == (SN::SN_Long(2) || SN::SN_Long(-2)).BuildSet()).Evaluate().GetBool() );
        CPPUNIT_ASSERT( (z == (SN::SN_Long(2) || SN::SN_Long(-2))).Assert() );
        CPPUNIT_ASSERT( (x == (SN::SN_Long(2) || SN::SN_Long(-2))).Evaluate().GetBool() );
        
        SN::SN_DECLARE( y );
        CPPUNIT_ASSERT( (y*y == SN::SN_Long(4)).Assert() );   // recognise that y*y = y.Square() in the * operator in SNI_Expression.
        CPPUNIT_ASSERT( ((y.BuildSet()) == (SN::SN_Long(2) || SN::SN_Long(-2)).BuildSet()).Evaluate().GetBool() );
    }
}
