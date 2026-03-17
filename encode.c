#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"
/* Name :jagan 
   Date :10-11-2025
   Project name :Stegnography
   */
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
OperationType check_operation_type(char *argv[])
{
	//comparing -e is present in cml
	if(strcmp("-e", argv[1]) == 0)
	{
		//if -e return e_encode
		return e_encode;
	}
	//comparing -d is present in cml
	else if(strcmp("-d",argv[1]) == 0)
        { 
		//if -d return e_decode
	        return e_decode;

	}	
	else
	{
	    //if it other than -e and -d return e_unsupported	
	    return e_unsupported;
	}

}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	//check the src file is .bmp or not 
	 printf("Checking arguments is started\n");
	if ( strstr(argv[2],".bmp") != NULL)
	{
		//storing source file name in src_image_fname
		encInfo -> src_image_fname = argv[2];
		//checking the secret file is .txt , .sh or .c
		if(strstr(argv[3],".txt") != NULL ||strstr(argv[3],".sh") != NULL || strstr(argv[3],".c") != NULL)
		{
			//Storing secret file name to secret_fname
			encInfo -> secret_fname = argv[3];
			encInfo -> extn_secret_file[MAX_FILE_SUFFIX];
			char *extn = strstr(encInfo -> secret_fname, ".txt");
			//storing secret file extn in extn_secret_file
                 	strcpy(encInfo -> extn_secret_file, extn);
                        //checking output file is given or not
			if (argv[4] != NULL)
			{
				//checking if the .bmp extn is there in outputfile
				if (strstr(argv[4],".bmp") != NULL)
				{
					//storing output file name to stego_image_fname
					encInfo -> stego_image_fname = argv[4];
					return e_success;
				}
				else
				{
					printf("Error :Provide Output File with .bmp extension.\n");
					printf("Difficulty in checking file extenstions\n");
					return e_failure;
				}		
			}
			else
			{
				//if user is not given the output file give difault name as stego.bmp and return e_success
				encInfo -> stego_image_fname = "stego.bmp";
				return e_success;
			}
			
		}
		else
		{
			printf("Error :: Dificulty in checking secret message file extenstion in .txt|.c|.sh\n");
			printf("Dificulty in checking file extensions\n");
			e_failure;
		}
		
	}
	else
	{
		printf("Error :: Dificulty in checking source file extension in .bmp\n");
		printf("Dificuly in checking file extensions\n");
		return e_failure;
	}
	

}
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
     fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
  //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
   // printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
