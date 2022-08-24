#include <stdio.h>
#include "types.h"
#include <string.h>
#include "common.h"
#include "decode.h"

//Function to read and validate decode arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //Checking whether source file passed or not
    if (argv[2] != NULL)
    {
	//Checking whether source file has .bmp extension or not
	if (strstr(argv[2], ".bmp"))
	{
	    decInfo->src_image_fname = argv[2];
	}
	else
	{
	    printf("ERROR : Entered file is not having .bmp exentension.\n");
	    return e_failure;
	}
    }
    else
    {
	printf("ERROR :Source file is not passed.\n");
	return e_failure;
    }

    //For secret file
    //If secret file name is given
    if (argv[3] != NULL)
    {
	char * ptr;
	//Fetching the extension
	ptr = strstr(argv[3], ".");

	//if extension exist
	if (ptr != NULL)
	{
	    int size;
	    size = strlen(ptr);
	    //Validation if extension size more than MAX_FILE_SUFFIX 4
	    if (size > MAX_FILE_SUFFIX)
	    {
		printf("ERROR : Please enter file with maximum file suffix 4.\n");
		return e_failure;
	    }
	    else
	    {
		//Storing data
		strcpy(decInfo->extn_secret_file, ptr);
		decInfo->secret_fname = argv[3];
		decInfo->secret_extn_flag = 1;
		return e_success;
	    }
	}
	//If extension does not exist
	else
	{
	    //If extension does not exist storing only name in temp array
	    char * str = argv[3];
	    int len = strlen(str), i;
	    for (i = 0; i < len; i++)
	    {
		decInfo->temp_secret_fname[i] = *str++;
	    }
	    decInfo->secret_extn_flag = 0;
	    return e_success;
	}
    }
    //If secret file name not given
    else
    {
	char * str = "decoded";
	int len = strlen(str), i;
	for (i = 0; i < len; i++)
	{
	    decInfo->temp_secret_fname[i] = *str++;
	}
	decInfo->secret_extn_flag = 0;
	return e_success;
    }
}

// Function to perform the decoding 
Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO : Opening required files.\n");

    //Calling function to open source file
    if (open_file(decInfo) == e_success)
    {
	printf("INFO : Decoding magic string signature.\n");
	fseek(decInfo->fptr_src_image, 54L, SEEK_SET);

	//Calling function to decode and check magic string
	if (decode_magic_string(MAGIC_STRING, decInfo) == e_success)
	{
	    printf("INFO : Decoding magic string is successful.\n");
	    printf("INFO : Decoding output file extesion size.\n");

	    //Calling function to decode extension size
	    if (decode_extn_size(decInfo) == e_success)
	    {
		printf("INFO : Output file extension size decoded.\n");
		printf("INFO : Decoding output file extension.\n");

		//Calling function to decode secret file extension
		if (decode_secret_file_extn(decInfo) == e_success)
		{
		    printf("INFO : Output file extension decoded.\n");
		    printf("INFO : Opening secret file.\n");

		    //Calling function to open secret file
		    if (open_secret_file(decInfo) == e_success)
		    {
			printf("INFO : Decoding secret file size.\n");

			//Calling function to decode secret file size
			if(decode_secret_file_size(decInfo) == e_success)
			{
			    printf("INFO : Secret file size decoded.\n");
			    printf("INFO : Decoding the secret file data.\n");

			    //Calling function to decode secret file data
			    if (decode_secret_file_data(decInfo) == e_success)
			    {
				printf("INFO : Secret file data decoded.\n");
				return e_success;
			    }
			    else
			    {
				printf("ERROR : Decoding secret file data failed\n");
				return e_failure;
			    }
			}
			else
			{
			    printf("ERROR : Decoding secret file size failed.\n");
			    return e_failure;
			}
		    }	
		    else
		    {
			printf("ERROR : Opening secret file failed.\n");
			return e_failure;
		    }
		}
		else
		{
		    printf("ERROR : Decoding secret file extension failed.\n");
		    return e_failure;
		}
	    }
	    else
	    {
		printf("ERROR : Decoding secret file extension size failed.\n");
		return e_failure;
	    }
	}
	else
	{
	    printf("ERROR : Magic string decoding failure.\n");
	    return e_failure;
	}
    }
    else
    {
	printf("ERROR : Source file opening failure.\n");
	return e_failure;
    }
}


