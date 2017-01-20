#include <stdlib.h>

#include "ei_main.h"
#include "ei_types.h"
#include "ei_draw.h"
#include "ei_event.h"
#include "hw_interface.h"
using namespace ei;

int ei_main(int argc, char** argv)
{
    Size main_window_size = Size(640,480);
    linked_point_t	points[4];
    int	coords[] = { 20, 20, 620, 20, 20, 460, 620, 460 };
    color_t red          = { 0xff, 0x00, 0x00, 0xff };
    color_t transp_blue  = { 0x00, 0x00, 0xff, 0x88 };

   // Init acces to hardware.
    hw_init();

    // Create the main window.
    surface_t main_window = hw_create_window(&main_window_size, EI_FALSE);

    // Define the polygon vertices.
    for (int i = 0; i < 4; i++) {
        points[i].point.x	= coords[i * 2];
        points[i].point.y	= coords[i * 2 + 1];
        if (i < 3)
            points[i].next	= &(points[i + 1]);
        else
            points[i].next	= NULL;
    }

    // fill surface, draw polygon, update screen.
    fill(main_window, &red, EI_FALSE);
    draw_polygon(main_window, points, transp_blue, NULL);
    hw_surface_update_rects(NULL);

    // Wait for a key press.
    bool running = EI_TRUE;
    while (running == EI_TRUE) {
        Event* event = hw_event_wait_next();
        if(event->type == ei_ev_keydown)
            running = EI_FALSE;
    }
    // Free hardware resources.
    hw_quit();

    return (EXIT_SUCCESS);
}
