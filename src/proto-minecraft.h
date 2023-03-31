#ifndef PROTO_MINECRAFT_H
#define PROTO_MINECRAFT_H
#include "proto-banner1.h"
#include "util-bool.h"

extern struct ProtocolParserStream banner_minecraft;


/**
 * Called during configuration when processing a command-line option
 * like "--minecraft-field <name=value>" to add/change a field in the HTTP 
 * header.
 */
size_t
minecraft_change_field(unsigned char **inout_header, size_t header_length,
                    const char *field_name,
                    const unsigned char *field_value, size_t field_value_len,
                    int what);


/**
 * Called during configuration when processing a command-line option
 * like "--minecraft-url /foo.html". This replaces whatever the existing
 * URL is into the new one. 
 * @param item
 *      0=method, 1=url, 2=version
 * @return
 *   the new length of the header (expanded or shrunk)
 */
size_t
minecraft_change_requestline(unsigned char **inout_header, size_t header_length,
                    const void *url, size_t url_length, int item);

#endif

