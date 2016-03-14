//
//  PNG.c
//  PNG
//
//  Created by Michael Kwasnicki on 19.03.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "simplePNG.h"

#include <png.h>

#include <stdlib.h>


bool pngIsPNG(const uint8_t * const in_PNG_DATA) {
    const uint8_t magic[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    return memcmp(in_PNG_DATA, magic, 8) == 0;
}


#define PNG_ERROR (0)
#define PNG_OK (1)


/* example.c - an example of using libpng
 * Last changed in libpng 1.4.0 [January 3, 2010]
 * This file has been placed in the public domain by the authors.
 * Maintained 1998-2010 Glenn Randers-Pehrson
 * Maintained 1996, 1997 Andreas Dilger)
 * Written 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
 */

/* This is an example of how to use libpng to read and write PNG files.
 * The file libpng.txt is much more verbose then this.  If you have not
 * read it, do so first.  This was designed to be a starting point of an
 * implementation.  This is not officially part of libpng, is hereby placed
 * in the public domain, and therefore does not require a copyright notice.
 *
 * This file does not currently compile, because it is missing certain
 * parts, like allocating memory to hold an image.  You will have to
 * supply these parts to get it to compile.  For an example of a minimal
 * working PNG reader/writer, see pngtest.c, included in this distribution;
 * see also the programs in the contrib directory.
 */

/* The png_jmpbuf() macro, used in error handling, became available in
 * libpng version 1.0.6.  If you want to be able to run your code with older
 * versions of libpng, you must define the macro yourself (but only if it
 * is not already defined by libpng!).
 */

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

/* Check to see if a file is a PNG file using png_sig_cmp().  png_sig_cmp()
 * returns zero if the image is a PNG and nonzero if it isn't a PNG.
 *
 * The function check_if_png() shown here, but not used, returns nonzero (true)
 * if the file can be opened and is a PNG, 0 (false) otherwise.
 *
 * If this call is successful, and you are going to keep the file open,
 * you should call png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK); once
 * you have created the png_ptr, so that libpng knows your application
 * has read that many bytes from the start of the file.  Make sure you
 * don't call png_set_sig_bytes() with more than 8 bytes read or give it
 * an incorrect number of bytes read, or you will either have read too
 * many bytes (your fault), or you are telling libpng to read the wrong
 * number of magic bytes (also your fault).
 *
 * Many applications already read the first 2 or 4 bytes from the start
 * of the image to determine the file type, so it would be easiest just
 * to pass the bytes to png_sig_cmp() or even skip that if you know
 * you have a PNG file, and call png_set_sig_bytes().
 */
#define PNG_BYTES_TO_CHECK 4
int pngCheck(const char *in_FILE) {
    png_byte buf[PNG_BYTES_TO_CHECK];
    
    /* Open the prospective PNG file. */
    FILE *fp = fopen(in_FILE, "rb");
    
    if (!fp) {
        return 0;
    }
    
    /* Read in some of the signature bytes */
    if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK) {
        fclose(fp);
        return 0;
    }
    
    /* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
     Return nonzero (true) if they match */
    int isPNG = !png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK);
    fclose(fp);
    
    return isPNG;
}



struct mem_encode
{
    const char *buffer;
    size_t size;
};


static void user_read_fn(png_structp png_ptr, png_bytep out_data, png_size_t length) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    struct mem_encode *state = (struct mem_encode *)io_ptr;
    memcpy(out_data, state->buffer, length);
    state->buffer += length;
    state->size -= length;
}



