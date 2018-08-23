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

#if defined(__APPLE__)
  #include <OpenGL/gl.h>
  #include <GLUT/glut.h>
#else
  #include <GL/gl.h>
  #include <GL/glut.h>
#endif // APPLE

#include <png.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "Settings.h"
#include "Tools.h"
#include "DnaDrawing.h"
#include "RawImage.h"

// Func prototypes
static void doNextMutation();               // do next mutation & compare
static void generateFirstDrawing();
static int loadEnvironmentPng();
static RawImage* renderDrawing(ei::DnaDrawing *d, bool returnRawImage);

// global variables
static int g_generationCount = 0;
static int g_imageNum = 0;
static const GLint g_width  = 200;
static const GLint g_height = 200;

static RawImage *g_environmentImage;
ei::DnaDrawing *g_lastDrawing = 0;
double g_lastDifference;

time_t g_startTime = 0;
time_t g_endTime = 0;

typedef struct {
    int renderImageEvery;
    int numberOfChildren;
    int generationLimit;
    int polygonsMax;
    int pointsMax;
    char *environmentFilename;
} ProgramArgs;

ProgramArgs g_programArgs = {300, 1, 10000, 50, 20, 0};


static void initializeGl()
{
    // Stretch viewport and projection to twice what we want.
    // Draw in left half, "last results" in right half
    glViewport(0, 0, g_width, g_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, g_width,
            0.0, g_height,
            1.0, -1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0f, g_height, 0.0f);     // move "up" in Y, 
    glScalef(1.0f, -1.0f, 1.0f);            // then flip Y

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// GLUT callbacks

/*
 * display() is the GLUT display callback that does a bit more
 * than redisplay the image. It displays the current image results
 * but also triggers mutations and renders mutants.
 */
static void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (g_generationCount == 0)
    {
        if (!loadEnvironmentPng())
            exit(0);
    }
    else if (g_generationCount == 1)
    {
        g_startTime = time(NULL);
        generateFirstDrawing();
    }
    else if (g_generationCount > g_programArgs.generationLimit)
    {
        g_endTime = time(NULL);
        std::cout << g_programArgs.generationLimit << " generations done in "
                  << difftime(g_endTime, g_startTime) << " seconds" << std::endl;
        glutIdleFunc(NULL);
    }
    else // all other increments
    {
        // 1. Periodically copy last results (if exist) to right
        // half of window @(200,200)-(400,200)
        glViewport(g_width, 0, g_width, g_height);
        renderDrawing(g_lastDrawing, false);

        // 2. render mutants in left half of window
        // @(0,0)-(200,200)
        glViewport(0, 0, g_width, g_height);
        doNextMutation();
    }

    ++g_generationCount;
}

static void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
      case 27: // ESC
      case 'q':
        exit(0);
        break;
    }
}

/*
 * mutationEngine() is the GLUT idle function that
 * triggers mutations by requesting redisplay.
 */
static void mutationEngine()
{
    glutPostRedisplay();
}

static void visible(int vis)
{
    if (vis == GLUT_VISIBLE)
        glutIdleFunc(mutationEngine);
    else
        glutIdleFunc(NULL);
}


// other imaging routines
static RawImage* renderDrawing(ei::DnaDrawing *d, bool returnRawImage)
{
    // Do a clear (to black) of the current viewport via a rect, not a glClear():
    glColor4f(0.0, 0.0, 0.0, 1.0);
    glRectf(0.0f, 0.0f, g_width, g_height);

    // render image:
    // * for each polygon:
    ei::DnaPolygonList *polys = d->polygons();
    ei::DnaPolygonList::iterator iter;
    for (iter = polys->begin(); iter != polys->end(); iter++)
    {
        // GL Abuse: DnaPolygon has no guarantee of being convex,
        // so results are unpredictable. Try this for now.
        glBegin(GL_POLYGON);
        {
            ei::DnaPolygon *poly = *iter;
            // ** allocate its color & alpha
            ei::DnaBrush *brush = poly->brush();
            glColor4b(brush->r, brush->g, brush->b, brush->a);
            // ** render a closed polygon:
            ei::DnaPointList *points = poly->points();
            // *** translate points
            for (int i=0; i < points->size(); i++)
            {
                glVertex2f( (*points)[i]->x, (*points)[i]->y);
            }
        }
        glEnd();
    }
    glFinish();

    RawImage *img = 0;

    if (returnRawImage)
    {
        img = new RawImage(g_width, g_height);

        glReadPixels(0, 0, g_width, g_height, GL_RGB,
                     GL_UNSIGNED_BYTE, img->getPixels());
    }

    return img;
}


