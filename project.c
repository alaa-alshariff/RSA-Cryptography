#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// // Function to check if a number is prime
int isPrime(int number)
{
    if (number <= 1)
    {
        return 0;
    }
    for (int i = 2; i * i <= number; i++)
    {
        if (number % i == 0)
        {
            return 0;
        }
    }
    return 1;
}

// // Function to find the greatest common divisor (GCD)
int gcd(int x, int y)
{
    while (y != 0)
    {
        int temp = y;
        y = x % y;
        x = temp;
    }
    return x;
}

// // Function to generate a random prime number of 16 bits
int generate_prime()
{
    int number;
    do
    {
        number = (rand() % (1 << 16)) + (1 << (16 - 1));
    } while (!isPrime(number));
    return number;
}

// Function to convert a message to an array of text chunks
void TextToChunks(const char *message, char ***chunks, int *numChunks)
{
    int chunkSize = 3;
    int messageLength = strlen(message);
    *numChunks = (messageLength + chunkSize - 1) / chunkSize;

    // Allocate memory for the chunks array
    // *chunks = (char **)malloc(*numChunks * sizeof(char *));
    *chunks = (char **)calloc(*numChunks, sizeof(char *));
    if (*chunks == NULL)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < *numChunks; i++)
    {
        (*chunks)[i] = (char *)malloc(chunkSize + 1);
        if ((*chunks)[i] == NULL)
        {
            fprintf(stderr, "Memory allocation failed.\n");
            exit(1);
        }
    }

    for (int i = 0; i < *numChunks; i++)
    {
        strncpy((*chunks)[i], message + i * chunkSize, chunkSize);
        (*chunks)[i][chunkSize] = '\0'; // Null-terminate the chunk
    }
}

// Function to convert an array of text chunks back to a message
char *ChunksToText(char **chunks, int numChunks)
{
    int chunkSize = 3;
    int messageLength = numChunks * chunkSize;
    char *message = (char *)malloc(messageLength + 1);

    if (message == NULL)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < numChunks; i++)
    {
        strcat(message, chunks[i]);
    }

    return message;
}

// Function to convert a chunk into an integer number
unsigned long long chunkToInteger(const char *chunk)
{
    unsigned long long integerChunk = 0;
    for (int i = 0; i < 3; i++)
    {
        integerChunk = (integerChunk << 8) | (chunk[i] & 0xFF);
    }
    return integerChunk;
}

// Function to convert an integer into a chunk of characters
void integerToChunk(unsigned long long integer, char *chunk, int chunkSize)
{
    for (int i = chunkSize - 1; i >= 0; i--)
    {
        chunk[i] = (char)(integer & 0xFF); // Extract the lowest 8 bits
        integer >>= 8;                     // Shift right by 8 bits to process the next 8 bits
    }
    chunk[chunkSize] = '\0'; // Null-terminate the chunk
}

// Function to perform modular exponentiation (a^b % mod)
unsigned long long modularExponentiation(unsigned long long base, unsigned long long exponent, unsigned long long modulus)
{
    if (modulus == 1)
    {
        return 0; // If modulus is 1, the result is always 0
    }

    unsigned long long result = 1;
    base = base % modulus; // Ensure base is within the modulus

    while (exponent > 0)
    {
        if (exponent % 2 == 1)
        {
            result = (result * base) % modulus;
        }
        base = (base * base) % modulus;
        exponent = exponent / 2;
    }

    return result;
}
// Function to perform RSA encryption
unsigned long long rsaEncrypt(unsigned long long plaintext, unsigned long long e, unsigned long long N)
{
    return modularExponentiation(plaintext, e, N);
}

// Function to convert an integer into a chunk of characters with a specific size
void integerToChunkWithSize(unsigned long long integer, char *chunk, int chunkSize)
{
    for (int i = chunkSize - 1; i >= 0; i--)
    {
        chunk[i] = (char)(integer & 0xFF); // Extract the lowest 8 bits
        integer >>= 8;                     // Shift right by 8 bits to process the next 8 bits
    }
}

// Function to perform RSA decryption
unsigned long long rsaDecrypt(unsigned long long ciphertext, unsigned long long d, unsigned long long N)
{
    if (N == 1)
    {
        return 0; // If modulus is 1, the result is always 0
    }

    unsigned long long result = 1;
    ciphertext = ciphertext % N; // Ensure ciphertext is within the modulus

    while (d > 0)
    {
        if (d % 2 == 1)
        {
            result = (result * ciphertext) % N;
        }
        ciphertext = (ciphertext * ciphertext) % N;
        d = d / 2;
    }

    return result;
}

// Function to sign a message
void signMessage(const char *message, unsigned long long d, unsigned long long N, unsigned long long *signature)
{
    int numChunks;
    char **chunks = NULL;

    TextToChunks(message, &chunks, &numChunks);

    // Calculate the signature for each chunk
    for (int i = 0; i < numChunks; i++)
    {
        unsigned long long integerMessage = chunkToInteger(chunks[i]);
        signature[i] = rsaDecrypt(integerMessage, d, N);
    }

    // Free allocated memory for chunks
    for (int i = 0; i < numChunks; i++)
    {
        free(chunks[i]);
    }
    free(chunks);
}

// Function to verify a message signature
int verifySignature(const char *message, const unsigned long long *signature, unsigned long long e, unsigned long long N)
{
    int numChunks;
    char **chunks = NULL;

    TextToChunks(message, &chunks, &numChunks);

    // Verify the signature for each chunk
    for (int i = 0; i < numChunks; i++)
    {
        unsigned long long integerMessage = chunkToInteger(chunks[i]);
        unsigned long long encryptedMessage = rsaEncrypt(signature[i], e, N);

        if (integerMessage != encryptedMessage)
        {
            // Signature verification failed
            return 0;
        }
    }

    // Free allocated memory for chunks
    for (int i = 0; i < numChunks; i++)
    {
        free(chunks[i]);
    }
    free(chunks);

    // Signature verification succeeded
    return 1;
}

