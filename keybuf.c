//
//  keybuf.c
//  zi23
//
//  Created by Thomas Foster on 3/4/23.
//

#include "keybuf.h"

#define KBUF_SIZE 16

static SDL_Keysym queue[KBUF_SIZE];
static int front = -1;
static int rear = -1;

bool KeyBufferIsEmpty(void)
{
    return front == -1;
}

void EnqueueKey(SDL_Keysym keysym)
{
    if ( (rear + 1) % KBUF_SIZE == front ) { // Full?
        return;
    }

    if ( KeyBufferIsEmpty() ) { // Empty?
        front = 0;
    }

    rear = (rear + 1) % KBUF_SIZE;
    queue[rear] = keysym;
}

bool DequeueKey(SDL_Keysym * out)
{
    if ( KeyBufferIsEmpty() ) {
        return false;
    }

    *out = queue[front];

    if ( front == rear ) {
        front = -1;
        rear = -1;
    } else {
        front = (front + 1) % KBUF_SIZE;
    }

    return true;
}
