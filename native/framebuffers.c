#include <sys/ioctl.h>
#include <linux/fb.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include "pyframebuffer.h"


/**
 * The array with the framebuffers.
 */
static struct pyfb_framebuffer framebuffers[MAX_FRAMEBUFFERS];


void pyfb_init(void) {
    for(int i = 0; i < MAX_FRAMEBUFFERS; i++) {
        framebuffers[i].fb_fd = -1;
        framebuffers[i].users = 0;
        framebuffers[i].fb_info.fb_size_b = 0;
        framebuffers[i].u32_buffer = NULL;

        atomic_flag flag = ATOMIC_FLAG_INIT;

        framebuffers[i].fb_lock = flag;
    }
}

void __APISTATUS_internal pyfb_fblock(uint8_t fbnum) {
    // first test if this device number is valid
    if(fbnum >= MAX_FRAMEBUFFERS) {
        return;
    }

    lock(framebuffers[fbnum].fb_lock);
}

void __APISTATUS_internal pyfb_fbunlock(uint8_t fbnum) {
    // first test if this device number is valid
    if(fbnum >= MAX_FRAMEBUFFERS) {
        return;
    }

    unlock(framebuffers[fbnum].fb_lock);
}

/**
 * Set the uint8_t number into the fb device name string buffer.
 *
 * @param number The buffer to set the number into
 * @param fbnum The framebuffer number to set into the buffer
 */
static inline void setNum(char* number, uint8_t fbnum) {
    snprintf(number, 4, "%hhu", fbnum);
}

int pyfb_open(uint8_t fbnum) {
    // first test if this device number is valid.
    if(fbnum >= MAX_FRAMEBUFFERS) {
        return -2;
    }

    // Now try to open the framebuffer
    lock(framebuffers[fbnum].fb_lock);

    // first check if we need to open the framebuffer
    if(framebuffers[fbnum].fb_fd != -1) {
        // the framebuffer is allready opened,
        // so it is okay with just increment the user count and return
        framebuffers[fbnum].users++;
        unlock(framebuffers[fbnum].fb_lock);
        return 0;
    }

    // else if we get here, we must open and initialize the framebuffer structure
    char* fb_device = "/dev/fb000000";
    setNum(&fb_device[7], fbnum);

    // open the framebuffer
    int fb_fd = open(fb_device, O_RDWR);
    if(fb_fd == -1) {
        // failed to open the requested device
        unlock(framebuffers[fbnum].fb_lock);
        return -1;
    }

    // Now get the screen info
    struct fb_var_screeninfo* vinfo = &framebuffers[fbnum].fb_info.vinfo;
    if(ioctl(fb_fd, FBIOGET_VSCREENINFO, vinfo) == -1) {
        // failed to get the vinfo structure
        unlock(framebuffers[fbnum].fb_lock);
        close(fb_fd);
        return -1;
    }

    // Now alocate the two buffers
    unsigned long int fb_size_b = vinfo->yres_virtual * vinfo->xres_virtual * vinfo->bits_per_pixel / 8;

    // offscreen buffers
    void* buffer = calloc(fb_size_b, 1);

    if(buffer == NULL) {
        // got out of memory for the offscreen buffers
        if(buffer != NULL) {
            free(buffer);
        }

        // free up all other resources for the new buffer
        close(fb_fd);
        memset((void*)vinfo, 0, sizeof(struct fb_var_screeninfo));
        unlock(framebuffers[fbnum].fb_lock);
        return -1;
    }

    // if we get here, all is right and the structure can be filled.
    framebuffers[fbnum].users = 1;
    framebuffers[fbnum].fb_fd = fb_fd;

    if(vinfo->bits_per_pixel == 32) {
        framebuffers[fbnum].u32_buffer = (uint32_t*)buffer;
    }else{
        framebuffers[fbnum].u16_buffer = (uint16_t*)buffer;
    }

    framebuffers[fbnum].fb_info.fb_size_b = fb_size_b;

    // structure ready, return with success
    unlock(framebuffers[fbnum].fb_lock);
    return 0;
}

void pyfb_close(uint8_t fbnum) {
    // first test if this device number is valid.
    if(fbnum >= MAX_FRAMEBUFFERS) {
        return;
    }

    // Now try to open the framebuffer
    lock(framebuffers[fbnum].fb_lock);

    if(framebuffers[fbnum].users > 1) {
        // this means that we are not the only user of this framebuffer,
        // so that we can just release our reference on it
        framebuffers[fbnum].users--;
        unlock(framebuffers[fbnum].fb_lock);
        return;
    }

    // for wrong usage of this library
    if(framebuffers[fbnum].users == 0) {
        // should never happen, but if we get here, clean all up to not break internals
        printf("WARNING: Detected internal mismatch of libaray usage.\nPlease check your program or report if is a bug from our side.\n");
        
        // close file descriptor if is still opened
        if(framebuffers[fbnum].fb_fd >= 0) {
            close(framebuffers[fbnum].fb_fd);
        }

        framebuffers[fbnum].fb_fd = -1;

        // free all buffers if are active
        void** buffer;

        if(framebuffers[fbnum].fb_info.vinfo.bits_per_pixel == 32) {
            buffer = ((void*)&framebuffers[fbnum].u32_buffer);
        }else{
            buffer = ((void*)&framebuffers[fbnum].u16_buffer);
        }

        if(*buffer != NULL) {
            free(*buffer);
        }

        *buffer = NULL;

        // Now invalidate the videomode info
        framebuffers[fbnum].fb_info.fb_size_b = 0;
        memset((void*)&framebuffers[fbnum].fb_info.vinfo, 0, sizeof(struct fb_var_screeninfo));

        // ok, return
        unlock(framebuffers[fbnum].fb_lock);
        return;
    }

    // Okay, now handle a real close
    framebuffers[fbnum].users = 0;
    close(framebuffers[fbnum].fb_fd);
    framebuffers[fbnum].fb_fd = -1;

    // free the offscreen buffers
    void** buffer;

    if(framebuffers[fbnum].fb_info.vinfo.bits_per_pixel == 32) {
        buffer = ((void*)&framebuffers[fbnum].u32_buffer);
    }else{
        buffer = ((void*)&framebuffers[fbnum].u16_buffer);
    }

    if(*buffer != NULL) {
        free(*buffer);
    }

    *buffer = NULL;

    // and clean up the videomode info
    framebuffers[fbnum].fb_info.fb_size_b = 0;
    memset((void*)&framebuffers[fbnum].fb_info.vinfo, 0, sizeof(struct fb_var_screeninfo));

    // all cleaned up and resources free
    // can return now
    unlock(framebuffers[fbnum].fb_lock);
}

