#ifndef _pyframebuffer_included
#define _pyframebuffer_included

#include <linux/fb.h>

#include <stdint.h>

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
#if defined(__GNUC__) && !defined(__DOXYGEN__)

#undef __APISTATUS_deprecated
#define __APISTATUS_deprecated __attribute__((deprecated("This function is deprecated and will be removed in a future version.")))

#endif

/**
 * The maximum amount of framebuffers that this library can handle.
 * Normally as the device file @c /dev/fbXX can have a number between
 * @c 0-31 , this means that @c 32 framebuffers can be handled, and
 * these we will also handle here.
 */
#define MAX_FRAMEBUFFERS 32

/**
 * Used for storing the videomode information.
 */
struct pyfb_videomode_info {
    /**
     * The screeninfo from the framebuffer.
     */
    const struct fb_var_screeninfo vinfo;

    /**
     * The size of the framebuffer in bytes.
     */
    const unsigned long int fb_size_b;
};

/**
 * Used for store framebuffer information internally.
 */
struct pyfb_framebuffer {
    
    /**
     * The framebuffer information.
     */
    struct pyfb_videomode_info fb_info;

    /**
     * The offscreen buffer for the framebuffer. The buffer to use of these
     * in this union depend on the framebuffer depth. To get the framebuffer
     * depth, use the @c pyfb_videomode_info.vinfo.bits_per_pixel field.
     */
    union {
        /**
         * Used if the framebuffer depth (color bits) is 8bit.
         */
        uint8_t* u8_buffer;

        /**
         * Used if the framebuffer depth (color bits) is 16bit.
         */
        uint16_t* u16_buffer;

        /**
         * Used if the framebuffer depth (color bits) is 32bit.
         */
        uint32_t* u32_buffer;
    };
};

/**
 * Opens a framebuffer. If it is allready opened, the implementation will remember
 * that the framebuffer is hold by multiple users and will free all resources to
 * this framebuffer only when the pyfb_close function is called the amount of times
 * this function has been called to be sure no user is using it.
 *
 * @param num The framebuffer number to open, usually a number between 0 and 31
 *
 * @return By success 0, else -1
 */
extern int pyfb_open(uint8_t num);

/**
 * Closes a framebuffer. This function does not closes the framebuffer automaticly!
 * If multiple times the pyfb_open function has been called on the framebuffer to
 * close, then it will only decrement the amount of users currently using this
 * framebuffer. Only if no other user is marked using this framebuffer, then all
 * resources related to the framebuffer will be cleaned up.
 *
 * @param num The framebuffer number to close
 */
extern void pyfb_close(uint8_t num);

/**
 * Returns the videomode info of a specific framebuffer. If the framebuffer is not
 * opened, then the @c pyfb_videomode_info.fb_size_b field will be @c 0 . If it is
 * not 0, then the structure is valid and the framebuffer is opened.
 *
 * @param num The framebuffer number to get the infos of
 * @param info_ptr The pointer to copy the videmode info to
 */
extern void pyfb_vinfo(uint8_t num, struct pyfb_videomode_info* info_ptr);

/**
 * Checks if the framebuffer number is opened.
 *
 * @param num The framebuffer number to check
 *
 * @return If opened returns 0, else -1
 */
extern int pyfb_isopen(uint8_t num);

#endif