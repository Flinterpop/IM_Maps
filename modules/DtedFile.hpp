
#ifndef __oe_terrain_DtedFile_H__
#define __oe_terrain_DtedFile_H__

#include <fstream>
#include <cstdio>
#include <cstring>
#include <iostream>

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

//#include "DataFile.hpp"

//------------------------------------------------------------------------------
// Class: DtedFile
//
// Description: DTED data loader.
//
// Factory name: DtedFile
//------------------------------------------------------------------------------

class DtedFile
    {

    public:
        DtedFile() {};
        bool loadData(char* fname);
        GLuint DTEDtoTexture();


protected:
   bool isVerifyChecksum() const { return verifyChecksum; }

private:
public: 
    
    const static int VIEW_TEX_SIZE = 3600;
    static bool localScale;
    static int maxElevationForScale; 
    //DtedFile dt;
    GLuint DtedTexture;
    GLubyte DtedImage[VIEW_TEX_SIZE][VIEW_TEX_SIZE][3]; // RGB storage
    
    char* filename;

    // Interpret signed-magnitude values from DTED file
    static short readValue(const unsigned char hbyte, const unsigned char lbyte);
    static long readValue(const unsigned char hbyte, const unsigned char byte1, const unsigned char byte2, const unsigned char lbyte);

    // Read in cell parameters from DTED headers
    bool readDtedHeaders(std::istream& in);
    bool readDtedData(std::istream& in);

   bool verifyChecksum {true};    // verify the file checksum flag

   double neLat{}, neLon{};       // Northeast lat/lon (degs)
   double swLat{}, swLon{};       // Southwest lat/lon (degs)
   double minElev{};               // Minimum elevation (m)
   double maxElev{};               // Maximum elevation (m)


   short** columns{};           // Array of data columns (values in meters)
   double   latSpacing{};        // Spacing between latitude points (degs)
   double   lonSpacing{};        // Spacing between longitude points (degs)
   unsigned int nptlat{};        // Number of points in latitude (i.e., number of elevations per column)
   unsigned int nptlong{};       // Number of points in longitude (i.e., number of columns)
   short    voidValue{ -32767 };   // Value representing a void (missing) data point





   void SetScaleLocal()
   {
       localScale = !localScale;
       DTEDtoTexture();
   }

   int heightAt(double lat, double lng, int& i, int& j)
   {
       //printf("%f %f %f %f\r\n", swLat, neLat, swLon, swLon);
       if (lat < swLat) return -1;
       if (lat > neLat) return -1;
       if (lng < swLon) return -1;
       if (lng > neLon) return -1;
       int height = 0;
       double deltaLat = lat - swLat;
       double deltaLng = neLon - lng;
       int indexJ = VIEW_TEX_SIZE * deltaLat;
       int indexI = VIEW_TEX_SIZE - (VIEW_TEX_SIZE * deltaLng);
       height = columns[indexI][indexJ];
       //printf("Height at %7.4f %8.4f\r\nDelta Lat/Lng: %f %f  indices: %d %d is %d\r\n", lat, lng, deltaLat,deltaLng, indexI, indexJ,height);
       i = indexI;
       j = indexJ;
       return height;
   }



   // Southwest corner latitude of this database (degs)
   bool setLatitudeSW(const double v)
   {
       bool ok = false;
       if (v >= -90.0 && v <= 90.0) {
           swLat = v;
           ok = true;
       }
       return ok;
   }

   // Southwest corner longitude of this database (degs)
   bool setLongitudeSW(const double v)
   {
       bool ok = false;
       if (v >= -180.0 && v <= 180.0) {
           swLon = v;
           ok = true;
       }
       return ok;
   }

   // Northeast corner latitude of this database (degs)
   bool setLatitudeNE(const double v)
   {
       bool ok = false;
       if (v >= -90.0 && v <= 90.0) {
           neLat = v;
           ok = true;
       }
       return ok;
   }

   // Northeast corner longitude of this database (degs)
   bool setLongitudeNE(const double v)
   {
       bool ok = false;
       if (v >= -180.0 && v <= 180.0) {
           neLon = v;
           ok = true;
       }
       return ok;
   }

};



#endif