bool pngDecode(uint8_t **out_image, uint32_t *out_width, uint32_t *out_height, uint32_t *out_channels, const uint8_t * const in_PNG_DATA, const size_t in_PNG_SIZE, const bool in_FLIP_Y) {       /* We need to open the file */
    struct mem_encode state = (struct mem_encode){
        .buffer = in_PNG_DATA,
        .size = in_PNG_SIZE
    };
    
    unsigned int sig_read = 0;
    png_uint_32 width, height;
    int channels;
    
    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also supply the
     * the compiler header file version, so that we know if the application
     * was compiled with a compatible version of the library.  REQUIRED
     */
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    
    if (!png_ptr) {
        return PNG_ERROR;
    }
    
    /* Allocate/initialize the memory for image information.  REQUIRED. */
    png_infop info_ptr = png_create_info_struct(png_ptr);
    
    if (info_ptr == NULL) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return PNG_ERROR;
    }
    
    /* Set error handling if you are using the setjmp/longjmp method (this is
     * the normal method of doing things with libpng).  REQUIRED unless you
     * set up your own error handlers in the png_create_read_struct() earlier.
     */
    
    if (setjmp(png_jmpbuf(png_ptr))) {
        /* Free all of the memory associated with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        /* If we get here, we had a problem reading the file */
        return PNG_ERROR;
    }
    
    /* One of the following I/O initialization methods is REQUIRED */
    /* PNG file I/O method 2 */
    /* If you are using replacement read functions, instead of calling
     * png_init_io() here you would call:
     */
    
    png_set_read_fn(png_ptr, &state, user_read_fn);
    /* where user_io_ptr is a structure you want available to the callbacks */
    
    
    /* If we have already read some of the signature */
    png_set_sig_bytes(png_ptr, sig_read);
    
    /*
     * If you have enough memory to read in the entire image at once,
     * and you need to specify only transforms that can be controlled
     * with one of the PNG_TRANSFORM_* bits (this presently excludes
     * dithering, filling, setting background, and doing gamma
     * adjustment), then you can read the entire image (including
     * pixels) into the info structure with this call:
     */
    
    int bit_depth;
    int color_type;
    
    png_read_info(png_ptr, info_ptr);
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    
    //	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    //
    //	int interlace_type;
    //	int compression_type;
    //	int filter_method;
    //	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, &compression_type, &filter_method);
    
    //printf("%i x %i @ %i, %i, %i, %i, %i\n", width, height, bit_depth, color_type, interlace_type, compression_type, filter_method);
    
    switch (color_type) {
        case PNG_COLOR_TYPE_GRAY:
            channels = 1;
            
            if (bit_depth < 8) {
                png_set_packing(png_ptr);
                png_set_expand_gray_1_2_4_to_8(png_ptr);
            }
            
            break;
            
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            channels = 2;
            break;
            
        case PNG_COLOR_TYPE_PALETTE:
            channels = 3;
            png_set_packing(png_ptr);
            png_set_palette_to_rgb(png_ptr);
            break;
            
        case PNG_COLOR_TYPE_RGB:
            channels = 3;
            break;
            
        case PNG_COLOR_TYPE_RGB_ALPHA:
            channels = 4;
            break;
            
        default:
            channels = 1;
            break;
    }
    
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
    }
    
    if (bit_depth == 16) {
        png_set_strip_16(png_ptr);
    }
    
    png_read_update_info(png_ptr, info_ptr);
    
    *out_image = calloc(width * height * channels, sizeof(png_byte));
    png_byte **png_rows = malloc(height * sizeof(png_byte *));
    unsigned int row;
    
    for (row = 0; row < height; row++) {
        if (in_FLIP_Y) {
            png_rows[height - 1 - row] = *out_image + (row * channels * width);
        } else {
            png_rows[row] = *out_image + (row * channels * width);
        }
    }
    
    png_read_image(png_ptr, png_rows);
    
    free(png_rows);
    
    *out_width = width;
    *out_height = height;
    *out_channels = channels;
    
    /* At this point you have read the entire image */
    
    /* Clean up after the read, and free any memory allocated - REQUIRED */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    
    /* That's it */
    return PNG_OK;
}



/* Read a PNG file.  You may want to return an error code if the read
 * fails (depending upon the failure).  There are two "prototypes" given
 * here - one where we are given the filename, and we need to open the
 * file, and the other where we are given an open file (possibly with
 * some or all of the magic bytes read - see comments above).
 */
/* prototype 1 */
bool pngRead(const char *in_FILE, const bool in_FLIP_Y, uint8_t **out_image, uint32_t *out_width, uint32_t *out_height, uint32_t *out_channels) {       /* We need to open the file */
    unsigned int sig_read = 0;
    png_uint_32 width, height;
    int channels;
    FILE *fp = fopen(in_FILE, "rb");
    
    if (!fp) {
        return PNG_ERROR;
    }
    
    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also supply the
     * the compiler header file version, so that we know if the application
     * was compiled with a compatible version of the library.  REQUIRED
     */
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    
    if (!png_ptr) {
        fclose(fp);
        return PNG_ERROR;
    }
    
    /* Allocate/initialize the memory for image information.  REQUIRED. */
    png_infop info_ptr = png_create_info_struct(png_ptr);
    
    if (info_ptr == NULL) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return PNG_ERROR;
    }
    
    /* Set error handling if you are using the setjmp/longjmp method (this is
     * the normal method of doing things with libpng).  REQUIRED unless you
     * set up your own error handlers in the png_create_read_struct() earlier.
     */
    
    if (setjmp(png_jmpbuf(png_ptr))) {
        /* Free all of the memory associated with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        /* If we get here, we had a problem reading the file */
        return PNG_ERROR;
    }
    
    /* One of the following I/O initialization methods is REQUIRED */
    /* PNG file I/O method 1 */
    /* Set up the input control if you are using standard C streams */
    png_init_io(png_ptr, fp);
    
    
    /* If we have already read some of the signature */
    png_set_sig_bytes(png_ptr, sig_read);
    
    /*
     * If you have enough memory to read in the entire image at once,
     * and you need to specify only transforms that can be controlled
     * with one of the PNG_TRANSFORM_* bits (this presently excludes
     * dithering, filling, setting background, and doing gamma
     * adjustment), then you can read the entire image (including
     * pixels) into the info structure with this call:
     */
    
    int bit_depth;
    int color_type;
    
    png_read_info(png_ptr, info_ptr);
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    
    //	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    //
    //	int interlace_type;
    //	int compression_type;
    //	int filter_method;
    //	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, &compression_type, &filter_method);
    
    //printf("%i x %i @ %i, %i, %i, %i, %i\n", width, height, bit_depth, color_type, interlace_type, compression_type, filter_method);
    
    switch (color_type) {
        case PNG_COLOR_TYPE_GRAY:
            channels = 1;
            
            if (bit_depth < 8) {
                png_set_packing(png_ptr);
                png_set_expand_gray_1_2_4_to_8(png_ptr);
            }
            
            break;
            
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            channels = 2;
            break;
            
        case PNG_COLOR_TYPE_PALETTE:
            channels = 3;
            png_set_packing(png_ptr);
            png_set_palette_to_rgb(png_ptr);
            break;
            
        case PNG_COLOR_TYPE_RGB:
            channels = 3;
            break;
            
        case PNG_COLOR_TYPE_RGB_ALPHA:
            channels = 4;
            break;
            
        default:
            channels = 1;
            break;
    }
    
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
    }
    
    if (bit_depth == 16) {
        png_set_strip_16(png_ptr);
    }
    
    png_read_update_info(png_ptr, info_ptr);
    
    *out_image = calloc(width * height * channels, sizeof(png_byte));
    png_byte **png_rows = malloc(height * sizeof(png_byte *));
    unsigned int row;
    
    for (row = 0; row < height; row++) {
        if (in_FLIP_Y) {
            png_rows[height - 1 - row] = *out_image + (row * channels * width);
        } else {
            png_rows[row] = *out_image + (row * channels * width);
        }
    }
    
    png_read_image(png_ptr, png_rows);
    
    free(png_rows);
    
    *out_width = width;
    *out_height = height;
    *out_channels = channels;
    
    /* At this point you have read the entire image */
    
    /* Clean up after the read, and free any memory allocated - REQUIRED */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    
    /* Close the file */
    fclose(fp);
    
    /* That's it */
    return PNG_OK;
}


