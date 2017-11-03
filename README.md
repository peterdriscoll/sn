# Introduction
This project is a set of classes that act as a logic, learning, and meta programming system, in C++.
The goal is to have a means of manipulating and refactoring legacy code as logic.

## Logic
But SN is based on narrowing, instead of resolution. This gives it greater flexibility.
Narrowing is based on considering multiple values for a variabl at one time, stored in value sets, with each recorded value tagged against "worlds" in world sets. A world coresponds to a logical condition.
Narrowing is the process of elimintaing conflicts being worlds, as the code runs.
SN is based on relational model. Every operation is a relationship between the resul and the input parameters. Evaluation proceeds using the known values to calculate the unknown.
If the calculation cannot proceed because of unknown values or cardinality (large value sets), calls are put on a heap for delayed processing.

## Meta programming
SN is designed for meta programming. Code is regarded as data, and the Meta class allows sections of code to be wrapped so that they may acted on by other code.
SN is designed for the implementation of compilers that take existing programs and convert them into code.

# Planned Development.

## Classes/namespaces

* Is a relationships.
* Tuple of fields.

## Parser for C++ ...

Parser written in SN.

## Learning
At a later stage integrated Bayesian probability will be added to this project.
Cobinational probability wi be added by attaching probabilities to the worlds.
Bayesian probability will attaching prior probabilities to asserted facts, based on the information quantity, reliability of the developer and manual override. Bayes' law will be used to provide updated probabilities. Generalizations of facts will then be considered based on the probability calculated by Bayes' Law.

## Two way integration with legacy code.

How to leave legacy code base mainly unchanged? Updates, not rewrites.
* Learning style of legacy code.
* Record white space against objects, and chain of decisions to allow regeneration of 

## Modelling of imperative programming.

The substitution operator allows the modelling of imperative programs, and unwinding them into logic.
Explicit time based representation of data.
Implicit parameters.

# Usage
1.	Installation process
Download.
Open Visual Studio sn.sln.
New projects link to the pgc and sn dlls in the bin directory.
See unit tests for usage.

2.	Software dependencies
* Visual studio.
* STL.

3.	Latest releases
20/09/2017 - First release. 
4.	API references

## Supported OS
I had intended this to be a windows/unix project. But at present, limitations on resources make this windows only.
* Port the unit tests.

## Build and Test
test_sn and test_pgc have unit tests, run through Visual Studio Test Explorer.

## Exanple.
Link to sn.lib and pgc.lib.
Set up directories.

    #include <iostream>
    #include <string>

    #include "sn.h"
    using namespace pgc;
    using namespace skynet; // Interface namespace for SN.

    string YourCppMethod()
    {
        Transaction transaction;  // outer transaction

        SN_DECLARE(result);
        
        Manager manager(AssertErrorHandler); // Sets up any configuration parameters for SN
        {
            Transaction transaction; // PGC transaction. Ensures all memory from SN objects freed on completion.

            SN_DECLARE(a); // Variables
            SN_DECLARE(b);
            SN_DECLARE(c);
            SN_DECLARE(d);

            (String("ratdogcat") == a + b).AssertAction(); // SN creates a and b are string reference to "ratdogcat".
            (a == d + c).AssertAction(); 
            (d == String("rat")).AssertAction(); // c contines after "rat". "dog ..."
            (c == String("dog")).AssertAction(); // b is the remainder of the string. So "cat".

            Assert::IsTrue(b.Evaluate().Equivalent(String("cat")));

            (result = d).AssertAction();
        }

        return result.GetString(); // result stiil exists because created in outer transaction.
    }

    void main()
    {
        cout << "The result is " << YourCppMethod() endl;
    }
    
# Technical

## Transactional garbage collector.
This garbage collector divides up work into transactions. When a transaction ends, all the memory for that transaction is released, unless it is linked to outside the transaction.
Linked objects are promoted by copying them to the outer transactions memory space.

## Current limitations.
32 bit configurations have all tests working.
64 bit configurations under investigation.
Basic logic and math functionality is in place.
Code is being simplified for greater reliability. Using Cartesion products.

## Recent enhancements
* Cartesian products to combine values from Value Sets.
* Namespaces
* Support for short, int, long, long long, float, double, and long double.

## Tasks.
* Fix 64 bit unit tests.
* Continued simplification of engine.
* Assert based evaluation.
* Work on the garbage collection.

# Contribute
Please contact me at peter.driscoll59@gmail.com.
I would be happy to have other conscientous developers contribute to this project