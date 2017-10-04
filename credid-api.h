#pragma once

#define MAXDATASIZE 4096

typedef struct credid_api_log_s {
  char  *query;
  int   success;
} credid_api_log_t ;

typedef struct credid_api_logs_link_s {
  credid_api_log_t  *line;
  struct credid_api_logs_link_s* next;
} credid_api_logs_link_t;

typedef struct {
  int   socket;
  char  *last_command_result;
  int   logs_enabled;
  credid_api_logs_link_t *logs;
  credid_api_logs_link_t *logs_end;
} credid_api_t;

/* Initialize the API */
credid_api_t *credid_api_init(char const *host, short unsigned int port);

/* Free the API */
int credid_api_free(credid_api_t *api);

/* Return 1 if the last query returned success, else 0 */
int credid_api_success(credid_api_t const *api);

/*
  Get the last result (data after "success" or "failure").
  This data is freed after each call to the API, you might need to copy it.
*/
char *credid_api_last_result(credid_api_t const *api);

/* Send AUTH ids */
int credid_api_auth(credid_api_t *api, char const *username, char const *password);

/* Request access to a resource */
int credid_api_user_has_access_to(credid_api_t *api, char const *perm, char const *res);

int credid_api_group_add(credid_api_t *api, char const *group, char const *perm, char const *resource);
int credid_api_group_remove(credid_api_t *api, char const *group, char const *resource);
int credid_api_group_list(credid_api_t *api);
int credid_api_group_list_perms(credid_api_t *api, char const *group);
int credid_api_group_get_perm(credid_api_t *api, char const *group, char const *resource);

int credid_api_user_list(credid_api_t *api);
int credid_api_user_add(credid_api_t *api, char const *username, char const *password);
int credid_api_user_remove(credid_api_t *api, char const *username);
int credid_api_user_add_group(credid_api_t *api, char const *username, char const *group);
int credid_api_user_remove_group(credid_api_t *api, char const *username, char const *group);
int credid_api_user_list_groups(credid_api_t *api, char const *username);
int credid_api_user_change_password(credid_api_t *api, char const *username, char const *newpassword);

/* Enable or disable the logging (1 or 0) */
int credid_api_setup_logs(credid_api_t *api, int enable);

/* Return the first log of the logs (and remove it from the logs). The returned pointer must be freed with free(). */
credid_api_log_t *credid_api_fetch_log(credid_api_t *api);

/* Remove and free the logs. Called by credid_api_free() */
int credid_api_free_logs(credid_api_t *api);
