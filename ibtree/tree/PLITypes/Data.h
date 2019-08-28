#ifndef Data_H
#define Data_H

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <string>
#include <iostream>
#include "../tools/FloatingPoint.h"

enum KeyType
{
	Integer=0,
	Float,
	Double,
	Time,
	String,
};


	struct Data
	{
	public:
		// Create an empty slice.
		Data() {}
		~Data() {}
		Data(const Data& t)
		{
			switch (kt)
			{
			case KeyType::Integer: data_i = t.data_i;
				break;
			case KeyType::Double: data_d = t.data_d;
				break;
			case KeyType::Float: data_f = t.data_f;
				break;
			case KeyType::String: data_s = t.data_s;
			}
		}

		// Create a slice that refers to the contents of "s"
		//Data(const std::string& s) : data_(s.data()), size_(s.size()) {}

		// Create a slice that refers to s[0,strlen(s)-1]
		Data(const char* s)
		{
			switch (kt)
			{
			case KeyType::Integer: data_i = std::atoi(s);
				break;
			case KeyType::Double: data_d = std::atof(s);
				break;
			case KeyType::Float: data_f = FloatingPoint::ToFloat(s);
				break;
			case KeyType::String: data_s = std::string(s);
			}
		}

		Data(const std::string& s) : data_s(s) { ktt = KeyType::String; }
		Data(const int& s) : data_i(s) { ktt = KeyType::Integer; }
		Data(const float& s) : data_f(s) { ktt = KeyType::Float; }
		Data(const double& s) : data_d(s) { ktt = KeyType::Double; }

		// Return a pointer to the beginning of the referenced data
		//const char* data() const { return data; }

		const std::string toString() const
		{
			switch (kt)
			{
			case KeyType::Integer: return std::to_string(data_i);
				break;
			case KeyType::Double: return std::to_string(data_d);
				break;
			case KeyType::Float: return std::to_string(data_f);
				break;
			case KeyType::String: return data_s;
			}
		}

		// Return the length (in bytes) of the referenced data
		//size_t size() const { return size; }

		// Change this slice to refer to an empty array
		//void clear() { data_ = ""; size_ = 0; }

		// Three-way comparison.  Returns value:
		//   <  0 iff "*this" <  "b",
		//   == 0 iff "*this" == "b",
		//   >  0 iff "*this" >  "b"
		int compare(const Data& b) const;

		Data& operator = (const float &t)
		{
			data_f = t;
			return *this;
		}

		Data& operator = (const Data &t)
		{
			switch (kt)
			{
			case KeyType::Integer: data_i = t.data_i;
				break;
			case KeyType::Double: data_d = t.data_d;
				break;
			case KeyType::Float: data_f = t.data_f;
				break;
			case KeyType::String: data_s = t.data_s;
			}
			return *this;
		}

		bool operator <(Data const& t) {
			switch (kt)
			{
			case KeyType::Integer: return data_i < t.data_i;
				break;
			case KeyType::Double: return data_d < t.data_d;
				break;
			case KeyType::Float: return data_f < t.data_f;
				break;
			case KeyType::String: return data_s < t.data_s;
			}
		}

		bool operator <=(Data const& t) {
			switch (kt)
			{
			case KeyType::Integer: return data_i <= t.data_i;
				break;
			case KeyType::Double: return data_d <= t.data_d;
				break;
			case KeyType::Float: return data_f <= t.data_f;
				break;
			case KeyType::String: return data_s <= t.data_s;
			}
		}


		bool operator <=(Data & t) {
			switch (kt)
			{
			case KeyType::Integer: return data_i <= t.data_i;
				break;
			case KeyType::Double: return data_d <= t.data_d;
				break;
			case KeyType::Float: return data_f <= t.data_f;
				break;
			case KeyType::String: return data_s <= t.data_s;
			}
		}

		bool operator>(Data const& t) {
			switch (kt)
			{
			case KeyType::Integer: return data_i > t.data_i;
				break;
			case KeyType::Double: return data_d > t.data_d;
				break;
			case KeyType::Float: return data_f > t.data_f;
				break;
			case KeyType::String: return data_s > t.data_s;
			}
		}

		bool operator >=(Data const& t) {
			switch (kt)
			{
			case KeyType::Integer: return data_i >= t.data_i;
				break;
			case KeyType::Double: return data_d >= t.data_d;
				break;
			case KeyType::Float: return data_f >= t.data_f;
				break;
			case KeyType::String: return data_s >= t.data_s;
			}
		}

		friend std::ostream& operator<<(std::ostream& out, const Data &f1)
		{
			switch (kt)
			{
			case KeyType::Integer: out << f1.data_i;
				break;
			case KeyType::Double: out << f1.data_d;
				break;
			case KeyType::Float: out << f1.data_f;
				break;
			case KeyType::String: out << f1.data_s;
			}

			return out;
		}

		friend std::istream& operator>> (std::istream& is, Data& f1)
		{
			switch (kt)
			{
			case KeyType::Integer: is >> f1.data_i;
				break;
			case KeyType::Double: is >> f1.data_d;
				break;
			case KeyType::Float: is >> f1.data_f;
				break;
			case KeyType::String: is >> f1.data_s;
			}

			return is;
		}

	//private:
		union
		{
			//const char* data;
			std::string data_s;
			int data_i;
			float data_f;
			double data_d;
		};
		static int y;
		static KeyType kt;
		KeyType ktt;
		//const char* data_c;
		//size_t size_;

		// Intentionally copyable
	};


	inline bool operator==(const Data& x, const Data& t)
	{
		switch (Data::kt)
		{
		case KeyType::Integer: return x.data_i == t.data_i;
			break;
		case KeyType::Double: return x.data_d == t.data_d;
			break;
		case KeyType::Float: return FloatingPoint::almostEqual(x.data_f, t.data_f);
			break;
		case KeyType::String: return x.data_s == t.data_s;
		}
	}

	inline bool operator<= (const Data& s, const Data& t)
	{
		switch (Data::kt)
		{
		case KeyType::Integer: return s.data_i <= t.data_i;
			break;
		case KeyType::Double: return s.data_d <= t.data_d;
			break;
		case KeyType::Float: return s.data_f <= t.data_f;
			break;
		case KeyType::String: return s.data_s <= t.data_s;
		}
	}

	inline bool operator< (const Data& s, const Data& t)
	{
		switch (Data::kt)
		{
		case KeyType::Integer: return s.data_i < t.data_i;
			break;
		case KeyType::Double: return s.data_d < t.data_d;
			break;
		case KeyType::Float: return s.data_f < t.data_f;
			break;
		case KeyType::String: return s.data_s < t.data_s;
		}
	}

	inline bool operator!=(const Data& x, const Data& y)
	{
		return !(x == y);
	}

	inline int Data::compare(const Data& b) const
	{
		/*const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
		int r = memcmp(data_, b.data_, min_len);
		if (r == 0)
		{
		if (size_ < b.size_) r = -1;
		else if (size_ > b.size_) r = +1;
		}
		return r;*/
		return 0;
	}





#endif  // Data_H
