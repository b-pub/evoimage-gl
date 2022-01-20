/*
 *  Evoimage-gl, a library and program to evolve images
 *  Copyright (C) 2009 Brent Burton
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
#include <pthread.h>
#include <cmath>
#include <cstdint>
#include <string>
#include <json/json.h>
#include <memory>

#include "Settings.h"
#include "Tools.h"
#include "DnaDrawing.h"

// Func prototypes
static void doNextMutation();               // do next mutation & compare
static void generateFirstDrawing();
static int loadEnvironmentPng();
static cairo_surface_t *renderDrawing(ei::DnaDrawing *d);

// global variables
static int g_generationCount = 0;
static int g_imageNum = 0;
static const int g_width  = 200;
static const int g_height = 200;

static cairo_surface_t *g_environmentImage;
ei::DnaDrawing *g_lastDrawing = 0;
uint32_t g_lastDifference;

time_t g_startTime = 0;
time_t g_endTime = 0;

typedef struct {
    int renderImageEvery;
    int numberOfChildren;
    int generationLimit;
    int polygonsMax;
    int pointsMax;
    char *environmentFilename;
    std::string jsonFilename;
} ProgramArgs;

ProgramArgs g_programArgs = {300, 1, 10000, 50, 20, 0};


// other imaging routines
static cairo_surface_t* renderDrawing(ei::DnaDrawing *d)
{
    cairo_surface_t *surface = 0;
    cairo_t *ctx = 0;

    surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, g_width, g_height);
    if (cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS)
    {
        ctx = cairo_create(surface);
        if (cairo_status(ctx) == CAIRO_STATUS_SUCCESS)
        {

            // Clear the current buffer to black:
            cairo_pattern_t *pattern = cairo_pattern_create_rgb(0.0, 0.0, 0.0);
            cairo_set_source(ctx, pattern);
            cairo_pattern_destroy(pattern);
            cairo_paint(ctx);                       // fills clip region

            // render image:
            // * for each polygon:
            ei::DnaPolygonList &polys = d->polygons();
            ei::DnaPolygonList::iterator iter;
            for (iter = polys.begin(); iter != polys.end(); iter++)
            {
                ei::DnaPolygon &poly = *iter;
                // Create path:
                ei::DnaPointList &points = poly.points();
                cairo_move_to(ctx, points[0].x, points[0].y);
                for (int i=1; i < points.size(); i++)
                {
                    cairo_line_to(ctx, points[i].x, points[i].y);
                }
                cairo_close_path(ctx);

                // ** allocate its color & alpha
                ei::DnaBrush &brush = poly.brush();
                cairo_set_source_rgba(ctx,
                                      brush.r / 255.0, brush.g / 255.0,
                                      brush.b / 255.0, brush.a / 255.0);

                cairo_fill(ctx); // fill and consume path
            }
        }
        else                    // context could not be allocated
        {
            cairo_destroy(ctx);
            ctx = 0;
            cairo_surface_destroy(surface);
            surface = 0;
        }
    }
    else                        // surface could not be allocated
    {
        cairo_surface_destroy(surface);
        surface = 0;
    }

    if (ctx)
        cairo_destroy(ctx);

    return surface;
}


void renderImageFile(cairo_surface_t *image, int imageIndex)
{
    // make new output image file
    char filename[100];
    sprintf(filename, "mutations/evoimg-%07d.png", imageIndex);

    // write image out to file
    cairo_surface_write_to_png(image, filename);
}

/*
 * A multithreaded implementation.
 */
typedef struct {
    cairo_surface_t *oldImage;
    cairo_surface_t *newImage;
    int rowStart;
    int rowEnd;
    uint32_t result;
} diffImageMTArgs;

