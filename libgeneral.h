int strtoi(char *s, char c);

void itostr(char* str, int len, unsigned int val);

char *strchar(char *s, char c);

int strsize(char *s);

int lstrcmp(char *s1, const char *s2, char end);

int lstrncmp(char *s1, const char *s2, size_t n);

int strcopy(char *dest, const char *src, char end);

char *strncopy(char *dest, const char *src, size_t n);

int count_bits(int n);

void error(char *msg, int len);

char *getenv(const char *env);

int microsleep(uint32_t usec);
