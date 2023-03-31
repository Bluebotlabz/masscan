#include "proto-minecraft.h"
#include "proto-banner1.h"
#include "proto-interactive.h"
#include "unusedparm.h"
#include "string_s.h"
#include "masscan-app.h"
#include "util-malloc.h"
#include "util-bool.h"
#include "proto-tcp.h"
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************************
 ***************************************************************************/
static const char minecraft_hello[] = {
    0x15,                                                                   // length
    0x00,                                                                   // packet ID (handshake)
    0xff, 0xff, 0xff, 0xff, 0x0f,                                           // protocol version number (-1 for ping)
    0x0b, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d, // hostname (we just use 'example.com')
    0xdd, 0x36,                                                             // port (stick with 25565)
    0x01,                                                                   // next state (1 for querying server status)
    0x01,                                                                   // length
    00                                                                      // packet ID (request status)
};

/*****************************************************************************
 * Initialize some stuff that's part of the Minecraft state-machine-parser.
 *****************************************************************************/
static void *
minecraft_init(struct Banner1 *b)
{
    //unsigned i;

    /*
     * These match Minecraft Header-Field: names
     */
    /*
 b->minecraft_fields = smack_create("minecraft", SMACK_CASE_INSENSITIVE);
 for (i=0; minecraft_fields[i].pattern; i++)
     smack_add_pattern(
                       b->minecraft_fields,
                       minecraft_fields[i].pattern,
                       minecraft_fields[i].pattern_length,
                       minecraft_fields[i].id,
                       minecraft_fields[i].is_anchored);
 smack_compile(b->minecraft_fields);*/

    /*
     * These match HTML <tag names
     */
    /*
 b->html_fields = smack_create("html", SMACK_CASE_INSENSITIVE);
 for (i=0; html_fields[i].pattern; i++)
     smack_add_pattern(
                       b->html_fields,
                       html_fields[i].pattern,
                       html_fields[i].pattern_length,
                       html_fields[i].id,
                       html_fields[i].is_anchored);
 smack_compile(b->html_fields);*/

    UNUSEDPARM(b);
    
    banner_minecraft.hello = MALLOC(banner_minecraft.hello_length);
    memcpy((char *)banner_minecraft.hello, minecraft_hello, banner_minecraft.hello_length);

    return 0;
}

/***************************************************************************
 * BIZARRE CODE ALERT!
 *
 * This uses a "byte-by-byte state-machine" to parse the response Minecraft
 * header. This is standard practice for high-performance network
 * devices, but is probably unfamiliar to the average network engineer.
 *
 * The way this works is that each byte of input causes a transition to
 * the next state. That means we can parse the response from a server
 * without having to buffer packets. The server can send the response
 * one byte at a time (one packet for each byte) or in one entire packet.
 * Either way, we don't. We don't need to buffer the entire response
 * header waiting for the final packet to arrive, but handle each packet
 * individually.
 *
 * This is especially useful with our custom TCP stack, which simply
 * rejects out-of-order packets.
 ***************************************************************************/
static void
minecraft_parse(const struct Banner1 *banner1,
                void *banner1_private,
                struct ProtocolState *pstate,
                const unsigned char *px, size_t length,
                struct BannerOutput *banout,
                struct InteractiveData *more)
{
    unsigned state = pstate->state;
    unsigned i;

    UNUSEDPARM(banner1_private);
    UNUSEDPARM(banner1);
    UNUSEDPARM(more);

    for (i = 0; i < length; i++) {
        banout_append_char(banout, PROTO_MINECRAFT, px[i]);
    }

    state++;
    tcp_close(more);

    pstate->state = state;
}

static int minecraft_selftest(void)
{
    return 0;
}

/***************************************************************************
 ***************************************************************************/
struct ProtocolParserStream banner_minecraft = {
    "minecraft",
    25565,
    minecraft_hello,
    sizeof(minecraft_hello),
    0,
    minecraft_selftest,
    minecraft_init,
    minecraft_parse,
};
