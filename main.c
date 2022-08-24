/*
    Title       : To implement stegonography. 
    Author      : Prajwal S
    Date        : 12/08/2020
    Description : Input  : Source bmp file, secret file, destination file[optional]. 
                  Output : Based on user choice, encode or decode the secret file in or from bmp file.
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
#include "common.h"
#include "encode.c"
#include "decode.c"

int main(int argc, char *argv[])
{
    //Declaring the local variables
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    //Validation for not passing the option
    if (argc > 1)
    {
	//Encoding :Calling function to check operation type
	if (check_operation_type(argv) == e_encode)
	{
	    printf("INFO : Encoding is choosen.\n");

	    //Validating the given arguments
	    if (read_and_validate_encode_args(argv, &encInfo) == e_success)
	    {
		printf("INFO : Validation is success\n");
		printf("INFO : Entering do encoding function.\n");

		//Calling the encoding function
		if(do_encoding(&encInfo) == e_success)
		{
		    printf("INFO : Encoding is done Successfully.\n");
		}
		else
		{
		    printf("ERROR: Encode Failure.\n");
		    return -1;
		}
	    }
	    else
	    {
		printf("ERROR : Please provide input as below:\n./lsb_steg -e File_name.bmp secret.txt <output.bmp>\n");
		return -1;
	    }
	}

	//Decoding :Calling function to check operation type
	else if (check_operation_type(argv) == e_decode)
	{
	    printf("INFO : Decoding is chosen\n");

	    //Validating the passed arguments
	    if (read_and_validate_decode_args(argv, &decInfo) == e_success)
	    {
		printf("INFO : Validation successful.\n");
		printf("INFO : Decoding procedure started.\n");

		//Calling decoding function
		if(do_decoding(&decInfo) == e_success)
		{
		    printf("INFO : Decoding is done Successfully.\n");
		}
		else
		{
		    printf("ERROR: Decode Failure.\n");
		    return -1;
		}
	    }
	    else
	    {
		printf("ERROR : Please prvide input as below:\n./lsb_steg -d File_name.bmp <output.txt>\n");
		return -1;
	    }
	}
	else
	{
	    printf("Unsupported: Please provide input as below.\n./lsb_steg -e File_name.bmp secret.txt <output.bmp>\n./lsb_steg -d File_name.bmp <decode.txt>\n");
	    return -1;
	}
    }
    else
    {
	printf("ERROR : Please provide operation type:\nEncoding :./lsb_steg -e File_name.bmp secret.txt <output.bmp>\nDecoding :./lsb_steg -d File_name.bmp <decode.txt>\n");
	return -1;
    }
    return 0;
}
