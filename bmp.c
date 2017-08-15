#include "bmp.h"
#include "libc.h"
#include <asm/mman.h>		//MMAP #defines

struct bmp_file_header
{
	uint16_t magic;
	uint32_t fsize;
	uint32_t garbage;
	uint32_t data_offset;
};

struct bmp_img_header
{
	uint32_t img_header_size;
	uint32_t w,h;
	uint16_t planes;
	uint16_t bpp;
	uint32_t enc;
	uint32_t data_size;
	uint32_t hres,vres;
	uint32_t colors,i_colors;
};

struct bmp_image
{
	struct bmp_file_header file_header;
	struct bmp_img_header img_header;
	void *pixel;
	int index;
};

static struct bmp_image *bmp[512];

static int bmpc=0;

inline void draw_bmp(int x, int y, int index)
{
	int i,j;
	for (i=0;i<bmp[index]->img_header.h;i++)
		for (j=0;j<bmp[index]->img_header.w;j++)
			if ((*((uint32_t*)(bmp[index]->pixel)+(j+i*bmp[index]->img_header.w)))&0xff000000)
				draw(x+j,y+i,(*((uint32_t*)(bmp[index]->pixel)+(j+i*bmp[index]->img_header.w))));
}

inline void draw_bmp_filter(int x, int y, int index, uint32_t filter)
{
	int i,j;
	for (i=0;i<bmp[index]->img_header.h;i++)
		for (j=0;j<bmp[index]->img_header.w;j++)
			if ((*((uint32_t*)(bmp[index]->pixel)+(j+i*bmp[index]->img_header.w)))&0xff000000)
				draw(x+j,y+i,(*((uint32_t*)(bmp[index]->pixel)+(j+i*bmp[index]->img_header.w)))&filter);
}

inline void draw_bmp_part(int dest_x, int dest_y, int source_x, int source_y, int source_w, int source_h, int index)
{
	int x,y;
	for (y=source_y;y<source_y+source_h;y++)
		for (x=source_x;x<source_x+source_w;x++)
			if ((*((uint32_t*)(bmp[index]->pixel)+(x+y*bmp[index]->img_header.w)))&0xff000000)
				draw(dest_x+x-source_x,dest_y+y-source_y,(*((uint32_t*)(bmp[index]->pixel)+(x+y*bmp[index]->img_header.w))));
}

inline void draw_bmp_rev_part(int dest_x, int dest_y, int source_x, int source_y, int source_w, int source_h, int index)
{
	int x,y;
	for (y=source_y;y<source_y+source_h;y++)
		for (x=source_x+source_w;x>source_x;x--)
			if ((*((uint32_t*)(bmp[index]->pixel)+(x+y*bmp[index]->img_header.w)))&0xff000000)
				draw(dest_x+(source_w-x)+source_x,dest_y+y-source_y,(*((uint32_t*)(bmp[index]->pixel)+(x+y*bmp[index]->img_header.w))));
}

int load_bmp(char *bmp_fname)
{
	int bmp_fd;

	bmp_fd = open(bmp_fname,O_RDONLY);

	//New bmp
	bmpc=(bmpc+1)%512; //Increase the bmp count, but don't overflow
	bmp[bmpc]=sbrk(sizeof(struct bmp_image));
	struct bmp_image *img=bmp[bmpc];
	img->index=bmpc;

	read(bmp_fd,(void *)&img->file_header,2);
	if (img->file_header.magic!=0x4D42)
	{
		//Not a BMP
		printf("E: Not bmp (%X)\n",img->file_header.magic);
		return 0;
	}

	//Read the rest of the file header
	read(bmp_fd,(void *)&img->file_header.fsize,12);
	//Notice here that we cannot just start reading into the memory directly 
	//after file_header.magic because the compiler will add padding between 
	//the fields magic and fsize

	//Read the image header
	read(bmp_fd,(void *)&img->img_header,sizeof(struct bmp_img_header));

	if (img->img_header.enc!=0)
	{
		//Unsupported compression
		printf("E: Encoded\n");
		return 0;
	}

	//Jump to the pixel data
	lseek(bmp_fd,img->file_header.data_offset,SEEK_SET);

	//Allocate the memory for the pixel data
	img->pixel = mmap(0, img->img_header.w*(img->img_header.h)*4, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

	int bytes_per_pixel = img->img_header.bpp/8;

	//temporary storage for 1 pixel fo data
	uint32_t *tmp_pixel = (void*)sbrk(bytes_per_pixel);

	int y,x;
	for (y=0;y<img->img_header.h;y++)
	{
		for (x=0;x<img->img_header.w;x++)
		{
			//Read the pixel into the temporary buffer
			read(bmp_fd,tmp_pixel,bytes_per_pixel);

			uint32_t *location;
			location = img->pixel + (((img->img_header.h-y) * img->img_header.w) + (x)) * 4;//bytes_per_pixel;

			if (bytes_per_pixel==2)
					*location = (*tmp_pixel&0x7C00)<<6|(*tmp_pixel&0x03E0)<<3|(*tmp_pixel&0x001F); //not tested
			else if (bytes_per_pixel==3)
					*location = (*tmp_pixel)|0xFF000000; //I just hard code 0xFF in for the destination alpha
			else if (bytes_per_pixel==4)
					*location = *tmp_pixel; //on little endian systems, the BMP data is already in the correct format
		}
	}

	//Free the temporary memory
	sbrk(-(bytes_per_pixel));

	close(bmp_fd);

	//Return the index of the BMP
	return img->index;
}
