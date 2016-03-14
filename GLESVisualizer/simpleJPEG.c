//
//  simpleJPEG.c
//  SimpleJPEG
//
//  Created by Michael Kwasnicki on 20.10.15.
//  Copyright © 2015 Michael Kwasnicki. All rights reserved.
//

#include "simpleJPEG.h"

#include <stdio.h>

#include <jpeglib.h> // lacks header completeness

#include <stdlib.h>
#include <string.h>



bool jpegIsJPEG(const uint8_t * const in_JPEG_DATA) {
    const uint8_t magic[3] = { 0xFF, 0xD8, 0xFF };
    return memcmp(in_JPEG_DATA, magic, 3) == 0;
}



bool jpegDecode(uint8_t ** const out_image, uint32_t * const out_width, uint32_t * const out_height, uint32_t * const out_numChannels, const uint8_t * const in_JPEG_DATA, const size_t in_JPEG_SIZE, const bool in_FLIP_Y) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    int row_stride;
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, in_JPEG_DATA, in_JPEG_SIZE);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
    
    JDIMENSION w = cinfo.image_width;
    JDIMENSION h = cinfo.image_height;
    uint32_t c = cinfo.num_components;
    uint8_t *image = malloc(w * h * c * sizeof(uint8_t));
    uint8_t *imagePtr = image;
    
    if (in_FLIP_Y) {
        imagePtr = image + w * (h - 1) * c;
    }
    
    row_stride = cinfo.output_width * cinfo.output_components;
    
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, &imagePtr, 1);
        
        if (in_FLIP_Y) {
            imagePtr -= row_stride;
        } else {
            imagePtr += row_stride;
        }
    }
    
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    
    *out_image = image;
    *out_width = w;
    *out_height = h;
    *out_numChannels = c;
    
    return true;
}



bool jpegRead(uint8_t ** const out_image, uint32_t * const out_width, uint32_t * const out_height, uint32_t * const out_numChannels, const char * const in_FILE_NAME, const bool in_FLIP_Y) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * infile;
    int row_stride;
    
    infile = fopen(in_FILE_NAME, "rb");
    
    if (infile == NULL) {
        fprintf(stderr, "can't open %s\n", in_FILE_NAME);
        return false;
    }
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
    
    JDIMENSION w = cinfo.image_width;
    JDIMENSION h = cinfo.image_height;
    uint32_t c = cinfo.num_components;
    uint8_t *image = malloc(w * h * c * sizeof(uint8_t));
    uint8_t *imagePtr = image;
    
    if (in_FLIP_Y) {
        imagePtr = image + w * (h - 1) * c;
    }
    
    row_stride = cinfo.output_width * cinfo.output_components;
    
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, &imagePtr, 1);
        
        if (in_FLIP_Y) {
            imagePtr -= row_stride;
        } else {
            imagePtr += row_stride;
        }
    }
    
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    
    *out_image = image;
    *out_width = w;
    *out_height = h;
    *out_numChannels = c;
    
    return true;
}



bool jpegWrite(const char * const in_FILE_NAME, const uint32_t in_QUALITY, const uint8_t * const in_IMAGE, const uint32_t in_WIDHT, const uint32_t in_HEIGHT, const uint32_t in_NUM_CHANNELS) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * outfile;
    JSAMPROW row_pointer[1];
    int row_stride;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    outfile = fopen(in_FILE_NAME, "wb");
    
    if (outfile == NULL) {
        fprintf(stderr, "can't open %s\n", in_FILE_NAME);
        return false;
    }
    
    jpeg_stdio_dest(&cinfo, outfile);
    cinfo.image_width = in_WIDHT;
    cinfo.image_height = in_HEIGHT;
    
    switch (in_NUM_CHANNELS) {
        case 1:
            cinfo.input_components = 1;
            cinfo.in_color_space = JCS_GRAYSCALE;
            break;
            
        case 3:
            cinfo.input_components = 3;
            cinfo.in_color_space = JCS_RGB;
            break;
            
        default:
            return false;
    }
    
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, in_QUALITY, TRUE);
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = in_WIDHT * 3;
    
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &in_IMAGE[cinfo.next_scanline * row_stride];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
    return true;
}



void jpegFree(uint8_t ** const in_out_image) {
    free(*in_out_image);
    *in_out_image = NULL;
}
