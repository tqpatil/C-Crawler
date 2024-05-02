#ifndef URL_H
#define URL_H
#include <stdbool.h>
char * normalizeURL(const char *base, const char *url);

/**
 * Returns true if the webpage URL in the second argument is internal to the website from the first
 * URL. Both URLs must be provided in normalized form.
 */
bool isInternalURL(const char *base, const char *to_validate);

#endif
