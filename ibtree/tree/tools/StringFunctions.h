#ifndef StringFunctions_h
#define StringFunctions_h

#include <string>
#include <vector>
#include "../PLITypes/Constants.h"


class  StringFunctions
{
public:
	static std::vector<std::string> Split(const std::string  & original, const std::string & separator);


	static std::vector<std::string> Split(const std::string & original, const std::string & separator, const int count);


	static std::vector<std::string> Split(const std::string  & original, const char & separator);


	static std::vector<std::string> Split(const std::string & original, const char & separator, const int count);


	static std::string Trim(const std::string &s);


	static std::string LeftTrim(const std::string &s, char ct);

	static std::string RightTrim(const std::string &s, char ct);

	/*std::string rltrim(const std::string &s, char ct)
	{
		std::string::size_type n = s.size();
		std::string::size_type pos = n - 1;

		if (-1 != pos && s[pos] == ct) --pos;

		if (pos != n - 1) return (s.substr(0, pos + 1));
		else return s;
	}*/


	/*std::string trimNonAlpha(const std::string &s)
	{
	std::string::size_type pos = 0;
	std::string::size_type n = s.size();

	while (pos < n && !std::isalpha(s[pos])) ++pos;

	while (n != pos && !std::isalpha(s[n - 1])) --n;

	return (s.substr(pos, n - pos));
	}*/


	/*void split1(const std::string &s, const char delim, std::vector<std::string> & v) {
	// to avoid modifying original string
	// first duplicate the original string and return a char pointer then free the memory
	char * dup = _strdup(s.c_str());
	char *next_token1 = 0;
	char * token = strtok_s(dup, &delim, &next_token1);
	if (token != 0) {
	v.push_back(std::string(token));
	v.push_back(std::string(next_token1));
	//token = strtok_s(0, &delim, &next_token1);
	}
	free(dup);
	}*/



	/*void split(const std::string &s, const char delim, std::vector<std::string> & v)
	{
	char * dup = _strdup(s.c_str());
	char *next_token1 = 0;
	char * token = strtok_s(dup, &delim, &next_token1);
	while (token != 0) {
	v.push_back(std::string(token));
	token = strtok_s(0, &delim, &next_token1);
	}
	free(dup);
	}*/
};


#endif
