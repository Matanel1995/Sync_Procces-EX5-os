#include "Node.hpp"

class Stack{
    private:
        Node *StackHead;
        int len;
    public:
        Stack();
        ~Stack();
        void initStack();
        void push(char* data);
        void pop();
        Node top();
        void printStack();
        int getLen(){return this->len;}
    };

    Stack::Stack(){this->StackHead = NULL; this->len = 0;}
    Stack::~Stack()
    {
}
