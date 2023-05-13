//
//  keybuf.h
//  zi23
//
//  Created by Thomas Foster on 3/4/23.
//

#ifndef keybuf_h
#define keybuf_h

#include <stdbool.h>
#include <SDL2/SDL_keyboard.h>

bool KeyBufferIsEmpty(void);
void EnqueueKey(SDL_Keysym keysym);
bool DequeueKey(SDL_Keysym * out);

#endif /* keybuf_h */
