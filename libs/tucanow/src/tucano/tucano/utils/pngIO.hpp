/**
 * Tucano - A library for rapid prototying with Modern OpenGL and GLSL
 * Copyright (C) 2014
 * LCG - Laboratório de Computação Gráfica (Computer Graphics Lab) - COPPE
 * UFRJ - Federal University of Rio de Janeiro
 *
 * This file is part of Tucano Library.
 *
 * Tucano Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tucano Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tucano Library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PNGIO__
#define __PNGIO__

#include <tucano/framebuffer.hpp>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3

#include <png.h>


namespace Tucano
{

namespace ImageImporter
{

#if defined(_WIN32) && defined(_MSC_VER)
    static bool loadPNGImage (string filename, Tucano::Texture *tex);
    static bool writePNGImage (string filename, Tucano::Framebuffer* fbo, int attach = 0);
#else
    static bool loadPNGImage (string filename, Tucano::Texture *tex) __attribute__ ((unused));
    static bool writePNGImage (string filename, Tucano::Framebuffer* fbo, int attach = 0) __attribute__ ((unused));
#endif

static void abort_(const char * s, ...)
{
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
}


/* @brief Loads a texture from a PNG file using pnglib.
 * The texture receives data in the range [0,1] to create a FLOAT texture
 *
 * @param filename Given filename of the PNG file.
 * @param tex Pointer to the texture
 * @return True if loaded successfully, false otherwise
 */
static bool loadPNGImage (string filename, Tucano::Texture *tex)
{
    unsigned char header[8];    // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp)
            abort_("[read_png_file] File %s could not be opened for reading", filename);
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
            abort_("[read_png_file] File %s is not recognized as a PNG file", filename);


    /* initialize stuff */
    auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
            abort_("[read_png_file] png_create_read_struct failed");

    auto info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
            abort_("[read_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
            abort_("[read_png_file] Error during init_io");

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    auto width = png_get_image_width(png_ptr, info_ptr);
    auto height = png_get_image_height(png_ptr, info_ptr);
    auto color_type = png_get_color_type(png_ptr, info_ptr);
    auto bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    auto number_of_passes = png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);


    /* read file */
    if (setjmp(png_jmpbuf(png_ptr)))
            abort_("[read_png_file] Error during read_image");

    auto row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
    for (unsigned int y = 0; y < height; y++)
            row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

    png_read_image(png_ptr, row_pointers);

    fclose(fp);

    std::cout << "color type " << color_type << std::endl;

    int d = 3;
    if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
        d = 3;
    else if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGBA)
        d = 4;

    vector<float> data;
    for (unsigned int y = 0; y < height; y++) {
            png_byte* row = row_pointers[y];
            for (unsigned int x = 0; x < width; x++) {
                    png_byte* ptr = &(row[x*4]);
//                    printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
 //                          x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

                    /* set red value to 0 and green value to the blue one */
                    ptr[0] = 0;
                    ptr[1] = ptr[2];
                    data.push_back(ptr[0]);
                    data.push_back(ptr[1]);
                    data.push_back(ptr[2]);
                    if (d == 4)
                        data.push_back(ptr[3]);
            }
    }


    if (d == 3)
        tex->create (GL_TEXTURE_2D, GL_RGBA32F, width, height, GL_RGB, GL_FLOAT, &data[0], 0);
    else if (d == 4)
        tex->create (GL_TEXTURE_2D, GL_RGBA32F, width, height, GL_RGBA, GL_FLOAT, &data[0], 0);

    #ifdef TUCANODEBUG
    Tucano::Misc::errorCheckFunc(__FILE__, __LINE__, "load PAM");
    #endif

    return true;
}


