#ifndef _pyframebuffer_included
#define _pyframebuffer_included

#include <linux/fb.h>

// decorators for marking functions and variables

/**
 * Functions decorated with this macro are only for internal usage.
 */
#define __APISTATUS_internal

/**
 * Functions decorated with this macro are deprecated and for removal.
 */
#define __APISTATUS_deprecated

// if GNUC, also add a GNUC attribute for deprecation
#if defined(__GNUC__)

#undef __APISTATUS_deprecated
#define __APISTATUS_deprecated __attribute__((deprecated("This function is deprecated and will be removed in a future version.")))

#endif

/**
 * Used for storing the videomode information.
 */
struct pyfb_videmode
{
    /**
     * The screeninfo from the framebuffer.
     */
    const struct fb_var_screeninfo vinfo;
};

#endif