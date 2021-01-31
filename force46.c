#define _GNU_SOURCE

#include <dlfcn.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#if !defined(AF_INET_AVOID) || !defined(AF_INET_WANT)
#error "Both AF_INET_AVOID and AF_INET_WANT must be defined!"
#error "-DAF_INET_AVOID=AF_INET -DAF_INET_WANT=AF_INET6"
#endif

int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints, struct addrinfo **res) {
  // Enforce a AF_INET_WANT lookup.
  struct addrinfo *hints_copy = hints;
  hints_copy->ai_family = AF_INET_WANT;

  // The assignment of _getaddrinfo may look nuts, but satisfies the C
  // standard. The more straight forward way of `_getaddrinfo = dlsym(RTLD_NEXT,
  // "getaddrinfo");` would assign a void pointer to a function pointer, which
  // is forbidden.

  int (*_getaddrinfo)(const char *, const char *, const struct addrinfo *,
                      struct addrinfo **);
  *(void **)(&_getaddrinfo) = dlsym(RTLD_NEXT, "getaddrinfo");
  return _getaddrinfo(node, service, hints_copy, res);
}

int socket(int domain, int type, int protocol) {
  // To prevent other sockets from failing, only overwrite the IP to be avoided.
  // Otherwise, e.g., AF_UNIX might become AF_INET6.
  domain = (domain == AF_INET_AVOID) ? AF_INET_WANT : domain;

  int (*_socket)(int, int, int);
  *(void **)(&_socket) = dlsym(RTLD_NEXT, "socket");
  return _socket(domain, type, protocol);
}
