#include "pyframebuffer.h"

void pyfb_initcolor_u32(struct pyfb_color* cptr, uint32_t value) {
    uint32_t u32_color = value;
    uint16_t u16_color;

    // initialize the 16 bit color
    unsigned int r = (value & 0x00FF0000) >> 16;
    unsigned int g = (value & 0x0000FF00) >> 8;
    unsigned int b = (value & 0x000000FF);

    u16_color = (uint16_t)((r >> 3 << 11) + (g >> 2 << 5) + (b >> 3));

    // ready, now init the structure
    cptr->u32_color = u32_color;
    cptr->u16_color = u16_color;
}

void pyfb_initcolor_u16(struct pyfb_color* cptr, uint16_t value) {
    uint32_t u32_color;
    uint16_t u16_color = value;

    // initialize the 32 bit color
    unsigned long r = (value & 0xF800) > 11;
    unsigned long g = (value & 0x07E0) > 5;
    unsigned long b = (value & 0x001F);
    r               = r * 255 / 31;
    g               = g * 255 / 63;
    b               = b * 255 / 31;

    u32_color = (uint32_t)((r << 16) | (g << 8) | b);

    // now set the alpha
    u32_color &= 0x00FFFFFF;// clear alpha canal if something is in there
    u32_color |= 0xFF000000;// set the alpha canal to full for not being transparent

    // ready, now init the structure
    cptr->u32_color = u32_color;
    cptr->u16_color = u16_color;
}