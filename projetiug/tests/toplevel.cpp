#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_eventmanager.h"
#include "hw_interface.h"
#include "ei_widget.h"
#include "ei_geometrymanager.h"

using namespace ei;

bool_t process_key(Widget* widget, Event* event, void* user_param)
{
    if (static_cast<KeyEvent*>(event)->key_sym == ALLEGRO_KEY_ESCAPE) {
        static_cast<Application*>(user_param)->quit_request();
        return EI_TRUE;
    }

    return EI_FALSE;
}

bool_t process_display(Widget* widget, Event* event, void* user_param)
{
    if (static_cast<DisplayEvent*>(event)->closed == EI_TRUE) {
        static_cast<Application*>(user_param)->quit_request();
        return EI_TRUE;
    }

    return EI_FALSE;
}

int ei_main(int argc, char* argv[])
{
    Size         screen_size(800, 800);
    color_t      root_bgcol      = {0x52, 0x7f, 0xb4, 0xff};

    color_t      button_color    = {0x88, 0x88, 0x88, 0xff};
    color_t      text_color      = {0x00, 0x00, 0x00, 0xff};
    relief_t     relief          = ei_relief_raised;
    int          button_border_width = 4;
    const char*  button_title        = "click";

    anchor_t button_anchor   = ei_anc_southeast;
    float   button_rel_x    = 1.0;
    float   button_rel_y    = 1.0;
    int     button_x    = -16;
    int     button_y    = -16;
    float   button_rel_size_x = 0.5;

    Size         window_size(400,400);
    const char*  window_title    = "Hello World";
    color_t      window_color    = {0xA0,0xA0,0xA0, 0xff};
    int          window_border_width    = 2;
    bool_t       closable        = EI_TRUE;
    axis_set_t   window_resizable = ei_axis_both;
    Point        window_position(30, 10);

    Application* app = new Application(&screen_size, EI_FALSE);
    app->root_widget()->configure(&screen_size, &root_bgcol, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    Toplevel* toplevel = new Toplevel(app->root_widget());
    toplevel->configure(&window_size, &window_color, &window_border_width, &window_title, &closable, &window_resizable, NULL);

    Button* button = new Button(toplevel);
    button->configure (NULL, &button_color, &button_border_width, NULL, &relief, &button_title, NULL, &text_color, NULL, NULL, NULL, NULL);

    Placer::getInstance().configure(toplevel, NULL, &(window_position.x), &(window_position.y), NULL, NULL, NULL, NULL, NULL, NULL);
    Placer::getInstance().configure(button, &button_anchor, &button_x, &button_y, NULL,NULL, &button_rel_x, &button_rel_y, &button_rel_size_x, NULL);

    EventManager::getInstance().bind(ei_ev_keydown, NULL, "all", process_key, app);
    EventManager::getInstance().bind(ei_ev_display, NULL, "all", process_display, app);

    app->run();

    EventManager::getInstance().unbind(ei_ev_keydown, NULL, "all", process_key, app);
    EventManager::getInstance().unbind(ei_ev_display, NULL, "all", process_display, app);

    delete app;

    return (EXIT_SUCCESS);
}