int main()
{
    const char *message = "project done";
    unsigned long long p = 64717;
    unsigned long long q = 40699;
    unsigned long long my_N = 2633917183;
    unsigned long long phi_N = 2633811768;
    unsigned long long my_e = 2044897765;
    unsigned long long d = 1763740525;
    unsigned long long N = 2541476909;
    unsigned long long e = 1835403391;
    const char *name = "Alaa Alsharif";
    int numChunks;
    char **chunks = NULL;
    unsigned long long partner_cipher[] = {1949982988, 2606194791, 2472424713, 2556878229, 1846272149};
    int cipher_size = sizeof(partner_cipher) / sizeof(partner_cipher[0]);
    printf(" # IDs \n MY_ID = 40279078 \n PARTNER_ID = 40233193");
    printf("\n\n\n");

    printf(" # my data \n");
    printf(" p = %lld\n q = %lld\n N = %lld\n phi_N = %lld\n e = %lld\n d = %lld\n ", p, q, my_N, phi_N, my_e, d);
    printf("\n\n\n");
    printf(" # my partner data \n");
    printf(" PARTNER_N  = %lld\n PARTNER_e  = %lld\n ", N, e);
    printf("\n\n\n");

    printf("# encryption \n");
    printf("MY_MESSAGE = %s\n", message);

    printf("MY_MESSAGE_chunks = ");
    TextToChunks(message, &chunks, &numChunks);
    for (int i = 0; i < numChunks; i++)
    {
        printf("%s, ", chunks[i]);
    }
    printf("\n");
    unsigned long long *integers = (unsigned long long *)malloc(numChunks * sizeof(unsigned long long));
    unsigned long long *encryptedIntegers = (unsigned long long *)malloc(numChunks * sizeof(unsigned long long));
    for (int i = 0; i < numChunks; i++)
    {
        integers[i] = chunkToInteger(chunks[i]);
    }
    printf("MY_CIPHERTEXT = ");
    for (int i = 0; i < numChunks; i++)
    {
        encryptedIntegers[i] = rsaEncrypt(integers[i], e, N);
        printf("%lld, ", encryptedIntegers[i]);
    }
    printf("\n\n\n");
    printf("# decryption \n");

    printf("PARTNER_CIPHERTEXT = [1949982988, 2606194791, 2472424713, 2556878229, 1846272149]\n");

    unsigned long long *decryptedIntegers = (unsigned long long *)malloc(numChunks * sizeof(unsigned long long));
    for (int i = 0; i < cipher_size; i++)
    {
        decryptedIntegers[i] = rsaDecrypt(partner_cipher[i], d, my_N);
    }

    printf("PARTNER_MESSAGE_chunks_AFTER_DECRYPT = ");
    char **decryptedChunks = (char **)malloc(cipher_size * sizeof(char *));

    for (int i = 0; i < cipher_size; i++)
    {
        int chunkSize = 0;
        unsigned long long temp = decryptedIntegers[i];
        while (temp > 0)
        {
            chunkSize++;
            temp >>= 8;
        }

        decryptedChunks[i] = (char *)malloc(chunkSize + 1);
        integerToChunkWithSize(decryptedIntegers[i], decryptedChunks[i], chunkSize);
        decryptedChunks[i][chunkSize] = '\0';
        printf("%s, ", decryptedChunks[i]);
    }
    printf("\n");
    char *decryptedMessage = ChunksToText(decryptedChunks, cipher_size);
    printf("PARTNER_MESSAGE_AFTER_DECRYPT =  %s\n", decryptedMessage);
    printf("\n\n\n");

    // Printing signature chunks
    int sgnChunks;
    char **singnaturechunks = NULL;

    TextToChunks(name, &singnaturechunks, &sgnChunks);

    printf("MY_MESSAGE_TO_BE_SIGNED = %s\n", name);

    printf("MY_MESSAGE_TO_BE_SIGNED_chunks =");
    for (int i = 0; i < sgnChunks; i++)
    {
        printf("%s, ", singnaturechunks[i]);
    }
    printf("\n");

    unsigned long long signature[sgnChunks];
    signMessage(name, d, my_N, signature);
    printf("MY_SIGNATURE = ");
    for (int i = 0; i < sgnChunks; i++)
    {
        printf("%lld, ", signature[i]);
    }
    printf("\n\n\n");

    printf("# verfiy the signature \n");
    printf("PARTNER_SIGNED_MESSAGE = Deepti Maruvada \n");
    printf("PARTNER_SIGNATURE = [2025269189, 1969802717, 2376636831, 1400189108, 1551438492] \n");
    printf("IS_VALID_SIGNATURE = ");




const char *partner_sign_msg = "Deepti Maruvada";
unsigned long long partner_signature[]= {2025269189, 1969802717, 2376636831, 1400189108, 1551438492};


    int isSignatureValid = verifySignature(partner_sign_msg, partner_signature, e, N);
    if (isSignatureValid)
    {
        printf("True.\n");
    }
    else
    {
        printf("False.\n");
    }

    // Free allocated memory for chunks, integers, retransformedChunks, encryptedIntegers, and reconstructedMessage
    for (int i = 0; i < numChunks; i++)
    {
        free(chunks[i]);
    }
    free(chunks);

    free(integers);
    free(encryptedIntegers);

    fflush(stdout);
    return 0;
}
