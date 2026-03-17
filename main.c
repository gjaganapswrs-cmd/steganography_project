#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("Usage:\n");
        printf("  Encode: ./stego -e <input.bmp> <secret.txt> <output.bmp>\n");
        printf("  Decode: ./stego -d <stego.bmp> <outputfile>\n");
        return 1;
    }

    OperationType op_type = check_operation_type(argv);

    if(op_type == e_encode)
    {
        EncodeInfo encInfo;

        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("Arguments validated successfully (Encode).\n");

            if (do_encoding(&encInfo) == e_success)
                printf("Encoding completed successfully!\n");
            else
                printf("Encoding failed.\n");
        }
        else
        {
            printf("Encode argument validation failed.\n");
        }
    }

    else if(op_type == e_decode)
    {
        DecodeInfo decInfo;

        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("Arguments validated successfully (Decode).\n");

            if (do_decoding(&decInfo) == e_success)
                printf("Decoding completed successfully!\n");
            else
                printf("Decoding failed.\n");
        }
        else
        {
            printf("Decode argument validation failed.\n");
        }
    }

    else
    {
        printf("Unsupported operation. Use -e or -d\n");
    }

    return 0;
}
