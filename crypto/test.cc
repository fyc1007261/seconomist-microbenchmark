#include <sodium.h>
#include "timestamp.h"
#include <iostream>

void test_hash(size_t size, int times, double &hash_time)
{
    uint64_t total_hash = 0;
    for (int i = 0; i < times; ++i)
    {
        unsigned char hash[crypto_generichash_BYTES];
        unsigned char message[size] = {0};
        randombytes_buf(message, size);

        Timestamp t_start;
        crypto_generichash(hash, sizeof hash,
                           message, size,
                           NULL, 0);
        Timestamp t_end;
        total_hash += t_end - t_start;
        // std::cout << t_end - t_start << std::endl;
    }
    hash_time = total_hash / times;
}

void test_sign_verify(size_t size, int times, double &sign_time, double &verify_time)
{

    uint64_t total_sign = 0, total_verify = 0;

    for (int i = 0; i < times; ++i)
    {
        unsigned char message[size] = {0};
        randombytes_buf(message, size);

        unsigned char pk[crypto_sign_PUBLICKEYBYTES];
        unsigned char sk[crypto_sign_SECRETKEYBYTES];
        crypto_sign_keypair(pk, sk);

        unsigned char signed_message[crypto_sign_BYTES + size];
        unsigned long long signed_message_len;
        Timestamp t_start;
        crypto_sign(signed_message, &signed_message_len,
                    message, size, sk);
        Timestamp t_end;
        total_sign += t_end - t_start;

        unsigned char unsigned_message[size];
        unsigned long long unsigned_message_len;

        t_start.renew();
        if (crypto_sign_open(unsigned_message, &unsigned_message_len,
                             signed_message, signed_message_len, pk) != 0)
        {
            std::cerr << "Verification fails" << std::endl;
            exit(1);
        }
        t_end.renew();
        total_verify += t_end - t_start;
    }
    sign_time = total_sign / times;
    verify_time = total_verify / times;
}

int main()
{
    // int size[] = {32, 64, 128, 256, 512, 1024, 2048, 4096};
    int size[] = {128, 256, 512, 1024, 2048, 4096, 8192};
    for (int i = 0; i < sizeof(size) / sizeof(int); ++i)
    {
        double hash_time, sign_time, verify_time;
        test_hash(size[i], 1000, hash_time);
        test_sign_verify(size[i], 1000, sign_time, verify_time);
        std::cout << size[i] << "," << hash_time << "," << sign_time << "," << verify_time << std::endl;
    }
    return 0;
}