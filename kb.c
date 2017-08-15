#include <linux/input.h>	//input_event structure

static uint8_t key[248];

//Get the next event from the event device file
//Should generally be called in a loop until there are no events left
int kb_get_keys(int kb_fd)
{
	struct input_event ev;
	int r = read(kb_fd,&ev,sizeof(struct input_event));
	if (r>0)
	{
		if (ev.type == 1)
		{
			key[ev.code]=ev.value;
		}
		return 1;
	}
	else
		return 0;
}

//Returns the state of key k set by kb_get_keys()
inline int kb_key_state(int k)
{
	return key[k];
}

//Open the event file
int kb_init(char *fname)
{
	int ev_fd;

	//Open keyboard
	ev_fd = open(fname, O_RDONLY|O_NONBLOCK);

	if (ev_fd!=-1)
		return ev_fd;
	else
		return 0;
}
