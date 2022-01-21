/*
 *  evorender - render a previously saved JSON drawing to PNG.
 *  Part of evoimage-gl, a library and program to evolve images
 *  Copyright (C) 2009-2022 Brent Burton
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <cairo.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <cmath>
#include <cstdint>
#include <string>
#include <json/json.h>
#include <memory>

struct ProgramArgs {
    std::string inFilename;
    std::string outFilename;
    uint32_t width;                         // final rendered resolution
    uint32_t height;
};
ProgramArgs g_args {"", "", 200, 200};

void usage()
{
    std::cout << "usage: evorender -i input.json -o output.png [-w 200] [-h 200]\n"
              << "Switches:\n"
              << "    -i input.json  Input JSON, created by evoimagecairo\n"
              << "    -o output.png  Output PNG file.\n"
              << "    -w width       Output resolution width. Default is 200.\n"
              << "    -h height      Output resolution height. Default is 200.\n"
              << std::endl;
    exit(1);
}

void checkArgs(int argc, char *argv[])
{
    int option;
    int temp;
    while (-1 != (option = getopt(argc, argv, "i:o:w:h:")) )
    {
        switch (option)
        {
          case 'i':
            g_args.inFilename = optarg;
            break;
          case 'o':
            g_args.outFilename = optarg;
            break;
          case 'w':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -w\n";
                usage();
            }
            g_args.width = temp;
            break;
          case 'h':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -h\n";
                usage();
            }
            g_args.height = temp;
            break;

          case '?':
          default:
            usage();
            break;
        }
    }

    // Sanity check required args
    if (g_args.inFilename.length() == 0 ||
        g_args.outFilename.length() == 0)
    {
        std::cout << "ERROR: input and output filenames must be specified\n";
        usage();
    }
    if (g_args.width < 10 || g_args.height < 10)
    {
        std::cout << "ERROR: width and height must be at least 10\n";
        exit(1);
    }
}

void renderDrawing(Json::Value &drawing)
{
    cairo_surface_t *surface = 0;
    cairo_t *ctx = 0;

    surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, g_args.width, g_args.height);
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
    {
        throw std::logic_error("Internal error: couldn't create Cairo surface");
    }

    ctx = cairo_create(surface);
    if (cairo_status(ctx) != CAIRO_STATUS_SUCCESS)
    {
        cairo_surface_destroy(surface);
        throw std::logic_error("Internal error: couldn't create Cairo context");
    }

    // Clear the current buffer to black:
    cairo_pattern_t *pattern = cairo_pattern_create_rgb(0.0, 0.0, 0.0);
    cairo_set_source(ctx, pattern);
    cairo_pattern_destroy(pattern);
    cairo_paint(ctx);

    // render image:
    cairo_scale(ctx, g_args.width, g_args.height);

    Json::Value &polygons = drawing["polygons"];
    if (!polygons.isArray())
        throw std::logic_error("JSON 'polygons' is not an array");

    for (uint32_t i=0; i < polygons.size(); ++i)
    {
        Json::Value &points = polygons[i]["points"];
        if (!points.isArray())
            throw std::logic_error("JSON 'points' is not an array");

        cairo_move_to(ctx, points[0]["x"].asFloat(), points[0]["y"].asFloat());
        for (uint32_t j=1; j < points.size(); ++j)
        {
            cairo_line_to(ctx, points[j]["x"].asFloat(), points[j]["y"].asFloat());
        }
        cairo_close_path(ctx);

        // set color and alpha
        Json::Value &color = polygons[i]["color"];
        cairo_set_source_rgba(ctx,
                              color["r"].asFloat(), color["g"].asFloat(),
                              color["b"].asFloat(), color["a"].asFloat());

        cairo_fill(ctx); // fill and consume path
    }

    cairo_surface_write_to_png(surface, g_args.outFilename.c_str());
    cairo_destroy(ctx);
    cairo_surface_destroy(surface);
}

void loadAndRenderDrawing()
{
    // open file
    std::ifstream ins(g_args.inFilename);
    if (!ins.is_open())
        throw std::logic_error("Could not open input file");

    // read JSON
    Json::Value drawing;
    ins >> drawing;
    ins.close();

    renderDrawing(drawing);
}

int main(int argc, char **argv)
{
    checkArgs(argc, argv);

    std::cout << "Rendering " << g_args.inFilename
              << " to " << g_args.outFilename
              << " at " << g_args.width << 'x' << g_args.height
              << std::endl;

    try {
        loadAndRenderDrawing();
    }
    catch (std::exception &e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }

    return 0;
}