//Function to open bmp file
Status open_file(DecodeInfo *decInfo)
{
    // Opening Source Image file
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");
    
    // Do Error handling
    if (decInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_image_fname);

    	return e_failure;
    }
    printf("INFO : Opened %s.\n", decInfo->src_image_fname);
    return e_success;
}

//Function to open secret file
Status open_secret_file(DecodeInfo *decInfo)
{
    //Opening secret file
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    
    // Do Error handling
    if (decInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);

    	return e_failure;
    }
    printf("INFO : Opened %s.\n", decInfo->secret_fname);
    return e_success;
}



// Function to decode and check magic string
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    if (e_success == decode_data_from_image(magic_string, strlen(magic_string), decInfo->fptr_src_image))
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

//Function to decode and each character of magic string
Status decode_data_from_image(const char *data, int size, FILE *fptr_src_image)
{
    int i;
    char ch;
    DecodeInfo *decodeInfo;
    //Loop to check if encoded magic string is same
    for (i = 0; i < size; i++ )
    {
	fread(decodeInfo->image_data, sizeof(char), 8, fptr_src_image);
	ch = decode_char(decodeInfo->image_data);

	//Checking each decoded character with magic string
	if (ch != data[i])
	{
	    return e_failure;
	}	   
    }
    return e_success;
}

// Function to decode a char by decoding 8 bits from 8 bytes
char decode_char(char *image_buffer)
{
    int i;
    uint mask , ch = 0x00;
    //Loop to fetch 1 character from lsb of 8 characters
    for (i = 0; i < 8; i++)
    {
	ch <<= 1;
	mask = image_buffer[i] & 0x01;
	ch = mask | ch;
    }
    return (char)ch;
}

//Function to decode extension size
Status decode_extn_size(DecodeInfo *decInfo)
{
    char arr[32];
    //Reading and decoding the size
    fread(arr, sizeof(char), 32, decInfo->fptr_src_image);
    decInfo->extn_size = decode_size_from_lsb(arr);
    return e_success;	    
}

//Function to decode size by decoding 32 bytes
int decode_size_from_lsb(char *buffer)
{
    uint i, num = 0;
    //Loop to decode int from 32 lsb of characters
    for (i = 0; i < 32; i++)
    {
	num <<= 1;
	buffer[i] = buffer[i] & 0x01;
	num = num | buffer[i];
    }
    return num;
}


// Function to decode secret file extension
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    int i;
    char arr[4];
    
    //Condition if extension is not given by user
    if (decInfo->secret_extn_flag == 0)
    {
	for (i = 0; i < decInfo->extn_size; i++ )
	{
	    fread(decInfo->image_data, sizeof(char), 8, decInfo->fptr_src_image);
	    arr[i] = decode_char(decInfo->image_data);
	}
	decInfo->secret_fname = strcat(decInfo->temp_secret_fname, arr);
	return e_success;
    }
    //Conditioon to check if extension given by user is same as encoded
    else if (decInfo->secret_extn_flag == 1)
    {
	for (i = 0; i < decInfo->extn_size; i++ )
	{
   	    fread(decInfo->image_data, sizeof(char), 8, decInfo->fptr_src_image);
   	    arr[i] = decode_char(decInfo->image_data);
	}
   	if ( strcmp(arr,decInfo->extn_secret_file) == 0)
	{
	    return e_success;
	}
	else
	{
	    return e_failure;
	}
    }
}

// Function to decode secret file size
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char arr[32];
    //Reading 32 character and fetching size
    fread(arr, sizeof(char), 32, decInfo->fptr_src_image);
    decInfo->size_secret_file = decode_size_from_lsb(arr);
    return e_success;
}

// Function to decode secret file data and close files
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    int size = decInfo->size_secret_file, i;

    //Loop to read each character and write that in secret file
    for (i = 0; i < size; i++)
    {
	fread(decInfo->image_data, sizeof(char), 8, decInfo->fptr_src_image);
	ch = decode_char(decInfo->image_data);
	fwrite(&ch, sizeof(char), 1, decInfo->fptr_secret);
    }

    //After completeing closing the files
    fclose(decInfo->fptr_secret);
    fclose(decInfo->fptr_src_image);
    return e_success;
}

