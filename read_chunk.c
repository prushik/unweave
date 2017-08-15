#include "libc.h"
#include "read_chunk.h"
#include <linux/fs.h>


//don't call this, its for internal use and calling it will confuse things
inline int chunk_read(struct chunk_reader *chunk)
{
	lseek(chunk->fd, chunk->fpos, SEEK_SET);
	if (!chunk->eof)
	{
		int r = read(chunk->fd, chunk->buffer, chunk->size);
		chunk->eof = (r != chunk->size);
		chunk->pos = 0;
		return chunk->eof;
	}
}

int chunk_init(struct chunk_reader *chunk, int fd)
{
	chunk->fd = fd;
	chunk->eof = 0;
	#ifdef CHUNK_STATIC_BUFFER
		chunk->buffer = chunk_static_buffer;
		chunk->size = CHUNK_STATIC_BUFFER;
	#else
		#ifdef CHUNK_BUFFER_SIZE
			chunk->buffer = sbrk(CHUNK_BUFFER_SIZE);
			chunk->size = CHUNK_BUFFER_SIZE;
		#else
			chunk->buffer = sbrk(2048);
			chunk->size = 2048;
		#endif
	#endif
	chunk->pos=0;
	chunk->fpos=0;
	chunk_read(chunk);
}

int chunk_read_bytes(struct chunk_reader *chunk, char *data, int n)
{
	//stub for now, strcopy will be needed
}

//get a pointer to next delimiter d
void * chunk_tok(struct chunk_reader *chunk, char d, int *len)
{
	int i=chunk->pos;
	void *data = &chunk->buffer[i];

	for (; chunk->buffer[i] != d; i++)
	{
		if (i > chunk->size)
		{
			if (chunk->eof)
				break;

			i=0;
			chunk->fpos += chunk->pos;
			chunk_read(chunk);
			data = chunk->buffer;
		}
	}
	chunk->buffer[i] = '\0';
	*len = i-chunk->pos;
	chunk->pos = i + 1;

	return data;
}
