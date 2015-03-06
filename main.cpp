
// Std
#include <iostream>
#include <string>

#include <list>

// Project
#include "FormulaParser.h"

int main(int argc, char** argv)
{

    if (argc != 2)
        return 1;

    std::string in = argv[1];

    std::cout << "input formula : " << in << std::endl;

    FormulaParser p;
    if (!p.parse(in))
        return 1;

    std::cout << "Result is " << p.compute() << std::endl;

    return 0;
}

// ------------- TESTS -------------
/*
    std::list<std::string> list;

    list.push_back("a");
    list.push_back("b");
    list.push_back("c");
    list.push_back("d");

    std::cout << "List : ";
    std::list<std::string>::const_iterator it = list.begin();
    for (;it != list.end();it++)
    {
        std::cout << *it << ", ";

    }
    std::cout << std::endl;

    std::string limit = "a";
    std::list<std::string>::reverse_iterator ri = list.rbegin();
    int c = 10;
    while (ri != list.rend()
           && *ri != limit
           && c > 0)
    {
        std::cout << "Remove : " << *ri << std::endl;

        std::list<std::string>::iterator ii = --(ri.base());
        std::cout << "Iterator : " << *ii << std::endl;
        ri = std::list<std::string>::reverse_iterator(
                    list.erase(
                        --(ri.base())
                        )
                    );

//        list.pop_back();
//        ri = list.rbegin();

        c--;
    }



*/


