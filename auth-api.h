#pragma once

#define MAXDATASIZE 4096

typedef struct {
  int socket;
  char *last_command_result;
} auth_api_t;

/* Initialize the API */
auth_api_t *auth_api_init(char const *host, short unsigned int port);

/* Free the API */
int auth_api_free(auth_api_t *api);


/* Return 1 if the last query returned success, else 0 */
int auth_api_success(auth_api_t const *api);

/*
  Get the last result (data after "success" or "failure").
  This data is freed after each call to the API, you might need to copy it.
*/
char *auth_api_last_result(auth_api_t const *api);

/* Send AUTH ids */
int auth_api_auth(auth_api_t *api, char const *username, char const *password);

/* Request access to a resource */
int auth_api_user_has_access_to(auth_api_t *api, char const *perm, char const *res);

int auth_api_group_add(auth_api_t *api, char const *group, char const *perm, char const *resource);
int auth_api_group_remove(auth_api_t *api, char const *group, char const *resource);
int auth_api_group_list(auth_api_t *api);
int auth_api_group_list_perms(auth_api_t *api, char const *group);
int auth_api_group_get_perm(auth_api_t *api, char const *group, char const *resource);

int auth_api_user_list(auth_api_t *api);
int auth_api_user_add(auth_api_t *api, char const *username, char const *password);
int auth_api_user_remove(auth_api_t *api, char const *username);
int auth_api_user_add_group(auth_api_t *api, char const *username, char const *group);
int auth_api_user_remove_group(auth_api_t *api, char const *username, char const *group);
int auth_api_user_list_groups(auth_api_t *api, char const *username);
int auth_api_user_change_password(auth_api_t *api, char const *username, char const *newpassword);
