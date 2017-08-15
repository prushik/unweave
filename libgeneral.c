//This is libgeneral
//libgeneral is a collection of commonly used algorithms, but for which linux does not provide a syscall
//it's meant as a supplement for the header-only libc.h
//
//wtf though? why are all the functions given non-standard names?
//Well, thats so libgeneral can be used regardless of whether or not an external libc is used, we don't want conflicts.
//many of the libgeneral functions work slightly different from libc ones, making them incompatible anyways
#include "libc.h"
#include "libgeneral.h"
#include <sys/time.h>

//convert c terminated string to integer
int strtoi(char *s, char c)
{
	int i = 0;
	while (*s!=c)
	{
		i = i*10+*s-'0';
		s++;
	}
	return i;
}

//Don't know if you can tell, but this clearly isn't my code, style is all funky
void itostr(char* str, int len, unsigned int val)
{
	int i;

	for(i=1; i<=len; i++)
	{
		str[len-i] = (int) ((val % 10) + '0');
		val=val/10;
	}

	str[i-1] = '\0';
}

//Find character c in string s
//Why do I need this?
//update: maybe this is strtok ?
char *strchar(char *s, char c)
{
	int i;
	for (i=0; s[i]!='\0'; i++)
		if (s[i]==c)
				return &s[i];
	return 0;
}

//return size of s
int strsize(char *s)
{
	int i;
	for (i=0; s[i]!='\0'; i++) ;
	return i;
}

//Case-insensitive compare. s2 must be lower case.
int lstrcmp(char *s1, const char *s2, char end)
{
	int i;
	for (i=0; s1[i]!=end; i++)
		if (s1[i]!=s2[i] && s1[i]!=s2[i]-0x20)
				return 0;
	return 1;
}

//Compare n characters, case insensitive. s2 must be lower case.
int lstrncmp(char *s1, const char *s2, size_t n)
{
	int i;
	for (i=0; i<n; i++)
		if (s1[i]!=s2[i] && s1[i]!=s2[i]-0x20)
				return 0;
	return 1;
}

//Copy characters from one string to another until end
int strcopy(char *dest, const char *src, char end)
{
	int i;
	for (i=0; src[i]!=end; i++)
		dest[i]=src[i];
	return i;
}

//Copy n characters from one string to another
char *strncopy(char *dest, const char *src, size_t n)
{
	int i;
	for (i=0; i<n; i++)
		dest[i]=src[i];
	return dest;
}

//This counts set bits. Ideally we would use popcnt, but not everybody has sse4.2 yet
int count_bits(int n)
{
	unsigned int c;

	c = n - ((n >> 1) & 0x55555555);
	c = ((c >> 2) & 0x33333333) + (c & 0x33333333);
	c = ((c >> 4) + c) & 0x0F0F0F0F;
	c = ((c >> 8) + c) & 0x00FF00FF;
	c = ((c >> 16) + c) & 0x0000FFFF;

	return c;
}

int microsleep(uint32_t usec)
{
	struct timespec t;

	t.tv_sec = usec/1000;
	t.tv_nsec = (usec%1000)*1000000;

	nanosleep(&t,NULL);

	return 0;
}

void error(char *msg, int len)
{
	write(2, msg, len);
	exit(1);

	return;
}

//This is not getting implemented because it needs some help getting initialized
char *getenv(const char *env)
{
	//stub - get your env elsewhere
}
