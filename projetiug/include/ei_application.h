/**
 *  @file application.h
 *  @brief  Manages the main steps of a graphical application: initialization, main window,
 *    main loop, quitting, resource freeing.
 *
 *  \author
 *  Created by François Bérard on 30.12.11.
 *  Modified by Pierre Bénard and Aurélie Bugeau
 *
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "ei_types.h"
#include "ei_event.h"
#include "ei_widget.h"

namespace ei {

class Application
{
public:
    /**
     * \brief Creates an application.
     *    <ul>
     *      <li> initializes the hardware (calls \ref hw_init), </li>
     *      <li> creates the root window (either in a system window, or the entire screen), </li>
     *      <li> creates the root widget to accress the root window. </li>
     *    </ul>
     *
     * @param main_window_size  If "fullscreen is false, the size of the root window of the application.
     *          If "fullscreen" is true, the current monitor resolution is
     *          used as the size of the root window, this size is returned
     *          in this parameter.
     * @param fullScreen    If true, the root window is the entire screen. Otherwise, it is a system window.
     */
    Application(Size* main_window_size, bool_t fullscreen);

    /**
     * \brief Releases all the resources of the application, and releases the hardware
     *    (i.e., calls \ref hw_quit).
     */
    ~Application();

    /**
     * \brief Runs the application: enters the main event loop. Exits when
     *    \ref app_quit_request is called.
     */
    void run();

    /**
     * \brief Adds a rectangle to the list of rectangles that must be updated on screen. The real
     *    update on the screen will be done at the right moment in the main loop.
     *
     * @param rect    The rectangle to add, expressed in the root window coordinates.
     *        A copy is made, so it is safe to release the rectangle on return.
     */
    void invalidate_rect(const Rect &rect);

    /**
     * \brief Tells the application to quite. Is usually called by an event handler (for example
     *    when pressing the "Escape" key).
     */
    void quit_request();

    /**
     * \brief Returns the "root widget" of the application: a "frame" widget that encapsulate the
     *    root window.
     *
     * @return      The root widget.
     */
    Frame* root_widget();

    /**
     * \brief Returns the surface of the root window. Used to create surfaces with similar r, g, b
     *    channels.
     *
     * @return      The surface of the root window.
     */
    surface_t root_surface();
};

}

#endif

