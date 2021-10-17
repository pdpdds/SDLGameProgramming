/* This file is part of the Simple Sokoban project
 * Copyright (C) Mateusz Viste 2014
 */

#if defined(_WIN32) || defined(WIN32)
#include <winsock2.h>  /* gethostbyname() on nonstandard, exotic platforms */
#else
#include <netdb.h>     /* gethostbyname() on posix */
#endif

#include <errno.h>
 //#include <unistd.h> /* NULL */
#include <string.h> /* memcpy() */
#include <stdlib.h> /* realloc(), malloc() */
#include <stdio.h>  /* sprintf() */

void init_net(void) {
#if defined(_WIN32) || defined(WIN32)
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

/* open socket to remote host/port and return its socket descriptor */
static int makeSocket(char* host, long portnum) {
	int sock;
	struct sockaddr_in sa; /* Socket address */
	struct hostent* hp;    /* Host entity */

	hp = gethostbyname(host);
	if (hp == NULL) return(-1);

	/* Copy host address from hostent to (server) socket address */
	memcpy((char*)&sa.sin_addr, (char*)hp->h_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;  /* Set service sin_family to PF_INET */
	sa.sin_port = htons(portnum);       /* Put portnum into sockaddr */

	/* open socket */
	sock = socket(hp->h_addrtype, SOCK_STREAM, 0);
	if (sock == -1) return(-1);

	/* connect to remote host */
	if (connect(sock, (struct sockaddr*)&sa, sizeof(sa)) == -1) return(-1);

	return(sock);
}

static long readline(int sock, char* buf, long maxlen) {
	long res, bufpos = 0;
	char bytebuff;
	for (;;) {
		res = recv(sock, &bytebuff, 1, 0);
		if (res < 1) break;
		if (bytebuff == '\r') continue;
		if (bytebuff == '\n') break;
		buf[bufpos++] = bytebuff;
		if (bufpos >= maxlen - 1) break;
	}
	buf[bufpos] = 0;
	return(bufpos);
}

/* fetch a resource from host/path on defined port using http and return a pointer to the allocated chunk of memory
 * Note: do not forget to free the memory afterwards! */
long http_get(char* host, long port, char* path, unsigned char** resptr) {
#define BUFLEN 2048
	long resalloc = 1024;
	long reslen = 0;
	char linebuf[BUFLEN];
	long len;
	int sock;
	unsigned char* res;
	*resptr = NULL;
	res = NULL;
	sock = makeSocket(host, port);
	if (sock < 0) {
		printf("makeSocket() err: %s\n", strerror(errno));
		return(-1);
	}
	snprintf(linebuf, BUFLEN - 1, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, host);
	send(sock, linebuf, strlen(linebuf), 0);
	/* trash out all headers */
	for (;;) {
		len = readline(sock, linebuf, BUFLEN);
		if (len == 0) break;
	}
	/* preallocate initial buffer */
	res = malloc(resalloc);
	if (res == NULL) {
		close(sock);
		return(-1);
	}
	/* fetch data */
	for (;;) {
		len = recv(sock, linebuf, BUFLEN, 0);
		if (len == 0) break;
		if (len < 0) {
			printf("Err: %s\n", strerror(errno));
			free(res);
			close(sock);
			return(-1);
		}
		if (reslen + len + 1 > resalloc) {
			resalloc *= 2;
			res = realloc(res, resalloc);
			if (res == NULL) {
				close(sock);
				return(-1);
			}
		}
		memcpy(&(res[reslen]), linebuf, len);
		reslen += len;
	}
	res[reslen] = 0; /* terminate data with a NULL, just in case (I don't know what the caller will want to do with the data..) */
	*resptr = res;
	return(reslen);
}
