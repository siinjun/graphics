/* Release code for program 1 CPE 471 Fall 2016 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Image.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;

int g_width, g_height;

void resize_obj(std::vector<tinyobj::shape_t> &shapes);

int main(int argc, char **argv)
{
//    if(argc < 3) {
//      cout << "Usage: Assignment1 meshfile imagefile" << endl;
//      return 0;
//   }
    // OBJ filename
//    string meshName(argv[1]);
//    string imgName(argv[2]);

    string meshName("resources/tri.obj");
    string imgName("result.png");
    //set g_width and g_height appropriately!
    g_width = g_height = 100;

   //create an image
    auto image = make_shared<Image>(g_width, g_height);

    // triangle buffers
    vector<unsigned int> triBuf;
    // position buffer
    vector<float> posBuf;
    // Some obj files contain material information.
    // We'll ignore them for this assignment.
    vector<tinyobj::shape_t> shapes; // geometry
    vector<tinyobj::material_t> objMaterials; // material
    string errStr;
    
   bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
    /* error checking on read */
    if(!rc) {
        cerr << errStr << endl;
    } else {
         //keep this code to resize your object to be within -1 -> 1
       resize_obj(shapes);
        posBuf = shapes[0].mesh.positions;
        triBuf = shapes[0].mesh.indices;
    }
    cout << "Number of vertices: " << posBuf.size()/3 << endl;
    cout << "Number of triangles: " << triBuf.size()/3 << endl;

    // get coordinates of triangle
    int x1 = (posBuf[0] + 1) * 50;
    int y1 = (posBuf[1] + 1) * 50;
    int x2 = (posBuf[3] + 1) * 50;
    int y2 = (posBuf[4] + 1) * 50;
    int x3 = (posBuf[6] + 1) * 50;
    int y3 = (posBuf[7] + 1) * 50;
    if (x1 == g_width) {
        x1 = g_width - 1;
    }
    if (x2 == g_width) {
        x2 = g_width - 1;
    }
    if (x3 == g_width) {
        x3 = g_width - 1;
    }
    if (y1 == g_height) {
        y1 = g_height - 1;
    }
    if (y2 == g_height) {
        y2 = g_height - 1;
    }
    if (y3 == g_height) {
        y3 = g_height - 1;
    }
    //TODO add code to iterate through each triangle and rasterize it
    for (int j = 0; j < 100; j++) {
        for (int i = 0; i < 100; i++) {
            float beta = (float) ((x1 - x3) * (j - y3) - (i - x3) * (y1 - y3)) / ((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1));
            float gamma = (float) ((x2 - x1) * (j - y1)) / ((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1));
            float alpha = 1 - beta - gamma;

            // if in the triangle
            if ((beta >= 0 && beta <= 1) && (alpha >= 0 && alpha <= 1) && (gamma >= 0 && gamma <= 1)) {
                int red = (int) 255 * alpha;
                int green  = (int) 255 * beta;
                int blue = (int) 255 * gamma;
                image->setPixel(i, j, red, green, blue);
            }
            // not in trianlge
            else
                image->setPixel(i, j, 0, 0, 0);
        }
    }
    //write out the image
   image->writeToFile(imgName);

    return 0;
}

/*
   Helper function you will want all quarter
   Given a vector of shapes which has already been read from an obj file
   resize all vertices to the range [-1, 1]
 */
void resize_obj(std::vector<tinyobj::shape_t> &shapes){
   float minX, minY, minZ;
   float maxX, maxY, maxZ;
   float scaleX, scaleY, scaleZ;
   float shiftX, shiftY, shiftZ;
   float epsilon = 0.001;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   //Go through all vertices to determine min and max of each dimension
   for (size_t i = 0; i < shapes.size(); i++) {
      for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
         if(shapes[i].mesh.positions[3*v+0] < minX) minX = shapes[i].mesh.positions[3*v+0];
         if(shapes[i].mesh.positions[3*v+0] > maxX) maxX = shapes[i].mesh.positions[3*v+0];

         if(shapes[i].mesh.positions[3*v+1] < minY) minY = shapes[i].mesh.positions[3*v+1];
         if(shapes[i].mesh.positions[3*v+1] > maxY) maxY = shapes[i].mesh.positions[3*v+1];

         if(shapes[i].mesh.positions[3*v+2] < minZ) minZ = shapes[i].mesh.positions[3*v+2];
         if(shapes[i].mesh.positions[3*v+2] > maxZ) maxZ = shapes[i].mesh.positions[3*v+2];
      }
   }

    //From min and max compute necessary scale and shift for each dimension
   float maxExtent, xExtent, yExtent, zExtent;
   xExtent = maxX-minX;
   yExtent = maxY-minY;
   zExtent = maxZ-minZ;
   if (xExtent >= yExtent && xExtent >= zExtent) {
      maxExtent = xExtent;
   }
   if (yExtent >= xExtent && yExtent >= zExtent) {
      maxExtent = yExtent;
   }
   if (zExtent >= xExtent && zExtent >= yExtent) {
      maxExtent = zExtent;
   }
   scaleX = 2.0 /maxExtent;
   shiftX = minX + (xExtent/ 2.0);
   scaleY = 2.0 / maxExtent;
   shiftY = minY + (yExtent / 2.0);
   scaleZ = 2.0/ maxExtent;
   shiftZ = minZ + (zExtent)/2.0;

   //Go through all verticies shift and scale them
   for (size_t i = 0; i < shapes.size(); i++) {
      for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
         shapes[i].mesh.positions[3*v+0] = (shapes[i].mesh.positions[3*v+0] - shiftX) * scaleX;
         assert(shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
         shapes[i].mesh.positions[3*v+1] = (shapes[i].mesh.positions[3*v+1] - shiftY) * scaleY;
         assert(shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
         shapes[i].mesh.positions[3*v+2] = (shapes[i].mesh.positions[3*v+2] - shiftZ) * scaleZ;
         assert(shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
      }
   }
}
