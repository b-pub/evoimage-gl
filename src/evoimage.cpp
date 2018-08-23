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
/*
 * EvoImage - Evolutionary Image generation.
 *
 * This version uses the GD library.
 */

#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <stdio.h>

#include <gd.h>

#include "Settings.h"
#include "Tools.h"
#include "DnaDrawing.h"

static int g_imageNum = 0;


gdImagePtr renderDrawing(ei::DnaDrawing *d)
{
    // make new image, true color, with alpha support
    gdImagePtr img = 0;
    img = gdImageCreateTrueColor(ei::Tools::maxWidth, ei::Tools::maxHeight);
    if (0 == img)
    {
        std::cout << "Could not create image" << std::endl;
        return 0;
    }
    gdImageAlphaBlending(img, 1);
    int black = gdTrueColor(0,0,0);         // also acts as background

    // render image:
    // * for each polygon:
    ei::DnaPolygonList *polys = d->polygons();
    ei::DnaPolygonList::iterator iter;
    for (iter = polys->begin(); iter != polys->end(); iter++)
    {
        ei::DnaPolygon *poly = *iter;
        // ** allocate its color & alpha
        ei::DnaBrush *brush = poly->brush();
        int color = gdTrueColorAlpha(brush->r, brush->g, brush->b, brush->a);
        // ** render a closed polygon:
        // *** make array of gdPoints
        ei::DnaPointList *points = poly->points();
        gdPoint gdPts[points->size()];
        // *** translate points
        ei::DnaPointList::iterator eachPt;
        for (int i=0; i < points->size(); i++)
        {
            gdPts[i].x = (*points)[i]->x;
            gdPts[i].y = (*points)[i]->y;
        }
        // *** render via gdImageFilledPolygon().
        gdImageFilledPolygon(img, gdPts, points->size(), color);
    }

    return img;
}


void renderImageFile(gdImagePtr image, int imageIndex)
{
    // make new output image file
    char filename[100];
    sprintf(filename, "mutations/evoimg-%07d.png", imageIndex);
    FILE *outf = fopen(filename, "wb");
    if (NULL == outf)
    {
        std::cout << "Could not create file " << filename << std::endl;
        return;
    }

    // write image out to file
    gdImagePng(image, outf);

    // close file
    fclose(outf);
}

/*
 * A multithreaded implementation.
 */
typedef struct {
    gdImagePtr oldImage;
    gdImagePtr newImage;
    int rowStart;
    double result;
} diffImageMTArgs;

void* diffImagesMT(void *arg)
{
    diffImageMTArgs *args = (diffImageMTArgs*)arg;
    double difference = 0.0;
    int x,y, mx = ei::Tools::maxWidth, my = ei::Tools::maxHeight;
    // Loop Y then X. 6% faster for the GD library.
    for (y = args->rowStart; y < my; y += 2)
        for (x = 0; x < mx; x++)
        {
            int c1 = gdImageGetPixel(args->oldImage, x, y);
            int c2 = gdImageGetPixel(args->newImage, x, y);
            int r = gdTrueColorGetRed(c1) - gdTrueColorGetRed(c2);
            int g = gdTrueColorGetGreen(c1) - gdTrueColorGetGreen(c2);
            int b = gdTrueColorGetBlue(c1) - gdTrueColorGetBlue(c2);
            difference += (r*r + g*g + b*b);
        }
    args->result = difference;
    return 0;
}

double diffImages(gdImagePtr oldImage, gdImagePtr newImage)
{
    double differenceEvens = 0.0;
    double differenceOdds = 0.0;

    // subthread runs odds with diffImagesMT(oldImage, newImage, 1, &differenceOdds);
    pthread_t subThreadID = 0;
    diffImageMTArgs oddArgs = {oldImage, newImage, 1, 0.0};
    pthread_create( &subThreadID, NULL, diffImagesMT, &oddArgs);

    // main thread runs evens   diffImagesMT(oldImage, newImage, 0, &differenceEvens);
    diffImageMTArgs evenArgs = {oldImage, newImage, 0, 0.0};
    diffImagesMT(&evenArgs);

    // main thread finishes, and waits for subthread
    void *unusedResults;
    pthread_join(subThreadID, &unusedResults);

    return oddArgs.result + evenArgs.result;
}


void usage()
{
    std::cout << "usage: evoimage [options] environment.png" << std::endl
              << "Options:" << std::endl
              << "    -r n    Render every n generations (default 300)" << std::endl
              << "    -g n    Limit generations to n (default 10000)" << std::endl
              << "    -c n    Generate n (n=1..10) children per generation (default 1)" << std::endl
              << "    -s seed Initialize random number generator with seed" << std::endl
              << std::endl;
    exit(1);
}


typedef struct {
    int renderImageEvery;
    int numberOfChildren;
    int generationLimit;
    char *environmentFilename;
} ProgramArgs;

