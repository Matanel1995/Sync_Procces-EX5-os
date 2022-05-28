#ifndef _Node_
#define _Node_
#include <stdlib.h>
using namespace std;
typedef struct Node{
    char* data;
    struct Node *next = NULL;
}Node;
#endif
