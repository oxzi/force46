#define _GNU_SOURCE

#include <dlfcn.h>
#include <errno.h>
#include <netdb.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>

#if !defined(AF_INET_AVOID) || !defined(AF_INET_WANT)
#error "Both AF_INET_AVOID and AF_INET_WANT must be defined!"
#error "-DAF_INET_AVOID=AF_INET -DAF_INET_WANT=AF_INET6"
#endif

// Allow getaddrinfo calls to the undesired IP version. However, this might
// result in leaks for concurrent sockets.
bool getaddrinfo_active = false;

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

  getaddrinfo_active = true;
  const int ret = _getaddrinfo(node, service, hints_copy, res);
  getaddrinfo_active = false;

  return ret;
}

int socket(int domain, int type, int protocol) {
  // To prevent other sockets from failing, only drop the IP version to be
  // avoided. Otherwise, e.g., AF_UNIX might fail. Also check if getaddrinfo is
  // currently active.
  if (!getaddrinfo_active && domain == AF_INET_AVOID) {
    return EINVAL;
  }

  int (*_socket)(int, int, int);
  *(void **)(&_socket) = dlsym(RTLD_NEXT, "socket");
  return _socket(domain, type, protocol);
}
