#include <iostream>
#include <string>
#include <conio.h>

#include "sn.h"
using namespace PGCX;
using namespace skynet; // Interface namespace for SN.

string YourCppFunction()
{
	SN_DECLARE(result);

	{
		//Transaction transaction; // PGC transaction. Ensures all memory from SN objects freed on completion.

		SN_DECLARE(a); // Variables
		SN_DECLARE(b);
		SN_DECLARE(c);
		SN_DECLARE(d);

		(String("ratdogcat") == a + b).AssertAction(); // SN creates a and b are string reference to "ratdogcat".
		(a == d + c).AssertAction();
		(d == String("rat")).AssertAction(); // c contines after "rat". "dog ..."
		(c == String("dog")).AssertAction(); // b is the remainder of the string. So "cat".

		(b == String("cat")).EvaluateAction();

		(result == b).AssertAction();
	}

	return result.GetString(); // result stiil exists because created in outer transaction.
}

void main(int argc, char *argv[])
{
	Manager manager; // Sets up any configuration parameters for SN

	_getch();
	Transaction transaction;
	try
	{
		cout << "The result is " << YourCppFunction() << endl;
	}
	catch (Error e)
	{
		string description = e.GetDescription();
		cout << "Error: " + description;
	}
}
