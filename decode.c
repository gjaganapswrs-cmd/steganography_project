#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strstr(argv[2],".bmp")!=NULL)
    {
        printf(".bmp file is present\n");
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf(".bmp file is not present\n");
        return e_failure;
    }

    if(argv[3] != NULL)
    {
        decInfo->output_fname = argv[3];
        printf("Output file name is provided: %s\n", decInfo->output_fname);
    }
    else
    {
        decInfo->output_fname = "decoded.txt";
        printf("Default output file name: decoded.txt\n");
    }
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if(open_decode_files(decInfo) == e_success)
    {
        printf("All decode files opened successfully\n");
    }
    else
    {
        printf("Error in opening files\n");
        return e_failure;
    }

    if(skip_bmp_header(decInfo->fptr_stego_image) == e_success)
    {
        printf("Skipped BMP header bytes successfully\n");
    }
    else
    {
        printf("Failed to skip BMP header\n");
        return e_failure;
    }

    char magic_string[20];
    printf("Enter the magic string used during encoding: ");
    scanf("%19s", magic_string);

    if (decode_magic_string(magic_string, decInfo) == e_success)
    {
       printf("Magic string decoded successfully\n");
    }
    else
    {
       printf("Magic string decoding failed\n");
       return e_failure;
    }


    if(decode_secret_file_extn_size(decInfo) == e_success)
    {
        printf("Secret file extension size decoded successfully\n");
    }
    else
    {
        printf("Secret file extension size decoding failed\n");
        return e_failure;
    }

    if(decode_secret_file_extn(decInfo) == e_success)
    {
        printf("Secret file extension decoded successfully\n");
    }
    else
    {
        printf("Secret file extension decoding failed\n");
        return e_failure;
    }

    if(decode_secret_file_size(decInfo) == e_success)
    {
        printf("Secret file size decoded successfully\n");
    }
    else
    {
        printf("Secret file size decoding failed\n");
        return e_failure;
    }

    if(decode_secret_file_data(decInfo) == e_success)
    {
        printf("Secret file data decoded successfully\n");
    }
    else
    {
        printf("Secret file data decoding failed\n");
        return e_failure;
    }

    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    printf("Decoding completed successfully! Output written to %s\n", decInfo->output_fname);

    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if(decInfo->fptr_stego_image == NULL)
    {
        printf("Error: Stego image file not found\n");
        return e_failure;
    }

    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    if(decInfo->fptr_output == NULL)
    {
        printf("Error: Unable to create output file\n");
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }

    return e_success;
}

Status skip_bmp_header(FILE *fptr_stego_image)
{
    // BMP header size is always 54 bytes
    fseek(fptr_stego_image, 54, SEEK_SET);
    return e_success;
}


Status decode_byte_from_lsb(char *data, unsigned char *image_buffer)
{
    unsigned char ch = 0;
    for(int i = 0; i < 8; i++)
    {
        ch = ch | ((image_buffer[i] & 1) << i);
    }
    *data = ch;
    return e_success;
}

Status decode_size_from_lsb(long int *size, unsigned char *image_buffer)
{
    *size = 0;
    for(int i = 0; i < 32; i++)
    {
        *size = *size | ((image_buffer[i] & 1) << i);
    }
    return e_success;
}

/*Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    unsigned char image_buffer[8];
    char buffer[strlen(magic_string) + 1];

    for(int i = 0; i < strlen(magic_string); i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&buffer[i], image_buffer);
    }

    buffer[strlen(magic_string)] = '\0';

    if(strcmp(buffer, magic_string) == 0)
        return e_success;
    else
        return e_failure;
}*/

#include "common.h"

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    unsigned char image_buffer[8];
    char buffer[50];
    int len = strlen(magic_string);

    for(int i = 0; i < len; i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&buffer[i], image_buffer);
    }

    buffer[len] = '\0';

    unsigned char next_buffer[8];
    char next_char;
    fread(next_buffer, 1, 8, decInfo->fptr_stego_image);
    decode_byte_from_lsb(&next_char, next_buffer);

    if((strcmp(buffer, magic_string) == 0) && (next_char == '\0'))
    {
        printf("Magic string fully matched\n");
        return e_success;
    }
    else
    {
        printf("Magic string is not matched\n");
        return e_failure;
    }
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    unsigned char image_buffer[32];
    fread(image_buffer, 1, 32, decInfo->fptr_stego_image);
    decode_size_from_lsb(&decInfo->extn_size, image_buffer);
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    unsigned char image_buffer[8];
    for(int i = 0; i < decInfo->extn_size; i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&decInfo->extn_secret_file[i], image_buffer);
    }
    decInfo->extn_secret_file[decInfo->extn_size] = '\0';
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    unsigned char image_buffer[32];
    fread(image_buffer, 1, 32, decInfo->fptr_stego_image);
    decode_size_from_lsb(&decInfo->size_secret_file, image_buffer);
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    unsigned char image_buffer[8];
    char ch;
    for(long int i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&ch, image_buffer);
        fputc(ch, decInfo->fptr_output);
    }
    return e_success;
}