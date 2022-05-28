#include <string>
#include <iostream>
#include "Stack.hpp"
#include "malloc.cpp"


using namespace std;

void Stack::initStack(){
    this->StackHead = NULL;
}

void Stack::printStack(){//show
    string listData = "";
    Node *tempNode = (this->StackHead);
    int len = this->getLen();
    for(int i=0; i < len ; i++){
        listData = listData + "   ------>   " + tempNode->data;
        cout << tempNode << endl;
        tempNode = tempNode->next;
    }
    cout << listData << endl;
}

void Stack::push(char* data){
    //Allocating memory for new node
    Node *tempNode = (Node*)(myMalloc::malloc)(sizeof(Node));
    //Allocating 1024 bits to the data (requested in the assignment)
    tempNode->data = (char*)myMalloc::malloc(1024);
    //Seting the new data and the new next Node to null becouse this is the first Node in the stuck now
    tempNode->data = data;
    tempNode->next = NULL;
    //Check if the stuck is empty or not
    if(this->StackHead == NULL){
        this->StackHead = tempNode;
        this->len++;
    }
    else{//Other was here before now i will point to him
        tempNode->next = this->StackHead;
        this->StackHead = tempNode;
        this->len++;
    }
}

Node Stack::top(){
    return *(this->StackHead);
}

void Stack::pop(){
    //Check if the stack is empty or not 
    if(this->StackHead == NULL) return;
    //Stack is not empty need to free memory and delete referance
    if(this->len == 1){ //This is the head
        myMalloc::free(this->StackHead);
        myMalloc::free(this->StackHead->data);
        this->StackHead = NULL;
        this->len = 0;
    }
    else{
        //Creating temp Node to save the new head 
        Node *tempNode = this->StackHead;
        this->StackHead = tempNode->next;
        myMalloc::free(tempNode);
        myMalloc::free(tempNode->data);
        this->len--;
    }
}

// int main(){
//     Stack *a = new Stack;
//     //char userData = (char)myMalloc::malloc(sizeof(1024));
//     char userData1[1024];
//     char userData2[1024];
//     char userData3[1024];
//     char userData4[1024];
//     char userData5[1024];
//     char userData6[1024];
//     fgets(userData1,1024,stdin);
//     a->push(userData1);
//     a->printStack();
//     fgets(userData2,1024,stdin);
//     a->push(userData2);
//     a->printStack();
//     fgets(userData3,1024,stdin);
//     a->push(userData3);
//     a->printStack();
//     fgets(userData4,1024,stdin);
//     a->push(userData4);
//     a->printStack();
//     fgets(userData5,1024,stdin);
//     a->push(userData5);
//     a->printStack();
//     fgets(userData6,1024,stdin);
//     a->push(userData6);
//     a->printStack();
//     for(int i=0; i<4; i++){
//         //cout << a->top().data << endl;;
//         //cout << a->top().next << endl;;
//         //cout << a->getLen() << endl;
//         a->pop();
//     }
//     a->printStack();
//     //cout << a->getLen() << endl;
//     //a->printStack();
//     return 1;
// }