uint get_file_size(FILE *fptr_secret)
{
	//seek to the last byte
	fseek(fptr_secret, 0, SEEK_END);
	//returing position 
	return ftell(fptr_secret);
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	
	char buffer[54];
	//making file pointer point to first byte
	rewind (fptr_src_image);
        //reading 54 bytes from src file
	fread(buffer, 54, 1, fptr_src_image);
	//storing 54 bytes to the outputfile
	fwrite(buffer, 54, 1, fptr_dest_image);
	printf("Header is copying\n");
	return e_success;
	

}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	//calling the data to image function
	encode_data_to_image(magic_string, strlen(magic_string), encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
	return e_success;

}
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char buffer[8];
	for (int i = 0; i < size; i++)
	{
		//reading 8 bytes from src image
		fread(buffer, 8, 1, fptr_src_image);
		//call encode_byte_to_lsb function
		encode_byte_to_lsb(data[i], buffer);
                //storing 8 bytes to output image after encoding 
		fwrite(buffer, 8, 1, fptr_stego_image);
	}
	return e_success;
	
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	int i, get;
	for ( i = 7; i >= 0; i--)
	{
		get = ((data & (1 << i)) >> i);//get bit form msb
		image_buffer[7 - i] = image_buffer[7 - i] & (~1);//clear the bit in lsb
		image_buffer[7 - i] = image_buffer[7 - i] | get;//set bit on lsb

	}
	return e_success;

}
Status encode_secret_file_extn_size(long extn_size, EncodeInfo *encInfo)
{
	
	char buffer[32];
        //reading 32 bytes from src image
	fread(buffer, 32, 1, encInfo -> fptr_src_image);
	//function call
	//printf("%ld\n",extn_size);
	encode_int_to_lsb(extn_size, buffer);
	//storing 32 bytes to output image after encoding
	fwrite(buffer, 32, 1, encInfo -> fptr_stego_image);
	return e_success;
}
Status encode_int_to_lsb(int data, char *buffer)
{
	int i, get;
	for (i = 31; i >= 0; i--)
	{
	        get = ((data & (1 << i))  >> i);//get bit form msb
		buffer[31 - i] = buffer[31 - i] & (~1);//clear the bit in lsb
	        buffer[31 - i] = buffer[31 - i] | get;//set bit on lsb
	}
	return e_success;
}
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
	//calling data to image function
	encode_data_to_image(file_extn, strlen(file_extn), encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
	return e_success;
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	char buffer[32];
	//reading 32 bytes from src image
	fread(buffer, 32, 1, encInfo -> fptr_src_image);
	//function call
	encode_int_to_lsb(file_size, buffer);
	//storing 32 bytes to output image after encoding
	fwrite(buffer, 32, 1, encInfo -> fptr_stego_image);
        return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	rewind(encInfo -> fptr_secret);
	char secret_data[encInfo -> size_secret_file];
	//reading size bytes of secret file
	fread(secret_data, encInfo -> size_secret_file, 1, encInfo -> fptr_secret);
        //Function call data to image
	encode_data_to_image(secret_data, encInfo -> size_secret_file, encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
	//stroing size bytes of secret file
	fwrite(secret_data, encInfo -> size_secret_file, 1, encInfo -> fptr_secret);
	return e_success;

}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	char buffer;
	//Copying reaming data to output file upto end of file
	//return e_success
	while (fread(&buffer, 1, 1,  fptr_src) != 0)
	{
		fwrite(&buffer, 1, 1, fptr_dest);
	}
	return  e_success;
}
/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    //opening Src Image file  in read mode
    encInfo -> fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo -> fptr_src_image == NULL)
    {
	//perror will give which type error autiometically
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> src_image_fname);
        //if it equal to null return e_failure
    	return e_failure;
    }

    //opening Secret file in read mode
    encInfo -> fptr_secret = fopen(encInfo -> secret_fname, "r");
    // Do Error handling
    if (encInfo -> fptr_secret == NULL)
    {
	//perror will give which type error autiometically    
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> secret_fname);
        //if it equal to null return e_failure
    	return e_failure;
    }

    // opeing Stego Image file in write mode
    encInfo -> fptr_stego_image = fopen(encInfo -> stego_image_fname, "wb");
    //  Do Error handling  
    if (encInfo -> fptr_stego_image == NULL)
    {
	//perror will give which type error autiometically    
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> stego_image_fname);
        //if it equal to null return e_failure
    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
	//printf("image capacity = %u\n",encInfo -> image_capacity);

	encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
	//printf("Size = %lu\n",encInfo ->size_secret_file);
	printf("Checking capacity to encode message\n");
	int size = 54 + ((2 + 4 + 4 + 4 + encInfo -> size_secret_file)* 8);
	//printf("total size = %d\n", size);
        //checking size of secret file is less than image capacity 
	//if it true return e_success or false return e_failure
	if (encInfo -> image_capacity > size )
	{
		printf("Capacity of source file is more than the secret file.\n");
		return e_success;
	}
	else
	{
		printf("Capacity of source file is more than the secret file.\n");
		return e_failure;
	}

}
Status do_encoding(EncodeInfo *encInfo)
{
	printf("Open files proccess started\n");
	printf(" Encoding file ::%s\n",encInfo -> stego_image_fname);
	if(open_files(encInfo) == e_success)
	{
		printf("Files opend successfully\n");
		if (check_capacity(encInfo) == e_success)
		{
			if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
			{
				printf("Succesfully header is copied\n");
				printf("Encoding the magic string\n");
				if (encode_magic_string(MAGIC_STRING, encInfo) == e_success )
				{  
				      printf("Magic string Encoded Successfully\n");
	                              printf("Encoding the File Extension Size\n");
				      	if (encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo) == e_success)
					{
						 printf("File Extension Size Encoded Successfully\n");
						 printf("Encoding the File Extenstion\n");
						if (encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
						{
						           printf("File Extenstion Encoded SuccessFully(Ex::.txt\n");
							   printf("Encoding Secret File Size(data size)\n");
							if (encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
							{
								 printf("File Size is Encoded Successfully\n");
								 printf("Encoding the secret Data\n");
								if(encode_secret_file_data(encInfo) == e_success)
								{
									 printf("Secret Data Encoded Successfully\n");
								if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
								  {
									      printf("Encoded the Remaining Data\n");
									      return e_success;
								  }

								}
								else
								{
									printf("encoded unsuccesfully.\n");
									 return e_failure;
								}
								
							}
							else
							{
								printf("encoded unsuccesfully.\n");
								return e_failure;
							}

						}
						else
						{
							 printf("encoded unsuccesfully.\n");
							  return e_failure;
						}
						

					}
					else
					{
						 printf("encoded unsuccesfully.\n");
						 return e_failure;
					}
				

				}
				else
				{
					 printf("encoded unsuccesfully.\n");
					  return e_failure;
				}
				

			}
			else
			{
				 printf("encoded unsuccesfully.\n");
				 return e_failure;
			}
		}
		else
		{
			printf("encoded unsuccesfully.\n");
			 return e_failure;
		}
	}
	else
	{
		printf("encoded unsuccesfully.\n");
		 return e_failure;
	}
	return e_success;
}
