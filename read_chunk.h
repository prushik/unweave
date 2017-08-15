struct chunk_reader
{
	int fd;
	char *buffer;
	int size;
	long pos,fpos;
	char eof;
};

#ifdef CHUNK_STATIC_BUFFER
static char chunk_static_buffer[CHUNK_STATIC_BUFFER];
#endif

int chunk_read(struct chunk_reader *chunk);
int chunk_init(struct chunk_reader *chunk, int fd);
int chunk_read_bytes(struct chunk_reader *chunk, char *data, int n);
void * chunk_tok(struct chunk_reader *chunk, char d, int *len);
