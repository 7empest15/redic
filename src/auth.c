#include <openssl/sha.h>
#include <string.h>

#include "auth.h"

int check_password(const char *input, const char *stored_hash)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input, strlen(input), hash);
    return memcmp(hash, stored_hash, SHA256_DIGEST_LENGTH) == 0;
}