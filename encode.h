#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" 
#include "common.h"

//Stucture declaration to store information of source, stego and secret files
typedef struct _EncodeInfo
{
    //Source Image information
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    // Secret File Information
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

    //Stego Image Information
    char *stego_image_fname;
    FILE *fptr_stego_image;

} EncodeInfo;


// Encoding function prototypes
// Function to check operation type 
OperationType check_operation_type(char *argv[]);

//Read and validate Encode args from argv 
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

// Perform the encoding 
Status do_encoding(EncodeInfo *encInfo);

// Get File pointers for i/p and o/p files 
Status open_files(EncodeInfo *encInfo);

// check capacity 
Status check_capacity(EncodeInfo *encInfo);

// Get image size 
uint get_image_size_for_bmp(FILE *fptr_image);

// Get file size 
uint get_file_size(FILE *fptr);

// Copy bmp image header 
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

// Store Magic String 
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

// Encode secret file extenstion 
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

// Encode secret file size 
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

// Encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo);

// Encode function, which does the real encoding 
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

// Encode a byte into LSB of image data array 
Status encode_byte_to_lsb(char data, char *image_buffer);

// Copy remaining image bytes from src to stego image after encoding 
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

// Function to encode size
Status encode_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image);

// Function to encode size to lsb
Status encode_size_to_lsb(int size, char *buffer);

#endif