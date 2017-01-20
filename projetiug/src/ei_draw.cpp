#include "ei_draw.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

namespace ei {

linked_point_t* arc(const Point& center, float radius, int start_angle, int end_angle)
{
    linked_point_t* list = (linked_point_t*) malloc(sizeof(linked_point_t));
    linked_point_t* start = list;

    float angle = (end_angle - start_angle) * M_PI/180.f;
    int nbpts = radius * fabs(angle);
    for(int i=0; i<=nbpts; i++){
        list->next = (linked_point_t*) malloc(sizeof(linked_point_t));
        list = list->next;
        list->point.x = radius * cosf(start_angle * M_PI/180.f + (float)i * angle / (float)nbpts) + center.x;
        list->point.y = radius * sinf(start_angle * M_PI/180.f + (float)i * angle / (float)nbpts) + center.y;
    }
    list->next = NULL;
    return start->next;
}

linked_point_t* rounded_frame(Rect *rect, float radius, bt_part part)
{
    Point pt = rect->top_left;
    linked_point_t* list, *start;
    if(part != BT_BOTTOM){
        pt.x += radius;
        pt.y += (rect->size.height - radius);
        list = arc(pt, radius, 135, 180);
        start = list;
        while(list->next)
            list = list->next;
        pt.y -= (rect->size.height - 2.f * radius);
        list->next = arc(pt, radius, 180, 270);
        while(list->next)
            list = list->next;
        pt.x += (rect->size.width - 2.f * radius);
        list->next = arc(pt, radius, 270, 315);
        while(list->next)
            list = list->next;
    }

    if(part == BT_TOP) {
        list->next = (linked_point_t*) malloc(sizeof(linked_point_t));
        list = list->next;
        list->point.x = rect->top_left.x + 2*rect->size.width / 3;
        list->point.y = rect->top_left.y + rect->size.height / 2;
        list->next = (linked_point_t*) malloc(sizeof(linked_point_t));
        list = list->next;
        list->point.x = rect->top_left.x + rect->size.width / 3;
        list->point.y = rect->top_left.y + rect->size.height / 2;
        list->next = NULL;
        return start;
    }
    if(part == BT_BOTTOM){
        pt.x += rect->size.width - radius;
        pt.y += radius;
        list = arc(pt, radius, 315, 360);
        start = list;
    }else{
        list->next = arc(pt, radius, 315, 360);
    }

    list->next = arc(pt, radius, 315, 360);
    while(list->next)
        list = list->next;
    pt.y += (rect->size.height - 2.f * radius);
    list->next = arc(pt, radius, 0, 90);
    while(list->next)
        list = list->next;
    pt.x -= (rect->size.width - 2.f * radius);
    list->next = arc(pt, radius, 90, 135);
    while(list->next)
        list = list->next;
    if(part == BT_BOTTOM){
        list->next = (linked_point_t*) malloc(sizeof(linked_point_t));
        list = list->next;
        list->point.x = rect->top_left.x + rect->size.width / 3;
        list->point.y = rect->top_left.y + rect->size.height / 2;
        list->next = (linked_point_t*) malloc(sizeof(linked_point_t));
        list = list->next;
        list->point.x = rect->top_left.x + 2*rect->size.width / 3;
        list->point.y = rect->top_left.y + rect->size.height / 2;
        list->next = NULL;
    }
    return start;
}

void draw_line(surface_t surface, const Point& start,
                  const Point& end, const color_t& color,
                  const Rect* clipper)
{
    al_set_target_bitmap((ALLEGRO_BITMAP*) surface);
    if(clipper)
        al_set_clipping_rectangle(clipper->top_left.x, clipper->top_left.y, clipper->size.width, clipper->size.height);
    al_draw_line(start.x, start.y, end.x, end.y, al_map_rgba(color.red, color.green, color.blue, color.alpha), 1);
}

void draw_polyline(surface_t surface,
                      const linked_point_t* first_point,
                      const color_t color, const Rect* clipper)
{
    Point start, end;

    if (first_point == NULL)
        return;

    start = first_point->point;
    while (first_point->next != NULL) {
        end = first_point->next->point;
        draw_line(surface, start, end, color, clipper);
        start = end;
        first_point = first_point->next;
    }
}

static inline color_t alpha_blend(const color_t in_pixel, const color_t dst_pixel)
{
    color_t blended;
    float alpha = ((float)in_pixel.alpha) / 255.f;
    blended.red   = (1.f -  alpha) * dst_pixel.red   + alpha * in_pixel.red;
    blended.green = (1.f -  alpha) * dst_pixel.green + alpha * in_pixel.green;
    blended.blue  = (1.f -  alpha) * dst_pixel.blue  + alpha * in_pixel.blue;
    blended.alpha = 255;
    return blended;
}

typedef struct edge_t {
    int y_max;      //< Max ordinate
    int x_min;     //< Min abscissa
    int dx;         //< Displacement along x axis
    int dy;         //< Displacement along y axis
    int stepx;       //< Step along y axis (+/-1)
    int fraction;
    struct edge_t* next;
} edge_t;

/**
 * \brief   Compute min and max scanline (in the y direction) for the given polygon
 *
 * @param   first_point     The head of a linked list of the points of the polygon.
 * @param   min_scanline    Stores the minimum scanline
 * @param   min_scanline    Stores the maximum scanline
 */
void min_max_scanline(const linked_point_t* first_point, int* min_scanline, int* max_scanline)
{
    *min_scanline = first_point->point.y;
    *max_scanline = *min_scanline;
    const linked_point_t* lpoint = first_point->next;

    while (lpoint != NULL) {
        if (lpoint->point.y < *min_scanline)
            *min_scanline = lpoint->point.y;
        if (lpoint->point.y > *max_scanline)
            *max_scanline = lpoint->point.y;
        lpoint = lpoint->next;
    }
}

/**
 * \brief   Allocate and store all edges of the polygon into an edge_table (edge_t)
 *          in the increasing order of y and x.
 *          For example, edge_table[current_scanline] contains all edges for which
 *          lowest y position is current_scanline + min_scanline
 *
 * @param   first_point     The head of a linked list of the points of the polygon.
 * @param   min_scanline    The minimum scanline
 * @param   max_scanline    The maximum scanline
 */
edge_t** build_edge_table(const linked_point_t* first_point, int min_scanline, int max_scanline)
{
    // Allocate and initialize edge table
    edge_t** edge_table = (edge_t**)malloc((max_scanline - min_scanline + 1) * sizeof(edge_t*));
    for (int i = 0; i <= (max_scanline - min_scanline); i++)
        edge_table[i] = NULL;

    // Fill edge table
    Point start = first_point->point;
    const linked_point_t* lpoint = first_point->next;
    bool_t all_processed = EI_FALSE;
    edge_t* current_edge, *tmp_edge;
    int current_scanline;

    while (!all_processed) {
        if (lpoint == NULL) {
            lpoint = first_point;
            all_processed = EI_TRUE;
        }

        Point end = lpoint->point;
        // skip horizontal edges
        if (start.y != end.y) {
            current_edge = (edge_t*)malloc(sizeof(struct edge_t));
            if (start.y < end.y) {
                current_scanline = start.y;
                current_edge->y_max = end.y;
                current_edge->x_min = start.x;
                current_edge->dx = end.x - start.x;
                current_edge->dy = end.y - start.y;
            } else {
                current_scanline = end.y;
                current_edge->y_max = start.y;
                current_edge->x_min = end.x;
                current_edge->dx = start.x - end.x;
                current_edge->dy = start.y - end.y;
            }
            if (current_edge->dx < 0) {
                current_edge->dx = -current_edge->dx;
                current_edge->stepx = -1;
            } else {
                current_edge->stepx = 1;
            }
            if (current_edge->dx > current_edge->dy) {
                current_edge->fraction = current_edge->dy - current_edge->dx;
            } else {
                current_edge->fraction = current_edge->dx - current_edge->dy;
            }
            current_edge->dx = (current_edge->dx << 1);
            current_edge->dy = (current_edge->dy << 1);
            current_scanline -= min_scanline;

            // Insert in ET sorted by increasing y and x of the lower end
            if (edge_table[current_scanline] == NULL) {
                edge_table[current_scanline] = current_edge;
                current_edge->next = NULL;
            } else {
                tmp_edge = edge_table[current_scanline];
                while (tmp_edge->next != NULL && (tmp_edge->next->y_max <= current_edge->y_max)) {
                    if ((tmp_edge->next->y_max == current_edge->y_max) && (tmp_edge->next->x_min > current_edge->x_min))
                        break;
                    tmp_edge = tmp_edge->next;
                }
                current_edge->next = tmp_edge->next;
                tmp_edge->next = current_edge;
            }
        }
        // Process next edge
        start = end;
        lpoint = lpoint->next;
    }

    return edge_table;
}


/**
 * @brief Debugging functions to print the edge tables
 */
void print_edge_table_entry(edge_t* edge)
{
    while (edge != NULL) {
        printf("[%d, %d, %d, %d, %d, %d] ", edge->y_max,
               edge->x_min, edge->dx, edge->dy,
               edge->stepx, edge->fraction);
        edge = edge->next;
    }
    printf("\n");
}

void print_edge_table(edge_t** edge_table, int min_scanline, int max_scanline)
{
    edge_t* current_edge;
    printf("Edge table:\n");
    for (int i = 0; i <= (max_scanline - min_scanline); i++) {
        if (edge_table[i] != NULL) {
            printf("%d: ", i + min_scanline);
            current_edge = edge_table[i];
            print_edge_table_entry(current_edge);
        }
    }
    printf("\n");
}

void draw_polygon(surface_t surface, const linked_point_t* first_point,
                  const color_t& color, const Rect* clipper)
{
    edge_t* active_edge_table = NULL;
    edge_t* current_edge, * tmp_edge, * prev_edge;
    Point pos;
    int current_scanline;
    color_t p;

    if (first_point == NULL) {
        fprintf(stderr, "no point for the polygon\n");
        return;
    }

    hw_surface_lock(surface);

    // Compute min/max scanline
    int min_scanline, max_scanline;
    min_max_scanline(first_point, &min_scanline, &max_scanline);


    edge_t** edge_table = build_edge_table(first_point, min_scanline, max_scanline);

#ifdef DEBUG
    print_edge_table(edge_table, min_scanline, max_scanline);
#endif

    for (current_scanline = 0; current_scanline <= (max_scanline - min_scanline); current_scanline++) {
        // Move edges from ET[current_scanline] to AET sorted by increasing x of the lower end
        current_edge = edge_table[current_scanline];
        prev_edge = active_edge_table;
        while (current_edge != NULL) {
            if (active_edge_table == NULL) {
                active_edge_table = current_edge;
                break;
            } else {
                if (active_edge_table->x_min > current_edge->x_min) {
                    tmp_edge = current_edge->next;
                    current_edge->next = active_edge_table;
                    active_edge_table = current_edge;
                    current_edge = tmp_edge;
                } else {
                    while (prev_edge->next != NULL &&
                           prev_edge->next->x_min < current_edge->x_min) {
                        prev_edge = prev_edge->next;
                    }
                    tmp_edge = current_edge->next;
                    current_edge->next = prev_edge->next;
                    prev_edge->next = current_edge;
                    current_edge = tmp_edge;
                }
            }
        }

        // Remove from AET edges for wich y_max = current_scanline + min_scanline
        current_edge = active_edge_table;
        tmp_edge = NULL;
        while (current_edge != NULL) {
            if (current_edge->y_max == current_scanline + min_scanline) {
                if (tmp_edge == NULL)
                    active_edge_table = current_edge->next;
                else
                    tmp_edge->next = current_edge->next;
            } else {
                tmp_edge = current_edge;
            }
            current_edge = current_edge->next;
        }

#ifdef DEBUG
        printf("%d: ", current_scanline + min_scanline);
        print_edge_table_entry(active_edge_table)
#endif

        // Fill pixel values
        current_edge = active_edge_table;
        while (current_edge != NULL) {
            pos.y = current_scanline + min_scanline;
            for (pos.x = ceil(current_edge->x_min); pos.x <= floor(current_edge->next->x_min); pos.x++) {
                if (clipper == NULL ||
                        ((clipper->top_left.x <= pos.x) && (pos.x < clipper->top_left.x + clipper->size.width)
                         && (clipper->top_left.y <= pos.y) && (pos.y < clipper->top_left.y + clipper->size.height))) {
                    p = hw_get_pixel(surface, pos);
                    hw_put_pixel(surface, pos, alpha_blend(color, p));
                }
            }
            current_edge = current_edge->next->next;
        }

        current_edge = active_edge_table;
        tmp_edge = NULL;
        prev_edge = NULL;
        int i = 0;
        while (current_edge != NULL) {
            // Update next x_ymin using Bersenham
            if (current_edge->dx > current_edge->dy) {
                while (current_edge->fraction < 0) {
                    current_edge->x_min += current_edge->stepx;
                    current_edge->fraction += current_edge->dy;
                }
                current_edge->fraction -= current_edge->dx;
            } else {
                if (current_edge->fraction >= 0) {
                    current_edge->x_min += current_edge->stepx;
                    current_edge->fraction -= current_edge->dy;
                }
                current_edge->fraction += current_edge->dx;
            }

            // Make sure TCA remains sorted by increasing x of the lower end
            if ((tmp_edge != NULL) && (tmp_edge->x_min > current_edge->x_min)) {
                // swap entries
                if (prev_edge != NULL) {
                    prev_edge->next = current_edge;
                    tmp_edge->next = current_edge->next;
                    current_edge->next = tmp_edge;
                } else {
                    active_edge_table = current_edge;
                    tmp_edge->next = current_edge->next;
                    current_edge->next = tmp_edge;
                }
                current_edge = tmp_edge;
            }
            prev_edge = tmp_edge;
            tmp_edge = current_edge;
            current_edge = current_edge->next;
            i++;
        }
    }
    hw_surface_unlock(surface);
}


void draw_text(surface_t surface, const Point* where,
                  const char* text, const font_t font,
                  const color_t* color)
{
    if (text == NULL || color == NULL){
        fprintf(stderr, "no text or color specified");
        return;
    }

    surface_t s_text;

    if (font == NULL) {
        s_text = hw_text_create_surface(text, ei_default_font, color);
    } else {
        s_text = hw_text_create_surface(text, font, color);
    }

    ei_copy_surface(surface, s_text, where, EI_TRUE);

    hw_surface_free(s_text);
}

void fill(surface_t surface, const color_t* color, const bool_t use_alpha)
{
    const color_t* c = color == NULL ? &ei_font_default_color : color;

    al_set_target_bitmap((ALLEGRO_BITMAP*) surface);
    if(use_alpha == EI_TRUE)
        al_clear_to_color(al_map_rgba(c->red, c->green, c->blue, c->alpha));
    else
        al_clear_to_color(al_map_rgba(c->red, c->green, c->blue, 0xff));
}


void ei_copy_surface(surface_t destination, const surface_t source,
                     const Point* where, const bool_t use_alpha)
{
    if(use_alpha == EI_TRUE)
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
    else
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_ZERO);
    al_set_target_bitmap((ALLEGRO_BITMAP*) destination);
    al_draw_bitmap((ALLEGRO_BITMAP*) source, where->x, where->y, 0);
}

}