void checkArgs(int argc, char *argv[], ProgramArgs *args)
{
    int option;
    int temp;
    while (-1 != (option = getopt(argc, argv, "r:g:c:s:")) )
    {
        switch (option)
        {
          case 'r':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -r" << std::endl;
                usage();
            }
            args->renderImageEvery = temp;
            break;
          case 'g':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -g" << std::endl;
                usage();
            }
            args->generationLimit = temp;
            break;
          case 'c':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -c" << std::endl;
                usage();
            }
            args->numberOfChildren = temp;
            break;
          case 's':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -s" << std::endl;
                usage();
            }
            srand(temp);
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
        args->environmentFilename = argv[0];
    }
    else
    {
        std::cout << "No environment image file given." << std::endl;
        usage();
    }
    // Sanity check arguments
    if (args->renderImageEvery < 1 ||
        args->numberOfChildren < 1 || args->numberOfChildren > 10 ||
        args->generationLimit < 1
        )
    {
        std::cout << "Invalid values for some arguments given." << std::endl;
        usage();
    }
}

int main(int argc, char *argv[])
{
    ProgramArgs args = {300, 1, 10000, 0};
    int nextRenderedImage = 0;

    checkArgs(argc, argv, &args);
    std::cout << "Settings:" << std::endl
              << "    rendering image every ~" << args.renderImageEvery << std::endl
              << "    children/generation: " << args.numberOfChildren << std::endl
              << "    number of generations: " << args.generationLimit << std::endl
              << "    environment image: " << args.environmentFilename << std::endl;

    ei::Settings settings;
    settings.activate();

    // Mutation algorithm
    // Load environment image and perform sanity checks
    FILE *environmentFile = fopen(args.environmentFilename, "rb");
    if (!environmentFile)
    {
        std::cout << "Could not open " << args.environmentFilename << std::endl;
        return 1;
    }
    gdImagePtr environment = gdImageCreateFromPng(environmentFile);
    fclose(environmentFile);
    if (!environment)
    {
        std::cout << "Could not create image from " << args.environmentFilename << std::endl;
        return 1;
    }
    if (gdImageSX(environment) != ei::Tools::maxWidth ||
        gdImageSY(environment) != ei::Tools::maxHeight)
    {
        gdImageDestroy(environment);
        std::cout << "environment.png is incorrect size (expected "
                  << ei::Tools::maxWidth << "x" << ei::Tools::maxHeight
                  << ")" << std::endl;
        return 1;
    }

    // Generate 1st Drawing. Calc difference. Save image&diff as "last".
    ei::DnaDrawing *lastDrwg = new ei::DnaDrawing();
    lastDrwg->init();
    gdImagePtr tempImage = renderDrawing(lastDrwg);
    double lastDifference = diffImages(environment, tempImage);

    renderImageFile(environment, 0);     // save environment as 0
    renderImageFile(tempImage, 1);       // always save off first specimen as 1
    std::cout << "Initial difference = " << lastDifference << std::endl;
    gdImageDestroy(tempImage);

    // Iterate the following:
    for (int generationCount=2; generationCount <= args.generationLimit; generationCount++)
    {
        // Periodically report current convergence
        if (0 == generationCount % 5000)
        {
            std::cout << "Current difference is " << lastDifference 
                      << " at mutation " << generationCount << std::endl;
        }

        // 1. Clone last drawing and mutate. Save as "newDrwg"
        typedef struct {
            ei::DnaDrawing *drawing;
            gdImagePtr      image;
        } DrawingInfo;
        DrawingInfo children[args.numberOfChildren];
        int child;                          // looping index
        int minChild;                       // child with minimal difference
        double newDifference;
        for (child=0; child < args.numberOfChildren; child++)
        {
            children[child].drawing = lastDrwg->clone();
            children[child].drawing->mutate();

            // 2. Calc difference between child and environment.
            children[child].image = renderDrawing(children[child].drawing);
            double difference = diffImages(environment, children[child].image);

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
        if (newDifference < lastDifference)
        {
            // 3.1 free last image
            delete lastDrwg;

            // 3.2 save newDrwg&diff as "last"
            lastDrwg = children[minChild].drawing;
            lastDifference = newDifference;
            children[minChild].drawing = 0;

            // 3.3 render image to file named by iteration
            // but limit it to sparse changes.
            if (generationCount > nextRenderedImage)
            {
                renderImageFile(children[minChild].image, generationCount);
                // if every = 100, then next after 171 is (171/100 + 1)*100 = 200
                nextRenderedImage = ( (generationCount / args.renderImageEvery + 1) *
                                      args.renderImageEvery);

#if 0
                if (newDifference < 2.86891e7)
                {
                    std::cout << "Difference of " << newDifference
                              << " reached at " << generationCount
                              << " generations" << std::endl
                              << "for a total of "
                              << generationCount * args.numberOfChildren
                              << " mutations" << std::endl;
                    generationCount = args.generationLimit + 1; // terminate main loop
                }
#endif
            } // time to render an image
        } // new difference is lower

        // 4 clean up this iteration. If a child improved the
        // drawing, it's pointer will be 0 already. Delete all
        // DnaDrawings and gdImagePtrs.
        for (child=0; child < args.numberOfChildren; child++)
        {
            delete children[child].drawing;
            gdImageDestroy(children[child].image);
        }

    } // for each generation...

    std::cout << "Cleaning up" << std::endl;
    delete lastDrwg;
    gdImageDestroy(environment);

    return 0;
}