void pyfb_vinfo(uint8_t fbnum, struct pyfb_videomode_info* info_ptr) {
    // first test if this device number is valid.
    if(fbnum >= MAX_FRAMEBUFFERS) {
        return;
    }

    lock(framebuffers[fbnum].fb_lock);

    memcpy(info_ptr, &framebuffers[fbnum].fb_info.vinfo, sizeof(struct fb_var_screeninfo));

    unlock(framebuffers[fbnum].fb_lock);
}

void pyfb_flushBuffer(uint8_t fbnum) {
    // first test if this device number is valid
    if(fbnum >= MAX_FRAMEBUFFERS) {
        return;
    }

    lock(framebuffers[fbnum].fb_lock);

    // next, test if the device is really in use
    if(framebuffers[fbnum].fb_fd == -1) {
        // this framebuffer is not in use, so ignore
        unlock(framebuffers[fbnum].fb_lock);
        return;
    }

    // if we get here, flush the offscreen buffer to the framebuffer
    lseek(framebuffers[fbnum].fb_fd, 0L, SEEK_SET);
    if(framebuffers[fbnum].fb_info.vinfo.bits_per_pixel == 32) {
        write(framebuffers[fbnum].fb_fd, (void*)framebuffers[fbnum].u32_buffer, (size_t)framebuffers[fbnum].fb_info.fb_size_b);
    }else{
        write(framebuffers[fbnum].fb_fd, (void*)framebuffers[fbnum].u16_buffer, (size_t)framebuffers[fbnum].fb_info.fb_size_b);
    }

    // okay, ready flushed
    unlock(framebuffers[fbnum].fb_lock);
}

/**
 * Sets a pixel into a 32 bit framebuffer. Please only call if the pixel format is a
 * 32 bit rgba buffer.
 *
 * Warning: All arguments are unchecked
 *
 * @param fbnum The framebuffer number to use
 * @param x The x coordniate of the pixel
 * @param y The y coordinate of the pixel
 * @param color The color structure
 * @param xres The x resolution
 */
static inline void pyfb_pixel32(uint8_t fbnum, unsigned int x, unsigned int y, const struct pyfb_color* color, unsigned int xres) {
    framebuffers[fbnum].u32_buffer[y * xres + x] = color->u32_color;
}

/**
 * Sets a pixel into a 16 bit framebuffer. Please only call if the pixel format is a
 * 16 bit rgba buffer.
 *
 * Warning: All arguments are unchecked
 *
 * @param fbnum The framebuffer number to use
 * @param x The x coordniate of the pixel
 * @param y The y coordinate of the pixel
 * @param color The color structure
 * @param xres The x resolution
 */
static inline void pyfb_pixel16(uint8_t fbnum, unsigned long int x, unsigned long int y, const struct pyfb_color* color, unsigned int xres) {
    framebuffers[fbnum].u16_buffer[y * xres + x] = color->u16_color;
}

void __APISTATUS_internal pyfb_setPixel(uint8_t fbnum, unsigned long int x, unsigned long int y, const struct pyfb_color* color) {
    // do
    unsigned int xres = framebuffers[fbnum].fb_info.vinfo.xres;
    unsigned int width = framebuffers[fbnum].fb_info.vinfo.bits_per_pixel;
    if(width == 16) {
        pyfb_pixel16(fbnum, x, y, color, xres);
    }else{
        pyfb_pixel32(fbnum, x, y, color, xres);
    }
}

void pyfb_ssetPixel(uint8_t fbnum, unsigned long int x, unsigned long int y, const struct pyfb_color* color) {
    // first check if fbnum is valid
    if(fbnum >= MAX_FRAMEBUFFERS) {
        return;
    }

    // Is valid, so lock it!
    lock(framebuffers[fbnum].fb_lock);

    // check if all values are valid
    unsigned int xres = framebuffers[fbnum].fb_info.vinfo.xres;
    unsigned int yres = framebuffers[fbnum].fb_info.vinfo.yres;

    if(x >= xres || y >= yres) {
        // x or y is not valid
        unlock(framebuffers[fbnum].fb_lock);
        return;
    }

    // else all is okay and we can continue
    unsigned int width = framebuffers[fbnum].fb_info.vinfo.bits_per_pixel;
    if(width == 16) {
        pyfb_pixel16(fbnum, x, y, color, xres);
    }else{
        pyfb_pixel32(fbnum, x, y, color, xres);
    }

    // ready, so return
    unlock(framebuffers[fbnum].fb_lock);
}