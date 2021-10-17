
#ifndef http_h_sentinel
#define http_h_sentinel

  void init_net(void);

/* fetch a resource from host/path on defined port using http and return a pointer to the allocated chunk of memory
 * Note: do not forget to free the memory afterwards! */
  long http_get(char *host, long port, char *path, unsigned char **resptr);

#endif
