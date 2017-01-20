#include <stdlib.h>

#include "ei_main.h"
#include "ei_application.h"
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

    // Create the application and change the color of the background.
    Application* app = new Application(&main_window_size, EI_FALSE);
    app->root_widget()->configure(NULL, &red, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	// Define the polygon vertices.
    for (int i = 0; i < 4; i++) {
		points[i].point.x	= coords[i * 2];
		points[i].point.y	= coords[i * 2 + 1];
		if (i < 3)
			points[i].next	= &(points[i + 1]);
		else
			points[i].next	= NULL;
	}

    // Fill the surface, draw polygon, update screen.
    fill(app->root_surface(), &red, EI_FALSE);
    draw_polygon(app->root_surface(), points, transp_blue, NULL);
	hw_surface_update_rects(NULL);

    // Run the application's main loop.
    app->run();

    delete app;

    return (EXIT_SUCCESS);
}