void renderImageFile(RawImage *image, int imageIndex)
{
    // make new output image file
    char filename[100];
    sprintf(filename, "mutations/evoimg-%07d.png", imageIndex);

    // write image out to file
    image->writeToPng(filename);
}

/*
 * A multithreaded implementation.
 */
typedef struct {
    RawImage *oldImage;
    RawImage *newImage;
    int rowStart;
    int rowEnd;
    double result;
} diffImageMTArgs;

void* diffImagesMT(void *arg)
{
    diffImageMTArgs *args = (diffImageMTArgs*)arg;
    double difference = 0.0;
    int x,y, mx = g_width, my = args->rowEnd;

    for (y = args->rowStart; y < my; y++)
        for (x = 0; x < mx; x++)
        {
            GLubyte *c1 = args->oldImage->getPixelAt(x, y);
            GLubyte *c2 = args->newImage->getPixelAt(x, g_height-y-1); // flip Y
            int r = c1[0] - c2[0];
            int g = c1[1] - c2[1];
            int b = c1[2] - c2[2];
            difference += (r*r + g*g + b*b);
        }
    args->result = difference;
    return 0;
}

double diffImages(RawImage *oldImage, RawImage *newImage)
{
    double differenceEvens = 0.0;
    double differenceOdds = 0.0;

    // subthread runs top half
    pthread_t subThreadID = 0;
    diffImageMTArgs topArgs = {oldImage, newImage, 0, g_height/2, 0.0};
    pthread_create(&subThreadID, NULL, diffImagesMT, &topArgs);

    // main thread runs bottome half
    diffImageMTArgs bottomArgs = {oldImage, newImage, g_height/2, g_height, 0.0};
    diffImagesMT(&bottomArgs);

    // main thread finishes, and waits for subthread
    void *unusedResults;
    pthread_join(subThreadID, &unusedResults);

    return topArgs.result + bottomArgs.result;
}


void usage()
{
    std::cout << "usage: evoimage [options] environment.png" << std::endl
              << "Options:" << std::endl
              << "    -r n    Render every n generations (default 300)" << std::endl
              << "    -g n    Limit generations to n (default 10000)" << std::endl
              << "    -c n    Generate n (n=1..10) children per generation (default 1)" << std::endl
              << "    -s seed Initialize random number generator with seed" << std::endl
              << "    -p n    Set maximum number of polygons used (default 50)" << std::endl
              << "    -v n    Set maximum number of vertices/polygon used (default 20)" << std::endl
              << std::endl
              << "The environment.png file must have a resolution of 200x200."  << std::endl;
    exit(1);
}

void checkArgs(int argc, char *argv[])
{
    int option;
    int temp;
    while (-1 != (option = getopt(argc, argv, "r:g:c:s:p:v:")) )
    {
        switch (option)
        {
          case 'r':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -r" << std::endl;
                usage();
            }
            g_programArgs.renderImageEvery = temp;
            break;
          case 'g':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -g" << std::endl;
                usage();
            }
            g_programArgs.generationLimit = temp;
            break;
          case 'c':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -c" << std::endl;
                usage();
            }
            g_programArgs.numberOfChildren = temp;
            break;
          case 's':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -s" << std::endl;
                usage();
            }
            srand(temp);
            break;
          case 'p':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -p" << std::endl;
                usage();
            }
            g_programArgs.polygonsMax = temp;
            break;
          case 'v':
            if (1 != sscanf(optarg, "%d", &temp))
            {
                std::cout << "invalid number for -v" << std::endl;
                usage();
            }
            if (temp < 3)
            {
                std::cout << "warning: polygons need at least 3 vertices (fixed)" << std::endl;
                temp = 3;
            }
            g_programArgs.pointsMax = temp;
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
        std::cout << "No environment image file given." << std::endl;
        usage();
    }
    // Sanity check arguments
    if (g_programArgs.renderImageEvery < 1 ||
        g_programArgs.numberOfChildren < 1 || g_programArgs.numberOfChildren > 10 ||
        g_programArgs.generationLimit < 1
        )
    {
        std::cout << "Invalid values for some arguments given." << std::endl;
        usage();
    }
}