static bool loadPNGImageOLD (string filename, Tucano::Texture *tex)
{
    ifstream in(filename.c_str(), std::ios::in | std::ios::binary);
    if (!in)
    {
        cerr << "Cannot open " << filename.c_str() << endl; exit(1);
        return false;
    }

    vector<float> data;
    string header, tuple;
    bool magic_number = false;
    int w, h, d, max_value;;

    in >> header;
    while (header.compare("ENDHDR") != 0)
    {
        if (header.compare("WIDTH") == 0)
           in >> w;
        else if (header.compare("HEIGHT") == 0)
           in >> h;
        else if (header.compare("DEPTH") == 0)
           in >> d;
        else if (header.compare("MAXVAL") == 0)
           in >> max_value;
        else if (header.compare("TUPLTYPE") == 0)
            in >> tuple;
        else if (header.compare("P7") == 0)
            magic_number = true;
        in >> header;
    }
    getline(in, header);

    if (!magic_number)
    {
        std::cerr << "Error opening : " << filename.c_str()  << " : Invalid Magic Number, should be P7" << std::endl;
        return false;
    }
    if (w < 1 || h < 1 || d < 1)
    {
        std::cerr << "Error opening : " << filename.c_str() << "Invalid dimension, WIDTH HEIGHT and DEPTH should be at least 1" << std::endl;
        return false;
    }
    
    /// @TODO should check number of necessary bytes from MAXVAL, for now assuming 1 byte but it could also be 2

    unsigned char byte_value = 0;
    unsigned int value = 0;
    while (in.read(reinterpret_cast< char* >(&byte_value), sizeof(byte_value)) )
    {
        value = byte_value;
        data.push_back(value/(float)max_value);
    }

    if (data.size() != (unsigned int)(w*h*d))
    {
        std::cerr << "possible error reading PAM file:\n read " << data.size()*sizeof(byte_value) << " bytes, should have read " << w*h*d*sizeof(byte_value) << std::endl;
    }

    if(in.is_open())
    {
        in.close();
    }

	// convert from grayscale_alpha to rgb_alpha
	bool convertGAToRGBA = !tuple.compare("GRAYSCALE_ALPHA");

    // flip texture since it will be upside down (invertex y axis)
    vector<float> flipped;
    for (int j = h-1; j >= 0; j--)
    {
        for (int i = 0; i < w; i++)
        {            
            for (int k = 0; k < d; ++k)
            {
                flipped.push_back( data[(j*w + i)*d + k]);
				// if grayscale_alpha convert to rgba_alpha (repeat value for G and B channels)
				if (convertGAToRGBA && k == 0)
				{				
                	flipped.push_back( data[(j*w + i)*d + k]);
                	flipped.push_back( data[(j*w + i)*d + k]);
				}
            }
        }
    }

    if (d == 3)
        tex->create (GL_TEXTURE_2D, GL_RGBA32F, w, h, GL_RGB, GL_FLOAT, &flipped[0], 0);
    else if (d == 4 || convertGAToRGBA)
        tex->create (GL_TEXTURE_2D, GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, &flipped[0], 0);

    #ifdef TUCANODEBUG
    Tucano::Misc::errorCheckFunc(__FILE__, __LINE__, "load PAM");
    #endif

    return true;
}



/**
 * @brief Saves a framebuffer attachment to an image file
 * Note that since the input is a float image, the max value is arbitrary
 * For now we support only 1 byte per value
 * We are also assuming writing to an RGBA format
 *
 * @param filename Image file to save fbo attach
 * @param fbo Pointer to the Framebuffer
 * @param attach Attchment number to be saved
 */
static bool writePNGImage (string filename, Tucano::Framebuffer* fbo, int attach)
{
    int max_value = 255;
    Eigen::Vector2i size = fbo->getDimensions();
    ofstream out_stream;
    out_stream.open(filename.c_str(), ios::out);
    out_stream << "P7\n";
    out_stream << "WIDTH " << size[0] <<"\n " << "HEIGHT " << size[1] << "\n";
    out_stream << "DEPTH 4\n";
    out_stream << "MAXVAL " << max_value << "\n";
    out_stream << "TUPLTYPE RGB_ALPHA\n";
    out_stream << "ENDHDR\n";

    GLfloat * pixels = new GLfloat[(int)(size[0]*size[1]*4)];
    fbo->bind();
    glReadBuffer(GL_COLOR_ATTACHMENT0+attach);
    glReadPixels(0, 0, size[0], size[1], GL_RGBA, GL_FLOAT, pixels);

    out_stream.close();
    out_stream.open(filename.c_str(), ios::out | ios::app | ios::binary);
    int pos;
    unsigned int value;
    for (int j = size[1]-1; j >= 0; --j)
    {
        for (int i = 0 ; i < size[0]; ++i)
        {
            pos = (i + size[0]*j)*4;
            for (int k = 0; k < 4; ++k)
            {
                value = (unsigned int) (max_value*pixels[pos+k]);
                out_stream.write(reinterpret_cast< char* >(&value), sizeof(value)) ;
            }
        }
        out_stream << "\n";
    }
    out_stream.close();

    fbo->unbind();
    delete [] pixels;
    return true;
}


}
}
#endif