void* diffImagesWorker(void *arg)
{
    diffImageMTArgs *args = (diffImageMTArgs*)arg;
    uint32_t difference = 0;
    int x,y, mx = g_width, my = args->rowEnd;

    for (y = args->rowStart; y < my; y++)
    {
        uint8_t *rowOld = (cairo_image_surface_get_data(args->oldImage) +
                           y * cairo_image_surface_get_stride(args->oldImage));
        uint8_t *rowNew = (cairo_image_surface_get_data(args->newImage) +
                           y * cairo_image_surface_get_stride(args->newImage));

        for (x = 0; x < mx; x++)
        {
            uint8_t *c1 = (rowOld + x * 4);
            uint8_t *c2 = (rowNew + x * 4);
            int r = c1[2] - c2[2];
            int g = c1[1] - c2[1];
            int b = c1[0] - c2[0];
            difference += (uint32_t)std::sqrt(r*r + g*g + b*b);
        }
    }
    args->result = difference;
    return 0;
}

#define SINGLE_THREAD 1

#if SINGLE_THREAD

uint32_t diffImages(cairo_surface_t *oldImage, cairo_surface_t *newImage)
{
    diffImageMTArgs bottomArgs = {oldImage, newImage, 0, g_height, 0};
    diffImagesWorker(&bottomArgs);
    return bottomArgs.result;
}

#else

uint32_t diffImages(cairo_surface_t *oldImage, cairo_surface_t *newImage)
{
    // subthread runs top half
    pthread_t subThreadID = 0;
    diffImageMTArgs topArgs = {oldImage, newImage, 0, g_height/2, 0};
    pthread_create(&subThreadID, NULL, diffImagesWorker, &topArgs);

    // main thread runs bottom half
    diffImageMTArgs bottomArgs = {oldImage, newImage, g_height/2, g_height, 0};
    diffImagesWorker(&bottomArgs);

    // main thread finishes, and waits for subthread
    void *unusedResults;
    pthread_join(subThreadID, &unusedResults);

    return topArgs.result + bottomArgs.result;
}
#endif // !SINGLE_THREAD


void usage()
{
    std::cout << "usage: evoimage [options] environment.png\n"
              << "Options:\n"
              << "    -r n    Render every n generations (default 300)\n"
              << "    -g n    Limit generations to n (default 10000)\n"
              << "    -c n    Generate n (n=1..10) children per generation (default 1)\n"
              << "    -s seed Initialize random number generator with seed\n"
              << "    -p n    Set maximum number of polygons used (default 50)\n"
              << "    -v n    Set maximum number of vertices/polygon used (default 20)\n"
              << "    -j file Save final image geometry as JSON 'file'\n"
              << std::endl
              << "The environment.png file must have a resolution of 200x200.\n";
    exit(1);
}

void checkArgs(int argc, char *argv[])
{
    int option;
    int temp;
    while (-1 != (option = getopt(argc, argv, "r:g:c:s:p:v:j:")) )
    {
        switch (option)
        {
          case 'r':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -r\n";
                usage();
            }
            g_programArgs.renderImageEvery = temp;
            break;
          case 'g':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -g\n";
                usage();
            }
            g_programArgs.generationLimit = temp;
            break;
          case 'c':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -c\n";
                usage();
            }
            g_programArgs.numberOfChildren = temp;
            break;
          case 's':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -s\n";
                usage();
            }
            std::cout << "Seeding rand() with " << temp << std::endl;
            srand(temp);
            break;
          case 'p':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -p\n";
                usage();
            }
            g_programArgs.polygonsMax = temp;
            break;
          case 'v':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -v\n";
                usage();
            }
            if (temp < 3)
            {
                std::cout << "warning: polygons need at least 3 vertices (fixed)\n";
                temp = 3;
            }
            g_programArgs.pointsMax = temp;
            break;

          case 'j':
            g_programArgs.jsonFilename = optarg;
            break;

          default:
            std::cout << "unrecognized switch " << (char)option << std::endl;
          case 'h':
          case '?':
            usage();
        }
    }
    argc -= optind;
    argv += optind;

    if (argc == 1)                      // get environment filename
    {
        g_programArgs.environmentFilename = argv[0];
    }
    else
    {
        std::cout << "No environment image file given.\n";
        usage();
    }
    // Sanity check arguments
    if (g_programArgs.renderImageEvery < 1 ||
        g_programArgs.numberOfChildren < 1 || g_programArgs.numberOfChildren > 10 ||
        g_programArgs.generationLimit < 1
        )
    {
        std::cout << "Invalid values for some arguments given.\n";
        usage();
    }
}