static int loadEnvironmentPng()
{
    // Load environment image and perform sanity checks
    g_environmentImage = new RawImage(g_width, g_height);
    if (!g_environmentImage->readFromPng(g_programArgs.environmentFilename))
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
    RawImage *tempImage = renderDrawing(g_lastDrawing, true);
    g_lastDifference = diffImages(g_environmentImage, tempImage);

    renderImageFile(g_environmentImage, 0);     // save environment as 0
    tempImage->invertRowsOnWrite(true);
    renderImageFile(tempImage, 1);       // always save off first specimen as 1
    std::cout << "Initial difference = " << g_lastDifference << std::endl;
    delete tempImage;
}


static void doNextMutation()
{
    static int nextRenderedImage = 0;

    // Mutation algorithm
    if (g_generationCount <= g_programArgs.generationLimit)
    {
        // Periodically report current convergence
        if (0 == g_generationCount % 5000)
        {
            std::cout << "Current difference is " << g_lastDifference 
                      << " at generation " << g_generationCount << ". "
                      << g_lastDrawing->polygons()->size() << " polys, "
                      << g_lastDrawing->pointCount() << " points"
                      << std::endl;
        }

        // 1. Clone last drawing and mutate. Save as "newDrwg"
        typedef struct {
            ei::DnaDrawing *drawing;
            RawImage       *image;
        } DrawingInfo;
        DrawingInfo children[g_programArgs.numberOfChildren];
        int child;                          // looping index
        int minChild;                       // child with minimal difference
        double newDifference;
        for (child=0; child < g_programArgs.numberOfChildren; child++)
        {
            children[child].drawing = g_lastDrawing->clone();
            children[child].drawing->mutate();

            // 2. Calc difference between child and environment.
            children[child].image = renderDrawing(children[child].drawing, true);
            double difference = diffImages(g_environmentImage, children[child].image);

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
                children[minChild].image->invertRowsOnWrite(true);
                renderImageFile(children[minChild].image, g_generationCount);
                // if every = 100, then next after 171 is (171/100 + 1)*100 = 200
                nextRenderedImage = ( (g_generationCount / g_programArgs.renderImageEvery + 1) *
                                      g_programArgs.renderImageEvery);

#if 0
                if (newDifference < 2.86891e7)
                {
                    std::cout << "Difference of " << newDifference
                              << " reached at " << g_generationCount
                              << " generations" << std::endl
                              << "for a total of "
                              << g_generationCount * g_programArgs.numberOfChildren
                              << " mutations" << std::endl;
                    g_generationCount = g_programArgs.generationLimit + 1; // terminate main loop
                }
#endif
            } // time to render an image
        } // new difference is lower

        // 4 clean up this iteration. If a child improved the
        // drawing, it's pointer will be 0 already. Delete all
        // DnaDrawings and gdImagePtrs.
        for (child=0; child < g_programArgs.numberOfChildren; child++)
        {
            delete children[child].drawing;
            delete children[child].image;
        }

    } // for each generation...

}

void cleanup()
{
    std::cout << "Cleaning up" << std::endl;
    delete g_lastDrawing;
    delete g_environmentImage;
}

int main(int argc, char *argv[])
{
    int nextRenderedImage = 0;

    glutInit(&argc, argv);

    checkArgs(argc, argv);
    std::cout << "Settings:" << std::endl
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

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(g_width * 2, g_height);
    glutInitWindowPosition(50, 50);
    glutCreateWindow(argv[0]);

    initializeGl();

    glutDisplayFunc(display); 
    glutKeyboardFunc(keyboard);
    glutVisibilityFunc(visible);
    glutMainLoop();

    return 0;
}

