#include "StringFunctions.h"

//This function splits a string(the first parameter) using a delimiter(second parameter) which is a string and returns the list as vector of strings.
std::vector<std::string> StringFunctions::Split(const std::string  & original, const std::string & separator)
{
	std::vector<std::string> results;
	size_t start = 0;
	size_t end = original.size();
	size_t next = original.find(separator, start);
	while (next < end)
	{
		std::string t = original.substr(start, next - start);
		results.push_back(t);
		start = next + separator.size();
		next = original.find(separator, start);
	}
	results.push_back(original.substr(start, next - start));
	return results;
}

//This function splits a string(the first parameter) using a delimiter(second parameter) so that the number of slits doesn't exceed from count(third parameter).
std::vector<std::string> StringFunctions::Split(const std::string & original, const std::string & separator, const int count)
{
	std::vector<std::string> results;
	if (count > 1)
	{
		size_t start = 0;
		size_t end = original.size();
		size_t next = original.find(separator, start);
		if (next != -1)
		{
			while (next < end)
			{
				std::string t = original.substr(start, next - start);
				results.push_back(t);
				start = next + separator.size();
				if (count == results.size() + 1) break;
				next = original.find(separator, start);
			}
			results.push_back(original.substr(start, next - start));
		}
		else
		{
			results.push_back(original);
			results.push_back("");
		}
	}
	else results.push_back(original);
	return results;
}

//This function splits a string(the first parameter) using a delimiter(second parameter) which is a c-string and returns the list as vector of strings.
std::vector<std::string> StringFunctions::Split(const std::string  & original, const char & separator)
{
	std::vector<std::string> results;
	size_t start = 0;
	size_t end = original.size();
	size_t next = original.find(separator, start);
	while (next < end)
	{
		std::string t = original.substr(start, next - start);
		results.push_back(t);
		start = next + 1;
		next = original.find(separator, start);
	}
	results.push_back(original.substr(start, next - start));
	return results;
}

//This function splits a string(the first parameter) using a delimiter(second parameter) so that the number of slits doesn't exceed from count(third parameter).
std::vector<std::string> StringFunctions::Split(const std::string & original, const char & separator, const int count)
{
	std::vector<std::string> results;
	if (count > 1)
	{
		size_t start = 0;
		size_t end = original.size();
		size_t next = original.find(separator, start);
		while (next < end)
		{
			std::string t = original.substr(start, next - start);
			results.push_back(t);
			start = next + 1;
			if (count == results.size() + 1) break;
			next = original.find(separator, start);
		}
		results.push_back(original.substr(start, next - start));
	}
	else results.push_back(original);
	return results;
}


//This function trims a string from left and right.
std::string StringFunctions::Trim(const std::string &s)
{
	std::string::size_type pos = 0;
	std::string::size_type n = s.size();

	while (pos < n && (s[pos]) != ' ') ++pos;

	while (n != pos && (s[n - 1] != ' ')) --n;

	return (s.substr(pos, n - pos));
}


//This function trims a string from left.
std::string StringFunctions::LeftTrim(const std::string &s, char ct)
{
	std::string::size_type pos = 0;
	std::string::size_type n = s.size();

	while (pos < n && s[pos] == ct) ++pos;

	return (s.substr(pos, n - pos));
}


//This function trims a string from right.
std::string StringFunctions::RightTrim(const std::string &s, char ct)
{
	std::string::size_type n = s.size();
	std::string::size_type pos = n - 1;

	while (-1 != pos && s[pos] == ct) --pos;

	if (pos != n - 1) return (s.substr(0, pos + 1));
	else return s;
}