static int loadEnvironmentPng()
{
    // Load environment image and perform sanity checks
    g_environmentImage = cairo_image_surface_create_from_png(g_programArgs.environmentFilename);
    if (cairo_surface_status(g_environmentImage) != CAIRO_STATUS_SUCCESS)
    {
        std::cout << "Could not open " << g_programArgs.environmentFilename << std::endl;
        return 0;
    }

    return 1;
}

static void generateFirstDrawing()
{
    // Generate 1st Drawing. Calc difference. Save image&diff as "last".
    g_lastDrawing = new ei::DnaDrawing();
    g_lastDrawing->init();
    cairo_surface_t *tempImage = renderDrawing(g_lastDrawing);
    g_lastDifference = diffImages(g_environmentImage, tempImage);

    renderImageFile(g_environmentImage, 0);     // save environment as 0
    renderImageFile(tempImage, 1);       // always save off first specimen as 1
    std::cout << "Initial difference = " << g_lastDifference << std::endl;
    cairo_surface_destroy(tempImage);
}

static void generateLastDrawing()
{
    cairo_surface_t *tempImage = renderDrawing(g_lastDrawing);
    renderImageFile(tempImage, g_programArgs.generationLimit);
    cairo_surface_destroy(tempImage);
}

static void doNextMutation()
{
    static int nextRenderedImage = 0;

    // Mutation algorithm
    if (g_generationCount <= g_programArgs.generationLimit)
    {
        // Periodically report current convergence
        if (0 == g_generationCount % 2000)
        {
            std::cout << "Current difference is " << g_lastDifference 
                      << " at generation " << g_generationCount << ". "
                      << g_lastDrawing->polygons().size() << " polys, "
                      << g_lastDrawing->pointCount() << " points"
                      << std::endl;
        }

        // 1. Clone last drawing and mutate.
        typedef struct {
            ei::DnaDrawing  *drawing;
            cairo_surface_t *image;
        } DrawingInfo;

        DrawingInfo children[g_programArgs.numberOfChildren];

        int child;                          // looping index
        int minChild;                       // child with minimal difference
        uint32_t newDifference;

        for (child=0; child < g_programArgs.numberOfChildren; child++)
        {
            children[child].drawing = g_lastDrawing->clone();
            children[child].drawing->mutate();

            // 2. Calc difference between child and environment.
            children[child].image = renderDrawing(children[child].drawing);
            uint32_t difference = diffImages(g_environmentImage, children[child].image);

            // Locate child with the best fit to environment (smallest difference)
            if (child == 0)
            {
                newDifference = difference;
                minChild = 0;
            }
            else
            {
                if (difference < newDifference) // found new min
                {
                    minChild = child;
                    newDifference = difference;
                }
            }
        }

        // 3. If a child's difference is less than last difference, then save it
        if (newDifference < g_lastDifference)
        {
            // 3.1 free last image
            delete g_lastDrawing;

            // 3.2 save newDrwg&diff as "last"
            g_lastDrawing = children[minChild].drawing;
            g_lastDifference = newDifference;
            children[minChild].drawing = 0;

            // 3.3 render image to file named by iteration
            // but limit it to sparse changes.
            if (g_generationCount > nextRenderedImage)
            {
                renderImageFile(children[minChild].image, g_generationCount);
                // if every = 100, then next after 171 is (171/100 + 1)*100 = 200
                nextRenderedImage = ( (g_generationCount / g_programArgs.renderImageEvery + 1) *
                                      g_programArgs.renderImageEvery);
            } // time to render an image
        } // new difference is lower

        // 4 clean up this iteration. If a child improved the
        // drawing, it's pointer will be 0 already. Delete all
        // DnaDrawings and gdImagePtrs.
        for (child=0; child < g_programArgs.numberOfChildren; child++)
        {
            delete children[child].drawing;
            cairo_surface_destroy(children[child].image);
        }
    }
}

