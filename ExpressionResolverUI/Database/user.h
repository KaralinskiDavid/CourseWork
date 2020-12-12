#pragma once

#include<string>

using namespace std;

class User {
public:
	User(wstring _login, size_t _passwordHash)
	{
		login = _login;
		passwordHash = _passwordHash;
	}

	wstring login;
	size_t passwordHash;
};

