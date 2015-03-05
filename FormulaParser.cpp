
// Std
#include <iostream>

// Project
#include "FormulaParser.h"

//****************************************************************************

FormulaParser::FormulaParser()
{
}

//****************************************************************************

bool FormulaParser::parse(const std::string & in)
{
    if (in.empty())
        return false;

     _expression = parseExpression(in);

     std::cout << "Finally -------" <<std::endl;
     printQueue();
     printStack();

    _queue.clear();
    _stack.clear();

    return !_expression.empty();
}

//****************************************************************************

double FormulaParser::compute() const
{
    return evaluate(_expression);
}

//****************************************************************************

RPNExpression FormulaParser::parseExpression(const std::string &input)
{
    // here input should be non-empty
    {
        std::string::const_iterator it = input.begin();
        std::string::const_iterator end = input.end();
        std::string token;
        bool parsingNumber = false;
        while (it != end)
        {

            //        std::cout << "Input character : " << *it << std::endl;

            // parse a number
            switch (*it)
            {
            case '0' : case '1' : case '2' : case '3' : case '4' : case '5' : case '6' : case '7' : case '8' : case '9' : case '.' :
                parsingNumber=true;
                token.push_back(*it);
                it++;
                continue;
                break;
            }

            if (!parsingNumber)
            {
                switch (*it)
                {
                case ' ' :
                    // ignore spaces
                    it++;
                    continue;
                    break;
                case '(' : case ')' : case '+' : case '-' : case '*' : case '/' :
                    token.push_back(*it);
                    it++;
                    break;
                default:
                    std::cerr << "Unrecognized caracter found" << std::endl;
                    return RPNExpression();
                    break;
                };

            }

            if (!processToken(token))
                return RPNExpression();

            token.clear();
            parsingNumber=false;


            printQueue();
            printStack();

        }

        // process the last token
        if (!processToken(token))
            return RPNExpression();
        token.clear();
        parsingNumber=false;
        printQueue();
        printStack();
    }


    // move remaining operators from stack to queue:
    {
        RPNExpression::reverse_iterator it = _stack.rbegin();
        while (it != _stack.rend())
        {
            if (it->type == RPNToken::LEFT_BRACKET)
            {
                std::cerr << "Error in the expression" << std::endl;
                return RPNExpression();
            }
            _queue.push_back(*it);
            _stack.pop_back();
            it = _stack.rbegin();
        }
    }

    return _queue;

}

//****************************************************************************

bool FormulaParser::processToken(const std::string & t)
{
    std::cout << "Process token : " << t << std::endl;

    if (t[0] == '(')
    {
        RPNToken token(RPNToken::LEFT_BRACKET);
        _stack.push_back(token);
        return true;
    }
    else if (t[0] == '+' ||
             t[0] == '-' ||
             t[0] == '*' ||
             t[0] == '/')
    {
        RPNToken token(RPNToken::OPERATOR);
        token.data.op = t[0];

        std::cout << "Process operator : " << token.data.op << std::endl;

        // push to stack and pop off other operators with higher or equal precedence
        RPNExpression::reverse_iterator it = _stack.rbegin();
        while (it->type == RPNToken::OPERATOR &&
               RPNToken::priority(*it) >= RPNToken::priority(token) &&
               it != _stack.rend())
        {

            std::cout << "Push into queue : " << it->data.op << std::endl;

            _queue.push_back(*it);
            _stack.pop_back();
            it = _stack.rbegin();

            printQueue();
            printStack();

        }
        _stack.push_back(token);
        return true;
    }
    else if (t[0] == ')')
    {

        std::cout << "Process ')'"<< std::endl;

        // move operations from the stack to the queue until '('
        RPNExpression::reverse_iterator it = _stack.rbegin();
        while (it->type != RPNToken::LEFT_BRACKET)
        {
            std::cout << "Push into queue : " << it->data.op << std::endl;

            _queue.push_back(*it);
            _stack.pop_back();
            it = _stack.rbegin();

            printQueue();
            printStack();

            if (it == _stack.rend())
            {
                std::cerr << "Unbalanced number of brackets" << std::endl;
                return false;
            }

        }

        // pop off the left bracket
        _stack.pop_back();

        return true;
    }

    // Convert to number
    RPNToken token(RPNToken::NUMBER);
    token.data.val = ::atof(t.c_str());
    _queue.push_back(token);
    return true;


}

//****************************************************************************

double FormulaParser::evaluate(const RPNExpression &expr) const
{    
    RPNExpression exprCopy = expr;
    RPNExpression::iterator it = exprCopy.begin();
    while (exprCopy.size() >= 3)
    {
        RPNExpression::iterator t = it;
        RPNToken & first = *(t++);
        RPNToken & second = *(t++);
        RPNToken & operation = *t;
        RPNToken r = RPNToken::evaluate(first, second, operation);
        if (r.type == RPNToken::NUMBER)
        {
            it = exprCopy.erase(it,t++);
            *it = r;
            printExpression(exprCopy);
            it = exprCopy.begin();
        }
        else
        {
            it++;
        }


        if (it == exprCopy.end())
        {
            std::cerr << "Error : Something wrong with the expression" << std::endl;
            printExpression(expr);
            return -123456789;
        }

    }

    RPNToken & r = exprCopy.front();
    if (r.type != RPNToken::NUMBER)
    {
        std::cerr << "Error : Something wrong with the expression" << std::endl;
        printExpression(expr);
        return -123456789;
    }
    return r.data.val;
}

//****************************************************************************

void FormulaParser::printExpression(const RPNExpression & expr) const
{
    std::cout << "Expression : ";
    RPNExpression::const_iterator it = expr.begin();
    RPNExpression::const_iterator end = expr.end();
    for (;it!=end;++it)
    {
        if (it->type == RPNToken::NUMBER)
        {
            std::cout << it->data.val << ",";
        }
        else if (it->type == RPNToken::OPERATOR)
        {
            std::cout << it->data.op << ",";
        }
    }
    std::cout << std::endl;
}

//****************************************************************************

void FormulaParser::printQueue() const
{
    std::cout << "Queue : ";
    std::list<RPNToken>::const_iterator it = _queue.begin();
    std::list<RPNToken>::const_iterator end = _queue.end();
    for (;it!=end;++it)
    {
        if (it->type == RPNToken::NUMBER)
        {
            std::cout << it->data.val << ",";
        }
        else if (it->type == RPNToken::OPERATOR)
        {
            std::cout << it->data.op << ",";
        }
    }
    std::cout << std::endl;
}

//****************************************************************************

void FormulaParser::printStack() const
{
    std::cout << "Stack : ";
    std::list<RPNToken>::const_iterator it = _stack.begin();
    std::list<RPNToken>::const_iterator end = _stack.end();
    for (;it!=end;++it)
    {
        if (it->type == RPNToken::LEFT_BRACKET)
        {
            std::cout << '(' << ",";
        }
        else if (it->type == RPNToken::RIGHT_BRACKET)
        {
            std::cout << ')' << ",";
        }
        else if (it->type == RPNToken::OPERATOR)
        {
            std::cout << it->data.op << ",";
        }
    }
    std::cout << std::endl;
}

//****************************************************************************
