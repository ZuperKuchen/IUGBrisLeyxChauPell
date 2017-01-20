/**
 * @file  ei_types.h
 *
 * @brief   Type, constant, and global definitions for the ei library.
 *
 *  Created by François Bérard on 18.12.11.
 *  Modified by Pierre Bénard and Aurélie Bugeau
 *
 */

#ifndef EI_TYPES_H
#define EI_TYPES_H

#include <stdint.h>
#include <vector>
#include <string>

namespace ei {

/********** Basic. **********/

/**
 * @brief The boolean type used in the library.
 */
typedef enum { EI_FALSE = 0, EI_TRUE = 1 } bool_t;

/********** Tags. **********/

/**
 * @brief A string that can be attached to a widget.
 *        All widget have the tag of the name of their widget class, and the tag "all".
 */
typedef std::string tag_t;

/**
 * @brief A tag and a pointer to create a linked list.
 */
typedef struct linked_tag_t {
    tag_t tag;
    struct linked_tag_t* next;
} linked_tag_t;

/********** Drawing. **********/

/**
 * @brief A 2-D point with integer coordinates.
 */
class Point {
public:
  int x;  ///< The abscissa of the point. The origin is on the left side of the image.
  int y;  ///< The ordinate of the point, the origin is at the top of the image,
          ///  ordinates increase towards the bottom.

  Point(int x=0, int y=0)
      : x(x), y(y) {}

  Point(const Point &p) {
      x = p.x;
      y = p.y;
  }

  Point& operator-(const Point &p) {
      x = -p.x;
      y = -p.y;
      return *this;
  }

  Point& operator=(Point p) {
      x = p.x;
      y = p.y;
      return *this;
  }

  friend Point operator+(const Point &p1, const Point &p2) {
      return Point(p1.x+p2.x, p1.y+p2.y);
  }

  friend Point operator-(const Point &p1, const Point &p2) {
      return Point(p1.x-p2.x, p1.y-p2.y);
  }
};

/**
 * @brief A 2-D size with integer dimensions.
 */
class Size {
public:
  float width;
  float height;

  Size(float w=0, float h=0)
      : width(w), height(h) {}

  friend Size operator+(const Size &s1, const Size &s2) {
      return Size(s1.width+s2.width, s1.height+s2.height);
  }

  friend Size operator-(const Size &s1, const Size &s2) {
      return Size(s1.width-s2.width, s1.height-s2.height);
  }
};

/**
 * @brief A rectangle defined by its top-left corner, and its size.
 */
class Rect {
public:
  Point top_left;  ///< Coordinates of the top-left corner of the rectangle.
  Size size;       ///< Size of the rectangle.
  Rect(Point top_left = Point(), Size size = Size())
      : top_left(top_left), size(size) {}
};

/**
 * @brief A rectangle plus a pointer to create a linked list.
 */
typedef struct linked_rect_t {
  Rect rect;  ///< The rectangle.
  struct linked_rect_t* next;  ///< The pointer to the next element in the linked list.
} linked_rect_t;

/**
 * @brief A point plus a pointer to create a linked list.
 */
typedef struct linked_point_t {
  Point point;  ///< The point.
  struct linked_point_t* next;  ///< The pointer to the next element in the linked list.
} linked_point_t;

/**
 * @brief A color with transparency.
 *
 *    Each channel is represented as an 8 bits unsigned interger,
 *    hence channel's minimum value is 0, maximum is 255.
 */
typedef struct {
  unsigned char red;    ///< The red component of the color.
  unsigned char green;  ///< The green component of the color.
  unsigned char blue;   ///< The blue component of the color.
  unsigned char alpha;  ///< The transparency of the color. 0 is invisible,
                        ///  255 is totally opaque.
} color_t;

/**
 * @brief The default background color of widgets.
 */
static const color_t ei_default_background_color = {0xA0, 0xA0, 0xA0, 0xff};

/**
 * @brief Identifies one particular point of a rectangle.
 */
typedef enum {
  ei_anc_none = 0,   ///< No anchor defined.
  ei_anc_center,     ///< Anchor in the center.
  ei_anc_north,      ///< Anchor on the top side, centered horizontally.
  ei_anc_northeast,  ///< Anchor on the top-right corner.
  ei_anc_east,       ///< Anchor on the right side, centered vertically.
  ei_anc_southeast,  ///< Anchor on the bottom-right corner.
  ei_anc_south,      ///< Anchor on the bottom side, centered horizontally.
  ei_anc_southwest,  ///< Anchor on the bottom-left corner.
  ei_anc_west,       ///< Anchor on the left side, centered vertically.
  ei_anc_northwest   ///< Anchor on the top-left corner.
} anchor_t;

/**
 * @brief Type of relief.
 */
typedef enum {
  ei_relief_none = 0,  ///< No relief (i.e. flat).
  ei_relief_raised,    ///< Above the screen.
  ei_relief_sunken     ///< Inside the screen.
} relief_t;

/**
 * @brief Set of axis.
 */
typedef enum {
  ei_axis_none = 0,  ///< No axis.
  ei_axis_x,         ///< Horizontal axis.
  ei_axis_y,         ///< Vertical axis.
  ei_axis_both       ///< Both horizontal and vertical axis.
} axis_set_t;

/********** Fonts management. **********/

/**
 * @brief An opaque type for handling fonts.
 *
 *    Fonts are created by calling \ref hw_text_font_create and released
 *    by calling \ref hw_text_font_free.
 */
typedef void* font_t;

/**
 * @brief The default font used in widgets.
 */
extern font_t ei_default_font;

static const int ei_font_default_size = 22;  ///< Default font color
static const color_t ei_font_default_color = {0x00, 0x00, 0x00, 0xff};
static const char ei_default_font_filename[] = DATA_DIR"font.ttf";

/********** Banner management. **********/
static const color_t ei_default_banner_color = {0xA0, 0xA0, 0xD0, 0xff};
static const char ei_default_banner_filename[] = DATA_DIR"cross.png";

static const int ei_default_button_border_width  = 4;    ///< The default border width of button widgets.
static const int ei_default_button_corner_radius = 10;   ///< The default corner radius of button widgets.

}

#endif
