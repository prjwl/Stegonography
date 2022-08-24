#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

//Function to check type of operation
OperationType check_operation_type(char *argv[])
{
    //Comparing argument with strings
    if (strcmp(argv[1], "-e") == 0)
    {
	return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
	return e_decode;
    }
    else
    {
	return e_unsupported;
    }
}

//Function to validate arguments
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //To check source image file
    if (argv[2] != NULL)
    {
	if (strstr(argv[2], ".bmp"))
	{
	    encInfo->src_image_fname = argv[2];
	}
	else
	{
	    printf("ERROR : Entered source file is not having .bmp extension.\n");
	    return e_failure;
	}
    }
    else
    {
	printf("ERROR : Source file is not passed.\n");
	return e_failure;
    }
    
    //To check secret file
    if (argv[3] != NULL)
    {
	int size;
	char * ptr;
	//Fetching the extension
	ptr = strstr(argv[3], ".");
	
	//If extension is present
	if (ptr != NULL)
	{
	    size = strlen(ptr);

	    //Condition to limit extension of secret file size to 4
	    if (size > MAX_FILE_SUFFIX)
	    {
		printf("ERROR : Entered secret file extesion size is more than 4.\n");
		return e_failure;
	    }
	    else
	    {
		strcpy(encInfo->extn_secret_file, ptr); 
		encInfo->secret_fname = argv[3];
	    }
	}
	else
	{
	    printf("ERROR : Secret file is not having a extension.\n");
	    return e_failure;
	}	    
    }
    else
    {
	printf("ERROR : Secret file is not passed.\n");
	return e_failure;
    }

    //To check output file
    if (argv[4] != NULL)
    {
	if (strstr(argv[4], ".bmp"))
	{
	    encInfo->stego_image_fname = argv[4];
	    return e_success;
	}
	else
	{
	    printf("ERROR : Output file passed is not having .bmp extension.\n");
	    return e_failure;
	}
    }
    else
    {
	encInfo->stego_image_fname = "stego.bmp";
	printf("INFO : Output file not mentioned. Creating stego.bmp as default.\n");
	return e_success;
    }
}


//Perform the encoding 
Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO : Opening required files.\n");

    //Calling function to open files
    if (open_files(encInfo) == e_success)
    {
	printf("INFO : Encoding procedure started\n");
	printf("INFO : Checking capacity.\n");

	//Calling function to check capacity of source file
	if (check_capacity(encInfo) == e_success)
	{
	    printf("INFO : check capacity success.\n");
	    printf("INFO : Copying BMP header.\n");

	    //Calling function to copy first 54bytes
	    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	    {
		printf("INFO : Copy BMP header success\n");
		printf("INFO : Encoding Magic string.\n");

		//Calling function to encode magic string
		if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
		{
		    printf("INFO : Magic string encoded Successfully.\n");
		    printf("INFO : Encoding secret file extension size.\n");

		    //Calling function to encode secret file extension size.
		    if (encode_size(strlen(encInfo->extn_secret_file),encInfo->fptr_src_image, encInfo->fptr_stego_image)  == e_success)
		    {
			printf("INFO : Secret file extension size encoded Successfully.\n");
			printf("INFO : Encoding secret file extension\n");

			//Calling function to encode secret file extension
			if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
			{
			    printf("INFO : Secret file extension encoded successfully.\n");
			    printf("INFO : Encoding secret file size.\n");

			    //Calling function to encode secret file size.
			    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
			    {
				printf("INFO : Secret file size encoded successfully.\n");
				printf("INFO : Encoding secret file data.\n");

				//Calling function to encode secret file data
				if (encode_secret_file_data(encInfo) == e_success)
				{
				    printf("INFO : Secret file data encoded successfully.\n");
				    printf("INFO : Copying remaining data.\n");

				    //Calling function to encode remaining data
				    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
				    {
					printf("INFO : Remainig data copied successfully.\n");
					return e_success;
				    }
				    else
				    {
					printf("ERROR : Remaing data copying failed.\n");
					return e_failure;
				    }
				}
				else
				{
				    printf("ERROR : Secret data encoding failed.\n");
				    return e_failure;
				}
			    }
			    else
			    {
				printf("ERROR : Secret file size copying failed.\n");
				return e_failure;
			    }
			}
			else
			{
			    printf("ERROR : Secret file extension encoding failed.\n");
			    return e_failure;
			}
		    }
		    else
		    {
			printf("ERROR : Secret file extension size encoding failed.\n");
			return e_failure;
		    }
		}
		else
		{
		    printf("ERROR : Magic String encoding failed.\n");
		    return e_failure;
		}
	    }
	    else
	    {
		printf("ERROR : Copying header failed.\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("ERROR : Capacity of source bmp file is not enough.\n");
	    return e_failure;
	}
    }
    else
    {
	printf("ERROR : Opening files failed.\n");
	return e_failure;
    }
    
    return e_success;
}


// Functio to open the files.
Status open_files(EncodeInfo *encInfo)
{
    // Opening source image
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    printf("INFO : Opened %s.\n", encInfo->src_image_fname);

    // Opening secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    printf("INFO : Opened %s.\n", encInfo->secret_fname);

    // Opening destination image.
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    printf("INFO : Opened %s.\n", encInfo->stego_image_fname);

    // No failure return e_success
    return e_success;
}



// Function to check capacity 
Status check_capacity(EncodeInfo *encInfo)
{
    //Getting size of source file
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    //Getting size of secret file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    //Getting extension size
    long size_of_extn = strlen(encInfo->extn_secret_file);

    //Checking the capacity
    if (encInfo->image_capacity > 54 + 16 + 32 + (size_of_extn * 8) + 32 + (8 * encInfo->size_secret_file))
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

//Function to get image size
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);

    // Return image capacity
    return width * height * 3;
}


