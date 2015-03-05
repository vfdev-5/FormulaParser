#ifndef FORMULAPARSER_H
#define FORMULAPARSER_H

// STD
#include <string>
#include <queue>
#include <stack>
#include <list>

//****************************************************************************

/*!
  This parser implements Shunting-yard algorithm and a method to evaluate RPN expression

  Idea :
    1) formula expression is parsed into RPN form using Shunting-yard algorithm
        There is the output queue (RPN expression) and operator stack
        Algorithm :
        a) Read a token
            - if token = number     => push to the output queue
            - if token = function   => push to the operator stack
            - if token = '('        => push to the operator stack
            - if token = ')'        =>
                 -- pop operators off the operator stack until '('
                 remove the '(' from the stack
                 -- if stack is empty and no '(' found -> expression error
            - if token = operator, e.g. Op1,   =>
                 -- while the top operator, e.g. Op2, in the stack has
                 precedence more than or equal to that of Op1 => pop Op2 into the queue
                 -- push Op1 to the stack
            - when no more tokens to read =>
                 -- read remaining tokens from the operator stack
                 -- if a '(' is found -> expression error
                 -- else pop the token to the output queue

        For example : (1 + (2/3)+1*3)*100+30
                Stack:
                Queue:
            1) token = '(' =>
                Stack:(,
                Queue:
            2) token = 1 =>
                Stack:(,
                Queue:1,
            3) token = + =>
                Stack:(,+
                Queue:1,
            4) token = '(' =>
                Stack:(,+,(
                Queue:1,
            5) token = 2 =>
                Stack:(,+,(,
                Queue:1,2,
            6) token = / =>
                Stack:(,+,(,/
                Queue:1,2,
            7) token = 3 =>
                Stack:(,+,(,/
                Queue:1,2,3,
            8) token = ')' =>
                Stack:(,+,
                Queue:1,2,3,/
            9) token = + =>
                Stack:(,+,+
                Queue:1,2,3,/
           10) token = 1 =>
                Stack:(,+,+
                Queue:1,2,3,/,1,
           11) token = * => precedence(*) > precedence(+) ==>
                Stack:(,+,+,*
                Queue:1,2,3,/,1,
           12) token = 3 =>
                Stack:(,+,+,*
                Queue:1,2,3,/,1,3,
           13) token = ')' =>
                Stack:
                Queue:1,2,3,/,1,3,*,+,+,
           14) token = * =>
                Stack:*,
                Queue:1,2,3,/,1,3,*,+,+,
           15) token = 100 =>
                Stack:*,
                Queue:1,2,3,/,1,3,*,+,+,100
           16) token = + => precedence(+) < precedence(*) ==>
                Stack:+,
                Queue:1,2,3,/,1,3,*,+,+,100,*
           17) token = 30 =>
                Stack:+
                Queue:1,2,3,/,1,3,*,+,+,100,*,30
           18) no tokens
                Stack:
                Queue:1,2,3,/,1,3,*,+,+,100,*,30,+
            ==> 1,(2/3),1,3,*,+,+,100,*,30,+
                1,(2/3),(1*3),+,+,100,*,30,+
                1,((2/3) + (1*3)),+,100,*,30,+
                (1 + ((2/3) + (1*3))),100,*,30,+
                (1 + ((2/3) + (1*3)))*100,30,+
                (1 + ((2/3) + (1*3)))*100 + 30


    2) evaluate RPN expression.
       For example :  A,B,C,*,+,D,-
       a) Search from the left the pattern X,Y,Operator
          when found compute and replace the patten with the result
            A,B,C -> B,C,* => R1=B*C ==> A,R1,+D,-
          Repeat from the begining
            A,R1,+ => R2=A+R1 ==> R2,D,-
          Repeat from the begining
            R2,D,- => R3=R2-D ==> R3
          No more patterns => End


  http://en.wikipedia.org/wiki/Shunting-yard_algorithm
  */

//****************************************************************************

// TODO :
//

struct RPNToken
{
    enum Type {
        NON_VALID = 0,
        NUMBER,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        OPERATOR
    };

    union Data {
        double val;
        char op;
    };

    Type type;
    Data data;

    RPNToken() : type(NON_VALID)
    {}

    RPNToken(Type t) : type(t)
    {}

    static int priority(const RPNToken & t)
    {
        if (t.type != OPERATOR)
            return -1;
        switch (t.data.op)
        {
        case '+' : case '-' : return 1;
        case '*' : case '/' : return 2;
        }
        return -1;
    }

    static RPNToken evaluate(const RPNToken & f, const RPNToken & s, const RPNToken & op)
    {
        if (f.type == RPNToken::NUMBER &&
                s.type == RPNToken::NUMBER &&
                op.type == RPNToken::OPERATOR)
        {
            RPNToken r(RPNToken::NUMBER);
            switch (op.data.op) {
            case '+':
                r.data.val=f.data.val + s.data.val;
                return r;
            case '-':
                r.data.val=f.data.val - s.data.val;
                return r;
            case '*':
                r.data.val=f.data.val * s.data.val;
                return r;
            case '/':
                r.data.val=f.data.val / s.data.val;
                return r;
            }
        }
        return RPNToken();
    }

};
typedef std::list<RPNToken> RPNExpression;

class FormulaParser
{

public:
    FormulaParser();

    bool parse(const std::string & in);

    double compute() const;

protected:

    RPNExpression parseExpression(const std::string & input);

    bool processToken(const std::string & t);

    double evaluate(const RPNExpression & expr) const;
    bool _isValid;

    RPNExpression _expression;
    RPNExpression _queue;
    RPNExpression _stack;

private:
    void printQueue() const;
    void printStack() const;
    void printExpression(const RPNExpression & expr) const;

};

//****************************************************************************

#endif // FORMULAPARSER_H
