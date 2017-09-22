#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "auth-api.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

#define auth_api_send(format, ...)                                      \
  ({                                                                    \
    int err = 0;                                                        \
    char cmd[MAXDATASIZE] = {0};                                        \
    snprintf(cmd, 256, format "\n" , ##__VA_ARGS__);                    \
    send(api->socket, cmd, strlen(cmd), 0);                             \
    int numbytes;                                                       \
    if (api->last_command_result != NULL)                               \
      free(api->last_command_result);                                   \
    api->last_command_result = malloc(MAXDATASIZE);                     \
    if (api->last_command_result != NULL) {                             \
      memset(api->last_command_result, 0, MAXDATASIZE);                 \
      if ((numbytes = recv(api->socket, api->last_command_result, MAXDATASIZE-1, 0)) == -1) { \
        err = 1;                                                        \
      }                                                                 \
    }                                                                   \
    else {                                                              \
      err = 2;                                                          \
    }                                                                   \
    err;                                                                \
  })

char *auth_api_last_result(auth_api_t const *api) {
  return api->last_command_result + 8;
}

int auth_api_success(auth_api_t const *api) {
  return strncmp(api->last_command_result, "success", 7) == 0;
}

int auth_api_auth(auth_api_t *api, char const *username, char const *password) {
  return auth_api_send("AUTH : %s %s", username, password);
}

int auth_api_user_has_access_to(auth_api_t *api, char const *perm, char const *res) {
  return auth_api_send("USER HAS ACCESS TO : \\a %s %s", perm, res);
}

int auth_api_group_add(auth_api_t *api, char const *group, char const *perm, char const *resource) {
  return auth_api_send("GROUP ADD : %s %s %s", group, perm, resource);
}

int auth_api_group_remove(auth_api_t *api, char const *group) {
  return auth_api_send("GROUP REMOVE : %s", group);
}

int auth_api_group_list(auth_api_t *api) {
  return auth_api_send("GROUP LIST");
}

int auth_api_group_list_perms(auth_api_t *api, char const *group) {
  return auth_api_send("GROUP LIST PERMS : %s", group);
}

int auth_api_group_get_perm(auth_api_t *api, char const *group, char const *resource) {
  return auth_api_send("GROUP GET PERM : %S %s", group, resource);
}

int auth_api_user_list(auth_api_t *api) {
  return auth_api_send("USER LIST");
}

int auth_api_user_add(auth_api_t *api, char const *username, char const *password) {
  return auth_api_send("USER ADD : %s %s", username, password);
}

int auth_api_user_remove(auth_api_t *api, char const *username) {
  return auth_api_send("USER REMOVE : %s", username);
}

int auth_api_user_add_group(auth_api_t *api, char const *username, char const *group) {
  return auth_api_send("USER ADD GROUP : %s %s", username, group);
}

int auth_api_user_remove_group(auth_api_t *api, char const *username, char const *group) {
  return auth_api_send("USER REMOVE GROUP : %s %s", username, group);
}

int auth_api_user_list_group(auth_api_t *api, char const *username) {
  return auth_api_send("USER LIST GROUP : %s", username);
}

int auth_api_user_change_password(auth_api_t *api, char const *username, char const *newpassword) {
  return auth_api_send("USER CHANGE PASSWORD : %s %s", username, newpassword);
}

auth_api_t *auth_api_init(char const *host, short unsigned int port) {
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  char port_string[6] = {0};
  snprintf(port_string, 5, "%i", port);
  printf("Connect to: %s:%s\n", host, port_string);
  if ((rv = getaddrinfo(host, port_string, &hints, &servinfo)) != 0) {
    perror("getaddrinfo");
    return NULL;
  }

  // loop through all the results and connect to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("connect");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return NULL;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
  printf("client: connecting to %s\n", s);

  freeaddrinfo(servinfo); // all done with this structure

  auth_api_t *api = malloc(sizeof(auth_api_t));
  if (api == NULL){
    close(sockfd);
    return NULL;
  }
  api->socket = sockfd;
  api->last_command_result = NULL;
  return api;
}

int auth_api_free(auth_api_t *api) {
  close(api->socket);
  if (api->last_command_result != NULL)
    free(api->last_command_result);
  free(api);
  return 0;
}
