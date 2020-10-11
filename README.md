# Introduction
This project is a set of classes that act as a logic, learning, and meta programming system, in C++.

## Logic
But SN is based on narrowing, instead of resolution. This gives it greater flexibility.
Narrowing is based on considering multiple values for a variabl at one time, stored in value sets, with each recorded value tagged against "worlds" in world sets. A world coresponds to a logical condition.
Narrowing is the process of elimintaing conflicts being worlds, as the code runs.
SN is based on relational model. Every operation is a relationship between the resul and the input parameters. Evaluation proceeds using the known values to calculate the unknown.
If the calculation cannot proceed because of unknown values or cardinality (large value sets), calls are put on a heap for delayed processing.

## Meta programming
SN is designed for meta programming. Code is regarded as data, and the Meta class allows sections of code to be wrapped so that they may acted on by other code.
SN is designed for the implementation of compilers that take existing programs and convert them into code.

## Transactional garbage collector.

This garbage collector divides up work into transactions. When a transaction ends, all the memory for that transaction is released, unless it is linked to outside the transaction.
Linked objects are promoted by copying them to the outer transactions memory space.

# Setup
It is a visual studio solution.

## Requirements

A computer with a reasonable amount of grunt and about 20 GB of space.

You'll need Visual Studio which you can get from here,
* https://visualstudio.microsoft.com/downloads/

Also the bitbucket extension,
* https://marketplace.visualstudio.com/items?itemName=MistyK.VisualStudioBitbucketExtension

Also boost which you can find here. I am currently using 1.72.
* https://www.boost.org/    

Unzip to
* C:\boost\boost_1_72_0

## Cloning the project

Create a free bitbucket account.

To download the project go into Team Explorer, and login to bitbucket.
Then under local GIT repositories click Clone. Enter
* https://bitbucket.org/peterdriscoll59/skynet2
in the top box and click clone.

# Usage

At the moment it is run by the regression tests. After compiling the solution you should see the tests in Test Explorer.

The projects are,
* sn - The main library.
* sn_lib - Parser stuff.
* pgc - Garbage collector library.
* test_sn - Tests for skynet.
* test_pgc - Tests for the garbage collector.
* http_server_lib - Http server library for the debugger interface.

## Debugger

Update the constant,
  const string doc_root = "C:/Users/peter_driscoll/Documents/Source/Repos/skynet2/html";
To the HTML directory in your skynet2 repository.

To run the debugger, update,
  bool runWebServer = false;
to
  bool runWebServer = true;  

Then, when you run a unit test it will open a browser page, which
allows you to step through the execution at an abstract level.

# Planned Development.

## Parser for C++ ...

Parser written in SN under development.

## Exanple standalone project.

See the Ex1StringRef project.
