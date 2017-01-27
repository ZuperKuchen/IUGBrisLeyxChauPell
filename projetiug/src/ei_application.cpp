#include <stdio.h>
#include <stdlib.h>

#include "ei_eventmanager.h"
#include "hw_interface.h"
#include "ei_application.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

namespace ei {
Application::Application(Size* main_window_size, bool_t fullscreen){

    hw_init();         //initialisation

    hw_create_window(main_window_size,fullscreen);    //create windows

   // A COMPLETER
}

Application::~Application(){

    hw_quit();
}


void run(){

    //A COMPLETER
}


void invalidate_rect(const Rect &rect);


void quit_request();


Frame* root_widget();

surface_t root_surface();


}
