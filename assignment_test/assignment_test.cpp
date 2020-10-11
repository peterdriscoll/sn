// assignment_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

using namespace std;

class expression
{
public:
	expression()
	{
	}
	
	virtual string Text() const = 0;
};

class assignment * m_global;

class assignment : public expression
{
private:
	string m_Text;

public:
	assignment(const string &p_Text)
		: m_Text(p_Text)
	{}

	expression &operator=(const string &p_Text)
	{
		m_Text = p_Text;
		return *new assignment("retriever");
	}

	string Text() const
	{
		return m_Text;
	}
};

int main()
{
	m_global = new assignment("cat");

	assignment test("bird");

	string text = (test = "dog").Text();
	string text2 = test.Text();
	std::cout << "Hello World! " << text << " " << text2 << "\n";
}