/* Write a png file */
bool pngWrite(const char *in_FILE_NAME, uint8_t *in_IMAGE, const uint32_t in_WIDTH, const uint32_t in_HEIGHT, const uint32_t in_CHANNELS) {
    int color_type = 0;
    int bytes_per_pixel = in_CHANNELS * sizeof(png_byte);
    
    switch (in_CHANNELS) {
        case 2:
            color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
            break;
            
        case 3:
            color_type = PNG_COLOR_TYPE_RGB;
            break;
            
        case 4:
            color_type = PNG_COLOR_TYPE_RGB_ALPHA;
            break;
            
        case 1:
        default:
            color_type = PNG_COLOR_TYPE_GRAY;
            break;
    }
    
    /* Open the file */
    FILE *fp = fopen(in_FILE_NAME, "wb");
    
    if (fp == NULL) {
        return PNG_ERROR;
    }
    
    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also check that
     * the library version is compatible with the one used at compile time,
     * in case we are using dynamically linked libraries.  REQUIRED.
     */
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    
    if (png_ptr == NULL) {
        fclose(fp);
        return PNG_ERROR;
    }
    
    /* Allocate/initialize the image information data.  REQUIRED */
    png_infop info_ptr = png_create_info_struct(png_ptr);
    
    if (info_ptr == NULL) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, NULL);
        return PNG_ERROR;
    }
    
    /* Set error handling.  REQUIRED if you aren't supplying your own
     * error handling functions in the png_create_write_struct() call.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
        /* If we get here, we had a problem writing the file */
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return PNG_ERROR;
    }
    
    /* I/O initialization method 1 */
    /* Set up the output control if you are using standard C streams */
    png_init_io(png_ptr, fp);
    
    /* Set the image information here.  Width and height are up to 2^31,
     * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
     * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
     * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
     * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
     * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
     * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
     */
    png_set_IHDR(png_ptr, info_ptr, in_WIDTH, in_HEIGHT, 8, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    
    /* Write the file header information.  REQUIRED */
    png_write_info(png_ptr, info_ptr);
    
    /* The easiest way to write the image (you may have a different memory
     * layout, however, so choose what fits your needs best).  You need to
     * use the first method if you aren't handling interlacing yourself.
     */
    png_bytep *row_pointers = calloc(in_HEIGHT, sizeof(png_bytep));
    
    if (in_HEIGHT > PNG_UINT_32_MAX / sizeof(png_bytep)) {
        png_error(png_ptr, "Image is too tall to process in memory");
    }
    
    png_uint_32 k;
    
    for (k = 0; k < in_HEIGHT; k++) {
        row_pointers[k] = in_IMAGE + k * in_WIDTH * bytes_per_pixel;
    }
    
    /* Write out the entire image data in one call */
    png_write_image(png_ptr, row_pointers);
    
    /* It is REQUIRED to call this to finish writing the rest of the file */
    png_write_end(png_ptr, info_ptr);
    
    /* Whenever you use png_free() it is a good idea to set the pointer to
     * NULL in case your application inadvertently tries to png_free() it
     * again.  When png_free() sees a NULL it returns without action, thus
     * avoiding the double-free security problem.
     */
    
    /* Clean up after the write, and free any memory allocated */
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    /* Close the file */
    fclose(fp);
    
    free(row_pointers);
    
    /* That's it */
    return PNG_OK;
}


void pngFree(uint8_t **in_out_image) {
    free(*in_out_image);
    *in_out_image = NULL;
}
