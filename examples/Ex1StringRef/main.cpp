#include <iostream>
#include <string>
#include <conio.h>

#include "snl.h"
using namespace PGCX;
using namespace skynet; // Interface namespace for SN.

std::string YourCppFunction()
{
	SN_DECLARE(result);

	{
		//Transaction transaction; // PGC transaction. Ensures all memory from SN objects freed on completion.

		SN_DECLARE(a); // Variables
		SN_DECLARE(b);
		SN_DECLARE(c);
		SN_DECLARE(d);

		(String("ratdogcat") == a + b).Assert().Do(); // SN creates a and b are string reference to "ratdogcat".
		(a == d + c).Assert().Do();
		(d == String("rat")).Assert().Do(); // c contines after "rat". "dog ..."
		(c == String("dog")).Assert().Do(); // b is the remainder of the string. So "cat".

		(b == String("cat")).Evaluate().Do();

		(result == b).Assert().Do();
	}

	return result.GetString(); // result stiil exists because created in outer transaction.
}

int main(int argc, char *argv[])
{
	Manager manager; // Sets up any configuration parameters for SN

	int c = _getch();
	Transaction transaction;
	try
	{
		std::cout << "The result is " << YourCppFunction() << std::endl;
	}
	catch (Error e)
	{
		std::string description = e.GetDescription();
		std::cout << "Error: " + description;
	}
}
