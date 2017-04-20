#include <iostream>
#include <cstdlib>
using namespace std;

#include "Queue.h"


// Default constructor: create an empty stack.
Queue::Queue() : List()
{
}// Queue()

// Insert an object into the queue.
void Queue::enqueue(int data)
{
    insertBack(data);
}// enqueue()

// Remove the front object and return its value.
int Queue::dequeue()
{
    int result = front();
    removeFront();
    return result;
}// dequeue()

// Inspect the value of the front object.
int Queue::front()
{
    if (!isEmpty()) 
        return *begin();
    else 
    {
        cout << "attempt to access the front of an empty queue" << endl;
        exit(1);
    }  
}// front()

// Delete all the objects in the queue.
void Queue::makeEmpty()
{
    List::makeEmpty();
}// makeEmpty()

// Test if the queue is empty.
bool Queue::isEmpty()
{
    return List::isEmpty();
}// isEmpty()

// Concatenate two queues.
void Queue::concatenate(Queue &other)
{
    concatenate(other);
}// concatenate()
