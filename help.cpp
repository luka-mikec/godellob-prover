#include "help.h"
#include <vector>

string inf_to_pref(string inf)
{
    vector<string> OperandStack, OperatorStack;

    while( inf != "" )
    {
        string token = string(inf[0]);

        if ( token != "~" && token != "B" && token != ">" && token != "(" && token != ")")
            OperandStack.push_back(token);
        else if ( token == "(" || OperatorStack.size() == 0 ||
                  true /*OperatorHierarchy(token) > OperatorHierarchy(OperatorStack.Top())*/ )
            OperatorStack.push_back ( token );
        else if( token == ")" )
        {
            while( OperatorStack.back() != "(" )
            {
                string operat = OperatorStack.back(); OperatorStack.pop_back();
                string r = OperandStack.back(); OperandStack.pop_back();
                string l = OperandStack.back(); OperandStack.pop_back();
                string operand = operat + l + r;
                OperandStack.Push(operand);
            }


            // Pop the left parthenses from the operator stack.
             OperatorStack.pop_back();
        }

        else if( false )/*
        // Continue to pop operator and operand stack, building prefix
        // expressions until the stack is empty or until an operator at
        // the top of the operator stack has a lower hierarchy than that
        // of the token.
            while( !OperatorStack.IsEmpty() and OperatorHierarchy(token) lessThen Or Equal to OperatorHierarchy(OperatorStack.Top()) )
                OperatorStack.Pop(operator)
                OperandStack.Pop(RightOperand)
                OperandStack.Pop(LeftOperand)
                operand = operator + LeftOperand + RightOperand
                OperandStack.Push(operand)
            endwhile
            // Push the lower precedence operator onto the stack
            OperatorStack.Push(token)
        endif*/
     }
    // If the stack is not empty, continue to pop operator and operand stacks building
    // prefix expressions until the operator stack is empty.
    while( !OperatorStack.empty() )
    {
        string operat = OperatorStack.back(); OperatorStack.pop_back();
        string r = OperandStack.back(); OperandStack.pop_back();
        string l = OperandStack.back(); OperandStack.pop_back();
        string operand = operat + l + r;
        OperandStack.Push(operand);
    }

    // Save the prefix expression at the top of the operand stack followed by popping // the operand stack.

    return OperandStack.back();


}
