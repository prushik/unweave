//Framebuffer Handling
//Author: Philip Rushik
//License: Beerware
//

#include <asm/mman.h>		//MMAP #defines
#include <asm/ioctl.h>		//IOCTL #defines
#include <linux/fb.h>		//FBDEV structures

static struct fb_fix_screeninfo finfo;
static struct fb_var_screeninfo vinfo;

static uint8_t *fbp, *bbp;

static int fb_fd;
static long screensize;

int fb_init_framebuffer(char *fb_fname)
{
	fb_fd = open(fb_fname,O_RDWR);

	//Get variable screen information
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
	vinfo.grayscale=0;
	vinfo.bits_per_pixel=32;
	vinfo.xres=1366;
	vinfo.yres=768;
//	vinfo.yres_virtual=768;
	ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

	ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

	screensize = vinfo.yres_virtual * finfo.line_length;

	fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);
	bbp = fbp;

	return fb_fd;
}

void fb_init_doublebuffer()
{
	bbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, (off_t)0);
}

inline uint32_t fb_pixel_color(uint8_t r, uint8_t g, uint8_t b)
{
	return (r<<vinfo.red.offset) | (g<<vinfo.green.offset) | (b<<vinfo.blue.offset);
}

inline void fb_draw(int x, int y, uint32_t pixel)
{
	long location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
	*((uint32_t*)(bbp + location)) = pixel;
}

inline void fb_clear(uint32_t pixel)
{
	int y,x;
	for (y=0;y<vinfo.yres;y++)
		for (x=0;x<vinfo.xres;x++)
			fb_draw(x,y,pixel);
}

inline void fb_clear_dirty(uint32_t pixel)
{
	int y,x;
	for (y=0;y<vinfo.yres;y++)
		for (x=0;x<vinfo.xres;x++)
			fb_draw(x,y,pixel);
}

//Really wish panning worked....
inline void fb_swap_buffers()
{
	int y,x;
	for (y=0;y<vinfo.yres;y++)
		for (x=0;x<vinfo.xres;x++)
		{
			long location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
			*((uint32_t*)(fbp + location)) = *((uint32_t*)(bbp + location));
		}
}
