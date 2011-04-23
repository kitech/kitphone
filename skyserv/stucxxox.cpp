// stucxxox.cpp --- 
// 
// Author: liuguangzhao
// Copyright (C) 2007-2010 liuguangzhao@users.sf.net
// URL: 
// Created: 2011-04-10 18:11:18 +0800
// Version: $Id$
// 

#include "stucxxox.h"

#include <new>

#include <tr1/memory>
#include <boost/smart_ptr.hpp>

auto i = 8;

enum class Enumeration {
    Val1,
    Val2,
    Val3 = 100,
    Val4 /* = 101 */
};

auto f1 = [] (int a, char *b) -> decltype(1) {
    
};

void test() {
    f1(0,0);

    struct Point  {
        Point() {}
        Point(int x, int y): x_(x), y_(y) {}
        int x_, y_;
    };

    union U {
        int z;
        double w;
        Point p;  // Illegal in C++; point has a non-trivial constructor.  However, this is legal in C++0x.
        U() { new( &p ) Point(); } // No nontrivial member functions are implicitly defined for a union;
        // if required they are instead deleted to force a manual definition.
    };

}

void test2()
{
    if (1) {
        boost::scoped_ptr<std::string> aptr(new std::string("oisdjfisdjf"));
        // boost::scoped_ptr<char> aptr = boost::scoped_ptr<char>(new char(100)); // error
        // boost::scoped_ptr<char> bptr = aptr; // error
    }
}

// show defaulted and deleted functions
class TX1 {
public:
    TX1& operator=(const TX1 &) = delete;
    TX1(const TX1 &) = delete;
};

class TX2 {
public:
    TX2& operator=(const TX2 &) = default;
    TX2(const TX2 &) = default;
};

// variadic templates
template<typename T, typename... Args>		// note the "..."
void printf(const char* s, T value, Args... args)	// note the "..."
{
    while (s && *s) {
        if (*s=='%' && *++s!='%') {	// a format specifier (ignore which one it is)
            std::cout << value;		// use first non-format argument
            return printf(++s, args...); 	// "peel off" first argument
        }
        std::cout << *s++;
    }
    // throw std::runtime error("extra arguments provided to printf");
}


