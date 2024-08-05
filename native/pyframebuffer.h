/**
 * Main header of the native sources.
 */
#ifndef _pyframebuffer_included
#define _pyframebuffer_included

#include <Python.h>
#include <linux/fb.h>
#include <stdatomic.h>
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
#define __APISTATUS_deprecated \
    __attribute__((deprecated("This function is deprecated and will be removed in a future version.")))

#endif

/**
 * Typedefinition for a synchronization lock. Used for locking in a
 * multithreaded context.
 *
 * The general usage is:
 * \code{.c}
 * // initialization
 * lock_t sync_lock = LOCK_UNLOCKED;
 *
 * // locking
 * lock(sync_lock);
 *
 * // do here something were only one thread can exit at a time
 * ...
 *
 * // unlock the lock for the next thread
 * unlock(sync_lock);
 * \endcode
 */
typedef atomic_flag lock_t;

/**
 * Locks a lock.
 *
 * For more information see lock_t.
 */
#define lock(x) while(atomic_flag_test_and_set(&x))

/**
 * Unlocks a lock.
 *
 * For more information see lock_t.
 */
#define unlock(x) atomic_flag_clear(&x)

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
    struct fb_var_screeninfo vinfo;

    /**
     * The size of the framebuffer in bytes.
     */
    unsigned long int fb_size_b;
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
         * Used if the framebuffer depth (color bits) is 16bit.
         */
        uint16_t* u16_buffer;

        /**
         * Used if the framebuffer depth (color bits) is 32bit.
         */
        uint32_t* u32_buffer;
    };

    /**
     * The filedescriptor to the target framebuffer.
     */
    int fb_fd;

    /**
     * The count of users of this framebuffer.
     */
    unsigned long int users;

    /**
     * The lock on this framebuffer.
     */
    lock_t fb_lock;
};

/**
 * The color structure used as color for a framebuffer.
 */
struct pyfb_color {
    /**
     * Used if the color is in 32bit format.
     */
    uint32_t u32_color;

    /**
     * Used if the color is in 16bit format.
     */
    uint16_t u16_color;
};

/**
 * Initializes a color with the 32bit color value.
 *
 * @param value The color value in 32 bits
 * @param cptr The pointer to the color structure
 */
extern void pyfb_initcolor_u32(struct pyfb_color* cptr, uint32_t value);

/**
 * Initializes a color with the 16bit color value.
 *
 * @param value The color value in 16 bits
 * @param cptr The pointer to the color structure
 */
extern void pyfb_initcolor_u16(struct pyfb_color* cptr, uint16_t value);

/**
 * Initializes the pyfb internal structures. This function is only callen
 * at the beginning of module initialization and should not be callen
 * at another way.
 */
extern void __APISTATUS_internal pyfb_init(void);

/**
 * Opens a framebuffer. If it is allready opened, the implementation will remember
 * that the framebuffer is hold by multiple users and will free all resources to
 * this framebuffer only when the pyfb_close function is called the amount of times
 * this function has been called to be sure no user is using it.
 *
 * @param fbnum The framebuffer number to open, usually a number between 0 and 31
 *
 * @return By success 0, else -1, -2 if invalid framebuffer number
 */
extern int pyfb_open(uint8_t fbnum);

/**
 * Closes a framebuffer. This function does not closes the framebuffer automaticly!
 * If multiple times the pyfb_open function has been called on the framebuffer to
 * close, then it will only decrement the amount of users currently using this
 * framebuffer. Only if no other user is marked using this framebuffer, then all
 * resources related to the framebuffer will be cleaned up.
 *
 * @param fbnum The framebuffer number to close
 */
extern void pyfb_close(uint8_t fbnum);

/**
 * Returns the videomode info of a specific framebuffer. If the framebuffer is not
 * opened, then the @c pyfb_videomode_info.fb_size_b field will be @c 0 . If it is
 * not 0, then the structure is valid and the framebuffer is opened.
 * 
 * This is the secure version of pyfb_vinfo as it locks the framebuffer before
 * collecting the data.
 *
 * @param fbnum The framebuffer number to get the infos of
 * @param info_ptr The pointer to copy the videmode info to
 */
extern void pyfb_svinfo(uint8_t fbnum, struct pyfb_videomode_info* info_ptr);

/**
 * Returns the videomode info of a specific framebuffer. If the framebuffer is not
 * opened, then the @c pyfb_videomode_info.fb_size_b field will be @c 0 . If it is
 * not 0, then the structure is valid and the framebuffer is opened.
 * 
 * This is the insecure version of pyfb_vinfo as it not locks the framebuffer before
 * collecting the data.
 *
 * @param fbnum The framebuffer number to get the infos of
 * @param info_ptr The pointer to copy the videmode info to
 */
