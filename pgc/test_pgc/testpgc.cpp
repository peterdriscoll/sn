#include "testpgc.h"
#include "cppunit/nova_extensions/hacksforcppunitstdnamespace.h"
#include "cppunit/TextOutputter.h"
#include "pgc.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TestPGC);

void 
TestPGC::setUp()
{
    PGC_Transaction::ResetNetMemoryUsed();
    PGC_Transaction::ResetGrossMemoryUsed();
}


void 
TestPGC::tearDown()
{
    CPPUNIT_ASSERT( PGC_Transaction::TotalNetMemoryUsed() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionUsedMemory() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionFreeMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
}

//  General test of the Promotional Garbage Collector memory management.
//  Test multi level promotion.
void
TestPGC::TestBasicPGC()
{	
    {
        PGC_Transaction parentTransaction;
        SRef<TestPgc_B> b = new TestPgc_B();
        
        CPPUNIT_ASSERT( parentTransaction.NetMemoryUsed() == sizeof(TestPgc_B) - OVERHEAD );
        CPPUNIT_ASSERT( parentTransaction.NetMemoryUsed() == PGC_Transaction::TotalNetMemoryUsed() );
        CPPUNIT_ASSERT( parentTransaction.GrossMemoryUsed() + PGC_Promotion::PromotionUsedMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
        
        {
            PGC_Transaction transaction;
            SRef<TestPgc_A> a;
            a = new TestPgc_A();
            a->SetDescription("a");
            b->SetTestA((TestPgc_A *) a);
            SRef<TestPgc_A> a1 = new TestPgc_A();
            a1->SetDescription("a1");
            a->SetNext(a1);
            SRef<TestPgc_A> a2 = new TestPgc_A();
            a2->SetDescription("a2");
            a1->SetNext(a2);
            SRef<TestPgc_A> a3 = new TestPgc_A();
            a3->SetDescription("a3");
            a2->SetNext(a3);
            
            CPPUNIT_ASSERT( parentTransaction.NetMemoryUsed() + transaction.NetMemoryUsed() == PGC_Transaction::TotalNetMemoryUsed() );
            CPPUNIT_ASSERT( parentTransaction.GrossMemoryUsed() + transaction.GrossMemoryUsed() + PGC_Promotion::PromotionUsedMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
            
            CPPUNIT_ASSERT( TestPgc_A::m_ActiveCount == 4);
            CPPUNIT_ASSERT( TestPgc_B::m_ActiveCount == 1);
            
            {
                PGC_Transaction childTransaction;

                SRef<TestPgc_A> c;
                c = new TestPgc_A();
                c->SetDescription("c");
                SRef<TestPgc_A> c1 = new TestPgc_A();
                c1->SetDescription("c1");
                c->SetNext(c1);
                SRef<TestPgc_A> c2 = new TestPgc_A();
                c2->SetDescription("c2");
                c1->SetNext(c2);
                SRef<TestPgc_A> c3 = new TestPgc_A();
                c3->SetDescription("c3");
                c2->SetNext(c3);
                
                a3->SetNext(c2);
                
                CPPUNIT_ASSERT( TestPgc_A::m_ActiveCount == 8);
                CPPUNIT_ASSERT( TestPgc_B::m_ActiveCount == 1);
                
                CPPUNIT_ASSERT( parentTransaction.NetMemoryUsed() + transaction.NetMemoryUsed() + childTransaction.NetMemoryUsed() == PGC_Transaction::TotalNetMemoryUsed() );
                CPPUNIT_ASSERT( parentTransaction.GrossMemoryUsed() + transaction.GrossMemoryUsed() + childTransaction.GrossMemoryUsed() + PGC_Promotion::PromotionUsedMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
            }
            
            CPPUNIT_ASSERT( TestPgc_A::m_ActiveCount == 6);
            CPPUNIT_ASSERT( TestPgc_B::m_ActiveCount == 1);
            
            b->SetTestA(a3->GetNext());
            a3->SetNext(0);
            b->SetTestA((TestPgc_A *) a);
            
            CPPUNIT_ASSERT( parentTransaction.NetMemoryUsed() + transaction.NetMemoryUsed() == PGC_Transaction::TotalNetMemoryUsed() );
            CPPUNIT_ASSERT( parentTransaction.GrossMemoryUsed() + transaction.GrossMemoryUsed() + PGC_Promotion::PromotionUsedMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
            // Promotion occurs in the destructor of the transaction here.
        }
        
        CPPUNIT_ASSERT( parentTransaction.NetMemoryUsed() == PGC_Transaction::TotalNetMemoryUsed() );
        CPPUNIT_ASSERT( parentTransaction.GrossMemoryUsed() + PGC_Promotion::PromotionFreeMemory() + PGC_Promotion::PromotionUsedMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
        
        SRef<TestPgc_A> xa = b->GetTestA();
        CPPUNIT_ASSERT( xa->GetDescription() == "a");
        SRef<TestPgc_A> xa1 = xa->GetNext();
        CPPUNIT_ASSERT( xa1->GetDescription() == "a1");
        SRef<TestPgc_A> xa2 = xa1->GetNext();
        CPPUNIT_ASSERT( xa2->GetDescription() == "a2");
        SRef<TestPgc_A> xa3 = xa2->GetNext();
        CPPUNIT_ASSERT( xa3->GetDescription() == "a3");
        CPPUNIT_ASSERT( xa3->GetNext() == 0);
        
        CPPUNIT_ASSERT( TestPgc_A::m_ActiveCount == 4);
        CPPUNIT_ASSERT( TestPgc_B::m_ActiveCount == 1);
        
        // All destructors called here in the destructor of the transaction.
    }
    
    CPPUNIT_ASSERT( PGC_Transaction::TotalNetMemoryUsed() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionUsedMemory() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionFreeMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
    
    CPPUNIT_ASSERT( TestPgc_A::m_ActiveCount == 0);  //  All destructors called.
    CPPUNIT_ASSERT( TestPgc_B::m_ActiveCount == 0);
}

//  Test automatic transaction creation.  No promotion.
//  * StartStackTransaction
//  * EndStackTransaction
void
TestPGC::TestRecursivePGC()
{
    {
        PGC_Transaction transaction;
        {
            PGC_Transaction::StartStackTransaction();
            {
                PGC_Transaction transaction;
                RecursivelyDoThis(100);
            }
            PGC_Transaction::EndStackTransaction();
        }
    }
    CPPUNIT_ASSERT( PGC_Transaction::TotalNetMemoryUsed() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionUsedMemory() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionFreeMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
    
    CPPUNIT_ASSERT( TestPgc_A::m_ActiveCount == 0);  //  All destructors called.
    CPPUNIT_ASSERT( TestPgc_B::m_ActiveCount == 0);
}

//  Test automatic transaction creation with promotion.
void
TestPGC::TestRecursiveKeepPGC()
{
    CPPUNIT_ASSERT( PGC_Promotion::PromotionUsedMemory() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionFreeMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
    {
        PGC_Transaction parentTransaction;
        long depth = 1000;
        SRef<TestPgc_A> keep = new TestPgc_A();
        {
            PGC_Transaction transaction;
            RecursivelyDoKeep(keep, depth);
            CPPUNIT_ASSERT( PGC_Transaction::TotalNetMemoryUsed() == (depth + 1) * (sizeof(TestPgc_A) - OVERHEAD) + depth * (sizeof(TestPgc_A) - OVERHEAD) + depth * (sizeof(TestPgc_B) - OVERHEAD) );
        }
        long countKeep = keep->CountList();
        CPPUNIT_ASSERT( countKeep == depth );
        CPPUNIT_ASSERT( PGC_Transaction::TotalNetMemoryUsed() == (countKeep + 1) * (sizeof(TestPgc_A) - OVERHEAD) );
        
        CPPUNIT_ASSERT( TestPgc_A::m_ActiveCount == countKeep+1);
        CPPUNIT_ASSERT( TestPgc_B::m_ActiveCount == 0);
    }
    CPPUNIT_ASSERT( PGC_Transaction::TotalNetMemoryUsed() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionUsedMemory() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionFreeMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
    
    CPPUNIT_ASSERT( TestPgc_A::m_ActiveCount == 0);  //  All destructors called.
    CPPUNIT_ASSERT( TestPgc_B::m_ActiveCount == 0);
}

void TestPGC::TestReferenceCyclePGC()
{
    size_t stackTramsactionSize = sizeof(PGC_StackTransaction);
    CPPUNIT_ASSERT( stackTramsactionSize == 1 );
    
    CPPUNIT_ASSERT( PGC_Promotion::PromotionUsedMemory() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionFreeMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
    {
        PGC_Transaction parentTransaction;
        SRef<TestPgc_A> a = new TestPgc_A();
        a->SetDescription("TestPgc_A a");
        {
            PGC_Transaction transaction;
            TestPgc_A *b = new TestPgc_A();
            a->SetNext(b);
            TestPgc_A *c = new TestPgc_A();
            b->SetNext(c);
            b->SetDescription("TestPgc_A b");
            c->SetNext(a);
            c->SetDescription("TestPgc_A c");
        }
        
        CPPUNIT_ASSERT( a->GetDescription() == "TestPgc_A a");
        CPPUNIT_ASSERT( a->GetNext()->GetDescription() == "TestPgc_A b");
        CPPUNIT_ASSERT( a->GetNext()->GetNext()->GetDescription() == "TestPgc_A c");
        
        CPPUNIT_ASSERT( a->GetNext()->GetNext()->GetNext()->GetDescription() == "TestPgc_A a");
        CPPUNIT_ASSERT( a->GetNext()->GetNext()->GetNext()->GetNext()->GetDescription() == "TestPgc_A b");
        CPPUNIT_ASSERT( a->GetNext()->GetNext()->GetNext()->GetNext()->GetNext()->GetDescription() == "TestPgc_A c");
        
        CPPUNIT_ASSERT( PGC_Transaction::TotalNetMemoryUsed() == 3 * (sizeof(TestPgc_A) - OVERHEAD) );
        
        CPPUNIT_ASSERT( TestPgc_A::m_ActiveCount == 3);
        CPPUNIT_ASSERT( TestPgc_B::m_ActiveCount == 0);
    }
    CPPUNIT_ASSERT( PGC_Transaction::TotalNetMemoryUsed() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionUsedMemory() == 0 );
    CPPUNIT_ASSERT( PGC_Promotion::PromotionFreeMemory() == PGC_Transaction::TotalGrossMemoryUsed() );
    
    CPPUNIT_ASSERT( TestPgc_A::m_ActiveCount == 0);  //  All destructors called.
    CPPUNIT_ASSERT( TestPgc_B::m_ActiveCount == 0);

    // Close the transaction setup by the static variables.
    delete PGC_Transaction::TopTransaction();
}