// Function to get file size 
uint get_file_size(FILE *fptr)
{
    uint size;
    //Taking pointer to end of file
    fseek(fptr, 0L, SEEK_END);
    size = ftell(fptr);
    rewind(fptr);
    return size;
}

// Function to copy bmp image header 
Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_dest_image)
{
    char ch;
    int i;
    
    //Bring the file pointers to initial point
    rewind(fptr_src_image);
    rewind(fptr_dest_image);

    //Loop to copy 54 bytes
    for (i = 0; i < 54; i++)
    {
	fread(&ch , sizeof(char), sizeof(char) , fptr_src_image); 
	fwrite(&ch , sizeof(char), sizeof(char) , fptr_dest_image);
    }
    return e_success;
}

// Function to encode magic string
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    if (e_success == encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image))
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

// Function to encode data to image
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    //Local  variable and pointer declaration
    int i;
    EncodeInfo *encodeInfo;

    //Loop to encode each byte to image
    for (i = 0; i < size; i++ )
    {
	fread(encodeInfo->image_data, sizeof(char), 8, fptr_src_image);
	encode_byte_to_lsb(data[i], encodeInfo->image_data);
	fwrite(encodeInfo->image_data, sizeof(char), 8, fptr_stego_image); 
    }
    return e_success;
}

// Function to encode 8 bits of data to 8 bytes
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    uint i;
    //Encoding 1byte to lsb of 8 bytes
    for (i = 0; i < 8; i++)
    {
	uint mask = 0x80;
	image_buffer[i] = image_buffer[i] & 0xFE;
	mask = mask & data;
	mask = mask >> 7;
	image_buffer[i] = image_buffer[i] | mask;
	data <<= 1;
    }
    return e_success;
}

// Encoding  extention size and file size 
Status encode_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[32];
    //Reading and encoding int to lsb of 32 bytes
    fread(arr, sizeof(char), 32, fptr_src_image);
    encode_size_to_lsb(size, arr);
    fwrite(arr, sizeof(char), 32, fptr_stego_image);
    return e_success;	
}

// Encoding size to lsb 
Status encode_size_to_lsb(int size, char *buffer)
{
    uint i;
    //Encoding int to lsb of 32 bytes
    for (i = 0; i < 32; i++)
    {
	uint mask = 0x80000000;
	buffer[i] = buffer[i] & 0xFE;
	mask = mask & size;
	mask = mask >> 31;
	buffer[i] = buffer[i] | mask;
	size <<= 1;
    }
    return e_success;
}

// Encoding secret file extenstion 
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    if (e_success == encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image))
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

// Encoding secret file size 
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char arr[32];
    //Reading and encoding secret file size
    fread(arr, sizeof(char), 32, encInfo->fptr_src_image);
    encode_size_to_lsb((int)file_size, arr);
    fwrite(arr, sizeof(char), 32, encInfo->fptr_stego_image);
    return e_success;
}

// Encoding secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    int size = encInfo->size_secret_file, i;
    for (i = 0; i < size; i++) 
    {
	fread(encInfo->image_data, sizeof(char), 8, encInfo->fptr_src_image);
	fread(&ch, 1, sizeof(char), encInfo->fptr_secret);
	encode_byte_to_lsb(ch, encInfo->image_data);
	fwrite(encInfo->image_data, sizeof(char), 8, encInfo->fptr_stego_image);
    }
    //Closing secret file.
    fclose(encInfo->fptr_secret);
    return e_success;
}	
	
// Function to copy remaining image bytes from src to stego image after encoding 
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    //Loop to encode remaing data
    while (fread(&ch , sizeof(char), sizeof(char) , fptr_src) > 0)
    {
	fwrite(&ch , sizeof(char), sizeof(char) , fptr_dest);
    }

    //Closing the files
    fclose(fptr_src);
    fclose(fptr_dest);
    return e_success;
}	

