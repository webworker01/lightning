#ifndef LIGHTNING_DAEMON_SPHINX_H
#define LIGHTNING_DAEMON_SPHINX_H

#include "config.h"
#include "bitcoin/privkey.h"
#include "bitcoin/pubkey.h"

#include <ccan/short_types/short_types.h>
#include <ccan/tal/tal.h>
#include <secp256k1.h>
#include <sodium/randombytes.h>

#define SECURITY_PARAMETER 20
#define NUM_MAX_HOPS 20
#define HOP_PAYLOAD_SIZE 20
#define TOTAL_HOP_PAYLOAD_SIZE (NUM_MAX_HOPS * HOP_PAYLOAD_SIZE)
#define MESSAGE_SIZE 0
#define ROUTING_INFO_SIZE (2 * NUM_MAX_HOPS * SECURITY_PARAMETER)
#define TOTAL_PACKET_SIZE (1 + 33 + SECURITY_PARAMETER + ROUTING_INFO_SIZE + \
			   TOTAL_HOP_PAYLOAD_SIZE + MESSAGE_SIZE)

struct onionpacket {
	/* Cleartext information */
	u8 version;
	u8 nexthop[20];
	u8 mac[20];
	secp256k1_pubkey ephemeralkey;

	/* Encrypted information */
	u8 routinginfo[ROUTING_INFO_SIZE];
	u8 hoppayloads[TOTAL_HOP_PAYLOAD_SIZE];
	u8 payload[MESSAGE_SIZE];
};

enum route_next_case {
	ONION_END = 0,
	ONION_FORWARD = 1,
};

struct hoppayload {
	u8 realm;
	u64 amount;
	u8 remainder[11];
};

struct route_step {
	enum route_next_case nextcase;
	struct onionpacket *next;
	u8 *payload;
	struct hoppayload *hoppayload;
};

/**
 * create_onionpacket - Create a new onionpacket that can be routed
 * over a path of intermediate nodes.
 *
 * @ctx: tal context to allocate from
 * @secpctx: the secp256k1_context for EC operations
 * @path: public keys of nodes along the path.
 * @hoppayloads: payloads destined for individual hosts (limited to
 *    HOP_PAYLOAD_SIZE bytes)
 * @num_hops: path length in nodes
 * @sessionkey: 20 byte random session key to derive secrets from
 * @message: end-to-end payload destined for the final recipient
 * @messagelen: length of @message
 */
struct onionpacket *create_onionpacket(
	const tal_t * ctx,
	secp256k1_context * secpctx,
	struct pubkey path[],
	struct hoppayload hoppayloads[],
	const u8 * sessionkey,
	const u8 * message,
	const size_t messagelen
	);

/**
 * process_onionpacket - process an incoming packet by stripping one
 * onion layer and return the packet for the next hop.
 *
 * @ctx: tal context to allocate from
 * @secpctx: the secp256k1_context for EC operations
 * @packet: incoming packet being processed
 * @hop_privkey: the processing node's private key to decrypt the packet
 * @hoppayload: the per-hop payload destined for the processing node.
 */
struct route_step *process_onionpacket(
	const tal_t * ctx,
	secp256k1_context * secpctx,
	const struct onionpacket *packet,
	struct privkey *hop_privkey
	);

/**
 * serialize_onionpacket - Serialize an onionpacket to a buffer.
 *
 * @ctx: tal context to allocate from
 * @secpctx: the secp256k1_context for EC operations
 * @packet: the packet to serialize
 */
u8 *serialize_onionpacket(
	const tal_t *ctx,
	const secp256k1_context *secpctx,
	const struct onionpacket *packet);

/**
 * parese_onionpacket - Parse an onionpacket from a buffer.
 *
 * @ctx: tal context to allocate from
 * @secpctx: the secp256k1_context for EC operations
 * @src: buffer to read the packet from
 * @srclen: length of the @src
 */
struct onionpacket *parse_onionpacket(
	const tal_t *ctx,
	const secp256k1_context *secpctx,
	const void *src,
	const size_t srclen
	);

void pubkey_hash160(
	const secp256k1_context *secpctx,
	u8 *dst,
	const struct pubkey *pubkey);

#endif /* LIGHTNING_DAEMON_SPHINX_H */
