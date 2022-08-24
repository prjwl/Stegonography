#ifndef DECODE_H
#define DECODE_H

#include "types.h" 
#include "common.h"

//Structure declaration
typedef struct _DecodeInfo
{
    //Source image information
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    //Secret file information
    char *secret_fname;
    char temp_secret_fname[100];
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;
    int extn_size;
    int secret_extn_flag;

} DecodeInfo;

//Function to read and validate decode arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

//Function for decoding
Status do_decoding(DecodeInfo *decInfo);

//Function for opening bmp the files
Status open_file(DecodeInfo *decInfo);

//Function for opening the secret file
Status open_secret_file(DecodeInfo *decInfo);

//Function for decoding the magic string
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

//Function for decoding data from image
Status decode_data_from_image(const char *data, int size, FILE *fptr_src_image);

//Function to decode a character
char decode_char(char *image_buffer);

//Function for decoding the extension size
Status decode_extn_size(DecodeInfo *decInfo);

//Function to decode size from decoding lsbs
int decode_size_from_lsb(char *buffer);

//Function to decode secret file extension
Status decode_secret_file_extn(DecodeInfo *decInfo);

//Function to decode secret file size
Status decode_secret_file_size(DecodeInfo *decInfo);

//Function to decode file data
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif
