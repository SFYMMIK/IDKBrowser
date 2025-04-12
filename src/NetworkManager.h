#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

void initialize_network_manager();
char* fetch_page(const char* url);
void cleanup_network_manager();

#ifdef __cplusplus
}
#endif

#endif // NETWORKMANAGER_H