extern void __APISTATUS_internal pyfb_vinfo(uint8_t fbnum, struct pyfb_videomode_info* info_ptr);

/**
 * Paints a single pixel to the framebuffer. This function is secure because before
 * painting, it validates the arguments.
 *
 * @param fbnum The number of the target framebuffer
 * @param x The x coordinate of the pixel
 * @param y The y coordinate of the pixel
 * @param color The color structure
 */
extern void pyfb_ssetPixel(uint8_t fbnum, unsigned long int x, unsigned long int y, const struct pyfb_color* color);

/**
 * Paints a single pixel to the framebuffer. This function is the insecure way because due
 * to performance increase, the arguments will not be checked. Please make sure
 * the arguments are correct, because it may cause a memory error if the data is
 * invalid. Also note that this function is marked as __APISTATUS_internal. Means
 * this is internal API and sould not be accessible from outside.
 *
 * This function by itself does not handle the locking of the framebuffer. The caller must
 * care of locking the framebuffer before calling this function.
 *
 * @param fbnum The number of the target framebuffer
 * @param x The x coordinate of the pixel
 * @param y The y coordinate of the pixel
 * @param color The color structure
 */
extern void __APISTATUS_internal pyfb_setPixel(uint8_t fbnum,
                                               unsigned long int x,
                                               unsigned long int y,
                                               const struct pyfb_color* color);

/**
 * Locks a framebuffer. Before using the pyfb_setPixel function, this function must be callen to
 * lock the framebuffer for synchonized access. Please remember to unlock the framebuffer with the
 * pyfb_fbunlock function to indicate that the work on this framebuffer is finished and the next
 * thread can continue working. If not removing the lock, this would end up in a infinite loop for
 * this thread by the next operation, and others. Please note the __APISTATUS_internal macro with
 * which this function is marked. This means that this function should not be used outside this
 * native implementation.
 *
 * @param fbnum The number of the framebuffer to lock
 */
extern void __APISTATUS_internal pyfb_fblock(uint8_t fbnum);

/**
 * Unlocks a framebuffer. This function must be callen when the work for a framebuffer is done. It is
 * used to unlock a framebuffer after it has been locked via the pyfb_fblock function.
 *
 * @param fbnum The number of the framebuffer to unlock
 */
extern void __APISTATUS_internal pyfb_fbunlock(uint8_t fbnum);

/**
 * Checks if the framebuffer is really opened. This is only used internally as checking function before
 * executing a draw operation to prevent NULL pointer access.
 * 
 * The access should work like this:
 * \code{.c}
 * if(!pyfb_fbused(fbnum)) {
 *     // any error handling
 *     return;
 * }
 * 
 * // if get here, the framebuffer is in use
 * \endcode
 * 
 * Please lock the framebuffer before invoking this function. This ensures that the status request is keept
 * up to date or is not modified at the same time this function is testing it.
 * 
 * @param fbnum The framebuffer number to check
 * 
 * @return If is in use 1, else 0
 */
extern int __APISTATUS_internal pyfb_fbused(uint8_t fbnum);

/**
 * Paints a exactly horizontal line. This function is secure because before painting,
 * it validates the arguments.
 *
 * @param fbnum The number of the target framebuffer
 * @param y The row on which to draw the line
 * @param x The starting x coordinate (included)
 * @param len The length of the line in pixel, means first pixel is painted on the x argument coordinate
 * @param color The color structure
 */
extern void pyfb_sdrawHorizontalLine(uint8_t fbnum,
                                     unsigned long int x,
                                     unsigned long int y,
                                     unsigned long int len,
                                     const struct pyfb_color* color);

/**
 * Paints a exactly horizontal line. This function is the insecure way because due to performance
 * increase, the arguments will not be checked. Please make sure
 * the arguments are correct, because it may cause a memory error if the data is
 * invalid. Also note that this function is marked as __APISTATUS_internal. Means
 * this is internal API and sould not be accessible from outside.
 *
 * This function by itself does not handle the locking of the framebuffer. The caller must
 * care of locking the framebuffer before calling this function.
 *
 * @param fbnum The number of the target framebuffer
 * @param y The row on which to draw the line
 * @param x The starting x coordinate (included)
 * @param len The length of the line in pixel, means first pixel is painted on the x argument coordinate
 * @param color The color structure
 */
extern void __APISTATUS_internal pyfb_drawHorizontalLine(uint8_t fbnum,
                                                         unsigned long int x,
                                                         unsigned long int y,
                                                         unsigned long int len,
                                                         const struct pyfb_color* color);

