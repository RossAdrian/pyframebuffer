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
        framebuffers[i].buffers[0].u32_buffer = NULL;
        framebuffers[i].buffers[1].u32_buffer = NULL;

        atomic_flag flag = ATOMIC_FLAG_INIT;

        framebuffers[i].fb_lock = flag;
    }
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
    void* buffer1 = calloc(fb_size_b, 1);
    void* buffer2 = calloc(fb_size_b, 1);

    if(buffer1 == NULL || buffer2 == NULL) {
        // got out of memory for the offscreen buffers
        if(buffer1 != NULL) {
            free(buffer1);
        }
        if(buffer2 != NULL) {
            free(buffer2);
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
    framebuffers[fbnum].used_buffer = BUFFER_USED_0;

    if(vinfo->bits_per_pixel == 32) {
        framebuffers[fbnum].buffers[0].u32_buffer = (uint32_t*)buffer1;
        framebuffers[fbnum].buffers[1].u32_buffer = (uint32_t*)buffer2;
    }else{
        framebuffers[fbnum].buffers[0].u16_buffer = (uint16_t*)buffer1;
        framebuffers[fbnum].buffers[1].u16_buffer = (uint16_t*)buffer2;
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
        void** buffer1;
        void** buffer2;

        if(framebuffers[fbnum].fb_info.vinfo.bits_per_pixel == 32) {
            buffer1 = ((void*)&framebuffers[fbnum].buffers[0].u32_buffer);
            buffer2 = ((void*)&framebuffers[fbnum].buffers[1].u32_buffer);
        }else{
            buffer1 = ((void*)&framebuffers[fbnum].buffers[0].u16_buffer);
            buffer2 = ((void*)&framebuffers[fbnum].buffers[1].u16_buffer);
        }

        if(*buffer1 != NULL) {
            free(*buffer1);
        }
        if(*buffer2 != NULL) {
            free(*buffer2);
        }

        *buffer1 = NULL;
        *buffer2 = NULL;

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
    framebuffers[fbnum].used_buffer = BUFFER_USED_0;

    // free the offscreen buffers
    void** buffer1;
    void** buffer2;

    if(framebuffers[fbnum].fb_info.vinfo.bits_per_pixel == 32) {
        buffer1 = ((void*)&framebuffers[fbnum].buffers[0].u32_buffer);
        buffer2 = ((void*)&framebuffers[fbnum].buffers[1].u32_buffer);
    }else{
        buffer1 = ((void*)&framebuffers[fbnum].buffers[0].u16_buffer);
        buffer2 = ((void*)&framebuffers[fbnum].buffers[1].u16_buffer);
    }

    if(*buffer1 != NULL) {
        free(*buffer1);
    }
    if(*buffer2 != NULL) {
        free(*buffer2);
    }

    *buffer1 = NULL;
    *buffer2 = NULL;

    // and clean up the videomode info
    framebuffers[fbnum].fb_info.fb_size_b = 0;
    memset((void*)&framebuffers[fbnum].fb_info.vinfo, 0, sizeof(struct fb_var_screeninfo));

    // all cleaned up and resources free
    // can return now
    unlock(framebuffers[fbnum].fb_lock);
}