/*
 * Write out drawing's polygons in JSON, if a filename was given.
 */
void saveDrawingJson(ei::DnaDrawing *drawing)
{
    if (0 == g_programArgs.jsonFilename.length())
        return;

    std::ofstream outfile(g_programArgs.jsonFilename);
    if (!outfile.is_open())
    {
        std::cout << "Cannot open JSON output file "
                  << g_programArgs.jsonFilename << std::endl;
        return;
    }

    /*
     * Convert to JSON with this representation:
     * * A drawing contains an array of polygons
     * * A polygon has a color, and an array of points
     * * A color is R,G,B,A values as doubles [0,1]
     * * A point is X,Y coordinates as doubles [0,1]
     */
    Json::Value drwg;
    Json::Value polygons(Json::arrayValue);
    for (auto dnapoly: drawing->polygons())
    {
        Json::Value color;
        color["r"] = dnapoly.brush().r / 255.0;
        color["g"] = dnapoly.brush().g / 255.0;
        color["b"] = dnapoly.brush().b / 255.0;
        color["a"] = dnapoly.brush().a / 255.0;

        Json::Value points(Json::arrayValue);
        for (auto dnapoint: dnapoly.points())
        {
            Json::Value ptval;
            ptval["x"] = dnapoint.x / 200.0;
            ptval["y"] = dnapoint.y / 200.0;
            points.append(ptval);
        }

        Json::Value polygon;
        polygon["color"] = color;
        polygon["points"] = points;

        polygons.append(polygon);
    }
    drwg["polygons"] = polygons;

    // write it out and close the file
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "    ";
    std::unique_ptr<Json::StreamWriter> writer( builder.newStreamWriter() );
    writer->write(drwg, &outfile);
    outfile.close();
    std::cout << "Wrote drawing to JSON file "
              << g_programArgs.jsonFilename << std::endl;

    return;
}

int main(int argc, char *argv[])
{
    int nextRenderedImage = 0;

    checkArgs(argc, argv);
    std::cout << "Settings:\n"
              << "    rendering image every ~"
              << g_programArgs.renderImageEvery << std::endl
              << "    children/generation: "
              << g_programArgs.numberOfChildren << std::endl
              << "    number of generations: "
              << g_programArgs.generationLimit << std::endl
              << "    environment image: "
              << g_programArgs.environmentFilename << std::endl
              << "    max polygons: "
              << g_programArgs.polygonsMax << std::endl
              << "    max points/poly: "
              << g_programArgs.pointsMax << std::endl;

    ei::Settings settings;
    settings.setPolygonsMax(g_programArgs.polygonsMax);
    settings.setPointsPerPolygonMax(g_programArgs.pointsMax);
    settings.activate();

    // Iterate the generations
    if (!loadEnvironmentPng())
        exit(0);

    g_startTime = time(NULL);
    while (g_generationCount <= g_programArgs.generationLimit) {
        if (g_generationCount == 0)
        {
            g_startTime = time(NULL);
            generateFirstDrawing();
        }
        else // all other increments
        {
            doNextMutation();
        }

        ++g_generationCount;
    }
    g_endTime = time(NULL);
    std::cout << g_programArgs.generationLimit << " generations done in "
              << difftime(g_endTime, g_startTime) << " seconds\n";

    generateLastDrawing();

    saveDrawingJson(g_lastDrawing);

    delete g_lastDrawing;
    cairo_surface_destroy(g_environmentImage);

    return 0;
}