/**
 * Paints a exactly vertical line. This function is secure, because before painting,
 * it validates the arguments.
 *
 * @param fbnum The number of the target framebuffer
 * @param y The starting y coordinate (included)
 * @param x The column on which to draw the line
 * @param len The length of the line in pixel, means first pixel is painted on the y argument coordinate
 * @param color The color structure
 */
extern void pyfb_sdrawVerticalLine(uint8_t fbnum,
                                   unsigned long int x,
                                   unsigned long int y,
                                   unsigned long int len,
                                   const struct pyfb_color* color);

/**
 * Paints a exactly vertical line. This function is the insecure way because due to performance
 * increase, the arguments will not be checked. Please make sure
 * the arguments are correct, because it may cause a memory error if the data is
 * invalid. Also note that this function is marked as __APISTATUS_internal. Means
 * this is internal API and sould not be accessible from outside.
 *
 * This function by itself does not handle the locking of the framebuffer. The caller must
 * care of locking the framebuffer before calling this function.
 *
 * @param fbnum The number of the target framebuffer
 * @param y The starting y coordinate (included)
 * @param x The column on which to draw the line
 * @param len The length of the line in pixel, means first pixel is painted on the y argument coordinate
 * @param color The color structure
 */
extern void __APISTATUS_internal pyfb_drawVerticalLine(uint8_t fbnum,
                                                       unsigned long int x,
                                                       unsigned long int y,
                                                       unsigned long int len,
                                                       const struct pyfb_color* color);

/**
 * Draws a line from one Point to another. This function is the insecure way because due to
 * performance increase, the arguments will not be checked. Please make sure the arguments
 * are correct, because it may cause a memory error if the data is invalid. Also note that this
 * function is marked as __APISTATUS_internal. Means this is an internal API and should not be
 * accessible from outside.
 * 
 * This function by it self does not handle the locking of the framebuffer. The caller must
 * care of locking the framebuffer before calling this function.
 * 
 * @param fbnum The number of the framebuffer
 * @param x1 The x1 coordinate
 * @param y1 The y1 coordinate
 * @param x2 The x2 coordinate
 * @param y2 The y2 coordinate
 * @param color The color value
 */
extern void __APISTATUS_internal pyfb_drawLine(uint8_t fbnum,
                                               unsigned long int x1,
                                               unsigned long int y1,
                                               unsigned long int x2,
                                               unsigned long int y2,
                                               const struct pyfb_color* color);

/**
 * Draws a line. This function is secure, because before painting,
 * it validates the arguments.
 *
 * @param fbnum The number of the framebuffer
 * @param x1 The x1 coordinate
 * @param y1 The y1 coordinate
 * @param x2 The x2 coordinate
 * @param y2 The y2 coordinate
 * @param color The color value
 */
extern void __APISTATUS_internal pyfb_sdrawLine(uint8_t fbnum,
                                                unsigned long int x1,
                                                unsigned long int y1,
                                                unsigned long int x2,
                                                unsigned long int y2,
                                                const struct pyfb_color* color);

/**
 * Safe version to draw a circle on the screen.
 * 
 * @param fbnum The framebuffer number
 * @param xm The x coordinate of the middle point
 * @param ym The y coordinate of the middle point
 * @param radius The circle radius
 * @param color The color value
 */
extern void pyfb_sdrawCircle(uint8_t fbnum,
                             unsigned long int xm,
                             unsigned long int ym,
                             unsigned long int radius,
                             struct pyfb_color* color);

/**
 * Unsafe version to draw a circle on the screen.
 * 
 * @param fbnum The framebuffer number
 * @param xm The x coordinate of the middle point
 * @param ym The y coordinate of the middle point
 * @param radius The circle radius
 * @param color The color value
 */
extern void __APISTATUS_internal pyfb_drawCircle(uint8_t fbnum,
                                                 unsigned long int xm,
                                                 unsigned long int ym,
                                                 unsigned long int radius,
                                                 struct pyfb_color* color);

/**
 * Paints the content of the offscreen buffer to the framebuffer. This function must be callen
 * because this is the only operation that is required to paint the content of the offscreen
 * buffer to the framebuffer. All other paint operations are only for painting to the offscreen
 * buffer, that must be flushed to the framebuffer with this function to display all content.
 *
 * This function will block until the buffer content is fully transfered to the framebuffer.
 * As this operation must be transfered via DMA, this still can take a while. In the internet,
 * it says that it can take something between 20 and 100 milliseconds.
 *
 * @param fbnum The framebuffer number of which to flush all buffers
 *
 * @return If succeeded 0, else -1
 */
extern int pyfb_flushBuffer(uint8_t fbnum);

#endif