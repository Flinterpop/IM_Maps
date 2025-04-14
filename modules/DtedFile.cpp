//==============================================================================
// DtedFile --
//
//     ---- Extracted for read.me file on a DTED cdrom.
//
//     B. DTED Cell Format
//
//  A Level 1 DTED cell is a single file containing the following
//  information obtained from the standard DMA DTED distribution
//  format with the following changes: VOL1, HDR1, EOF1, UTL1, and
//  hardware end of file marks are not included in the file.
//
//                                                     Starting Byte
//  (1) User Header Label (UHL: 80 bytes)                       1
//  (2) Data Set Identification Record (DSI: 648 bytes)        81
//  (3) Accuracy Record (ACC: 2700 bytes)*                    729
//  (4) Data Records (1201 records at 2414                   3429,5843,
//      bytes/record)**                                      8257,etc.
//
//  * To meet current distribution security requirements of producing
//  only unclassified CD DTED, when there are multiple accuracy
//  subregions they are left blank (bytes 58-2613 of the ACC record)
//  and the multiple accuracy outline flag is set to 10 (bytes 56-57
//  of the ACC record).
//
//  ** The number of records is a function of the latitude.  A count
//  of 1201 is for cells between latitudes S50 and N49 degrees.
//  There are no sparse cells.  Elevations are two-byte integers,
//  high order first, and negatives are signed magnitude.  Users may
//  have to switch the bytes and convert negatives to the complement
//  they use.  This can be done by putting the low order byte first,
//  then turning off bit 15 (the high order bit), then multiplying by
//  -1.  For positive numbers, only the bytes are switched.
//==============================================================================


#include <fstream>
#include <cstdio>
#include <cstring>
#include <iostream>

#include "DtedFile.hpp"


// DTED User Header Label (UHL) - 80 bytes
struct dtedUhlRecord
{
    char recognition_sentinel[3];       // "UHL"
    char fixed_by_standard[1];          // "1"
    char origin_longitude[8];           // SW corner DDDMMSSH
    char origin_latitude[8];            // SW corner DDDMMSSH
    char data_interval_longitude[4];    // tenths of seconds
    char data_interval_latitude[4];     // tenths of seconds
    char absolute_vertical_accuracy[4]; // meters
    char security_code[3];              // S: secret, C: confidential, U: unclassified, R: restricted
    char unique_ref_number[12];         // defined by producer, may be blank
    char number_longitude_lines[4];     // longitude lines in a full cell
    char number_latitude_lines[4];      // latitude lines in a full cell
    char multiple_accuracy[1];          // 0: single, 1: multiple
    char reserved_future[24];
};

// DTED DSI header - 648 bytes
struct dtedDsiRecord
{
    char stuff[648];
};

// DTED ACC header - 2700 bytes
struct dtedAccRecord
{
    char stuff[2700];
};

// DTED column record header
struct dtedColumnHeader
{
    unsigned char recognition_sentinel[1]; // 252(base 8) = 170
    char sequential_count[3];              // count of block within file, starting at 0
    char longitude_count[2];               // longitude index
    char latitude_count[2];                // latitude index
};
// DTED column record trailer
struct dtedColumnFooter
{
    unsigned char checksum[4];             // sum of bytes within block
};


//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
static const unsigned char  DATA_RECOGNITION_SENTINEL = 170; // 252 base 8
static const char* UHL_RECOGNITION_SENTINEL = "UHL";
static const char  UHL_FIXED_BY_STANDARD_BYTE = '1';


/*
void DtedFile::copyData(const DtedFile& org, const bool)
{
   BaseClass::copyData(org);

   verifyChecksum = org.verifyChecksum;
}
*/


int DtedFile::maxElevationForScale = 2000;
bool DtedFile::localScale = true;

GLuint DtedFile::DTEDtoTexture()
{
    float scale = 256.0f / maxElevationForScale;

    if (localScale)
    {
        int max = 0;
        int min = 10000;
        for (int i = 0; i < VIEW_TEX_SIZE; i++)
        {
            for (int j = 0; j < VIEW_TEX_SIZE; j++)
            {
                int val = columns[j][i];
                if (val > max) max = val;
                if (val < min) min = val;
            }
        }
        scale = 256.0f / max;
        printf("DTED: Min: %d  max: %d  scale: %f\r\n", min, max, scale);
    }

    //GLubyte image[VIEW_TEX_SIZE][VIEW_TEX_SIZE][3]; // RGB storage
    for (int i = 0; i < VIEW_TEX_SIZE; i++)
    {
        for (int j = 0; j < VIEW_TEX_SIZE; j++)
        {
            DtedImage[i][j][0] = (GLubyte)columns[j][VIEW_TEX_SIZE - i] * scale;
            DtedImage[i][j][1] = (GLubyte)columns[j][VIEW_TEX_SIZE - i] * scale;
            DtedImage[i][j][2] = (GLubyte)columns[j][VIEW_TEX_SIZE - i] * scale;
        }
    }

    glGenTextures(1, &DtedTexture);
    glBindTexture(GL_TEXTURE_2D, DtedTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, VIEW_TEX_SIZE, VIEW_TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, DtedImage);

    return DtedTexture;
}



//------------------------------------------------------------------------------
// Load a DTED cell into memory
//------------------------------------------------------------------------------
bool DtedFile::loadData(char * filename)
{
    std::ifstream in;
    in.open(filename, std::ios::binary);
    if (in.fail())
    {
        if (true) {
            std::cerr << "DtedFile::loadData() ERROR, could not open file: " << filename << std::endl;
        }
        return false;
    }

    // Read cell parameters from the DTED headers
    if (!readDtedHeaders(in))
    {
        //clearData();
        in.close();
        if (true) {
            std::cerr << "DtedFile::loadData() ERROR reading DTED headers in file: " << filename << std::endl;
        }
        return false;
    }

    // Read elevation data from the DTED file
    if (!readDtedData(in))
    {
        //clearData();
        in.close();
        if (true) {
            std::cerr << "DtedFile::loadData() ERROR reading data from file: " << filename << std::endl;
        }
        return false;
    }

    // Close the file
    in.close();
    return true;
}











//------------------------------------------------------------------------------
// Read basic information about this cell from the file headers
//------------------------------------------------------------------------------
bool DtedFile::readDtedHeaders(std::istream& in)
{
    // Read in the User Header Label (UHL) record
    dtedUhlRecord uhl;
    in.read(reinterpret_cast<char*>(&uhl), sizeof(uhl));
    if (in.fail() || in.gcount() < sizeof(uhl))
    {
        std::cerr << "DtedFile::readDtedHeaders: error reading UHL record." << std::endl;
        return false;
    }
    if (std::strncmp(uhl.recognition_sentinel, UHL_RECOGNITION_SENTINEL, sizeof(uhl.recognition_sentinel)) != 0)
    {
        std::cerr << "DtedFile::readDtedHeaders: invalid recognition sentinel in UHL record." << std::endl;
        return false;
    }
    if (uhl.fixed_by_standard[0] != UHL_FIXED_BY_STANDARD_BYTE)
    {
        std::cerr << "DtedFile::readDtedHeaders: invalid fixed_by_standard byte in UHL record." << std::endl;
        return false;
    }

    // Read in the Data Set Identification (DSI) record
    dtedDsiRecord dsi;
    in.read(reinterpret_cast<char*>(&dsi), sizeof(dsi));
    if (in.fail() || in.gcount() < sizeof(dsi))
    {
        std::cerr << "DtedFile::readDtedHeaders: error reading DSI record." << std::endl;
        return false;
    }

    // Read in the Accuracy Description (ACC) record
    dtedAccRecord acc;
    in.read(reinterpret_cast<char*>(&acc), sizeof(acc));
    if (in.fail() || in.gcount() < sizeof(acc))
    {
        std::cerr << "DtedFile::readDtedHeaders: error reading ACC record." << std::endl;
        return false;
    }

    // Extract the latitude and longitude of cell's SW corner.
    // DTED cells are always 1 degree by 1 degree,
    // and always start on an integer degree.
    int swcLatitude(0);
    int swcLongitude(0);
    sscanf_s(uhl.origin_latitude, "%3d", &swcLatitude);
    sscanf_s(uhl.origin_longitude, "%3d", &swcLongitude);
    if (uhl.origin_latitude[7] == 'S') swcLatitude = -1 * swcLatitude;
    if (uhl.origin_longitude[7] == 'W') swcLongitude = -1 * swcLongitude;

    std::cout << "swcLatitude: " << swcLatitude << std::endl;
    std::cout << "swcLongitude: " << swcLongitude << std::endl;
    std::cout << "nwcLatitude: " << (swcLatitude + 1) << std::endl;
    std::cout << "necLatitude: " << (swcLongitude + 1) << std::endl;

    setLatitudeSW( swcLatitude );
    setLongitudeSW( swcLongitude );
    setLatitudeNE( swcLatitude + 1 );
    setLongitudeNE( swcLongitude + 1 );

    // Extract the data intervals for latitude and longitude
    static const double TENTHS_OF_SECONDS_PER_DEGREE = 36000.0;
    int latIncr(0);
    int lonIncr(0);
    sscanf_s(uhl.data_interval_latitude, "%4d", &latIncr);
    sscanf_s(uhl.data_interval_longitude, "%4d", &lonIncr);
    latSpacing = latIncr / TENTHS_OF_SECONDS_PER_DEGREE;
    lonSpacing = lonIncr / TENTHS_OF_SECONDS_PER_DEGREE;

    std::cout << "latSpacing : " << latSpacing << std::endl;
    std::cout << "lonSpacing : " << lonSpacing << std::endl;


    // Extract the number of latitude and longitude lines
    unsigned int num_lat(0);
    unsigned int num_lon(0);
    sscanf_s(uhl.number_latitude_lines, "%4u", &num_lat);
    sscanf_s(uhl.number_longitude_lines, "%4u", &num_lon);
    nptlat = num_lat;
    nptlong = num_lon;

    std::cout << "nptlat   " << nptlat << std::endl;
    std::cout << "nptlong : " << nptlong << std::endl;

    return true;
}




//------------------------------------------------------------------------------
// Read elevation data from DTED file
//------------------------------------------------------------------------------
bool DtedFile::readDtedData(std::istream& in)
{
    if (nptlat < 1 || nptlong < 1)
    {
        std::cerr << "DtedFile::readDtedData: DTED headers indicate an empty file." << std::endl;
        return false;
    }

    // Allocate the elevation array
    columns = new short* [nptlong];
    for (unsigned int i = 0; i < nptlong; i++)
    {
        columns[i] = new short[nptlat];
    }

    // Read the elevation array.
    for (unsigned int lon = 0; lon < nptlong; lon++)
    {
        unsigned long checksum = 0;

        // read record header
        dtedColumnHeader head;
        in.read(reinterpret_cast<char*>(&head), sizeof(head));
        if (in.fail() || in.gcount() < sizeof(head))
        {
            std::cerr << "DtedFile::readDtedData: error reading column header." << std::endl;
            return false;
        }
        for (unsigned int i = 0; i < sizeof(head); i++)
            checksum += (reinterpret_cast<unsigned char*>(&head))[i];

        if (head.recognition_sentinel[0] != DATA_RECOGNITION_SENTINEL)
        {
            std::cerr << "DtedFile::readDtedData: record contains invalid recognition sentinel." << std::endl;
            return false;
        }

        // Read elevation values for record
        double minElev0 = 99999.0;
        double maxElev0 = 0.0;
        for (unsigned int lat = 0; lat < nptlat; lat++)
        {
            unsigned char values[2];
            in.read(reinterpret_cast<char*>(values), sizeof(values));
            if (in.fail() || in.gcount() < sizeof(values))
            {
                std::cerr << "DtedFile::readDtedData: error reading data value." << std::endl;
                return false;
            }
            checksum += values[0] + values[1];

            short height = readValue(values[0], values[1]);
            columns[lon][lat] = height;

            // check if this is the new min or max elevation
            if (height < minElev0) minElev0 = height;
            if (height > maxElev0) maxElev0 = height;
        }
        //setMinElevation(minElev0);
        //setMaxElevation(maxElev0);

        // Read data record footer and verify checksum
        dtedColumnFooter foot;
        in.read(reinterpret_cast<char*>(&foot), sizeof(foot));
        if (in.fail() || in.gcount() < sizeof(foot))
        {
            std::cerr << "DtedFile::readDtedData: error reading column footer." << std::endl;
            return false;
        }

        if (isVerifyChecksum()) {
            unsigned long file_cksum = readValue(foot.checksum[0], foot.checksum[1], foot.checksum[2], foot.checksum[3]);
            if (file_cksum != checksum)
            {
                std::cerr << "DtedFile::readDtedData: bad checksum in data record." << std::endl;
                return false;
            }
        }
    }
    return true;
}




//------------------------------------------------------------------------------
// Interpret signed-magnitude values from DTED file
//------------------------------------------------------------------------------
short DtedFile::readValue(const unsigned char hbyte, const unsigned char lbyte)
{
    // The data is stored as 2 byte characters (sign and magnitude)
    // with high byte first.  The high bit is the sign bit.  Check for
    // sign bit and then turn it off and set SIGN_VAL accordingly.
    short height = 0;
    short sign_val = 1;
    unsigned char nhbyte = hbyte;

    if (hbyte & ~0177)
    {
        // sign bit set
        nhbyte = hbyte & 0177;
        sign_val = -1;
    }
    height = (256 * static_cast<short>(nhbyte) + static_cast<short>(lbyte)) * sign_val;

    return height;
}


long DtedFile::readValue(const unsigned char hbyte, const unsigned char byte1, const unsigned char byte2, const unsigned char lbyte)
{
    // The data is stored as 4 byte characters (sign and magnitude)
    // with high byte first.  The high bit is the sign bit.  Check for
    // sign bit and then turn it off and set SIGN_VAL accordingly.
    long height = 0;
    int sign_val = 1;
    unsigned char nhbyte = hbyte;

    if (hbyte & ~0177)
    {
        // sign bit set
        nhbyte = hbyte & 0177;
        sign_val = -1;
    }
    height = (256 * 256 * 256 * static_cast<long>(nhbyte) + 256 * 256 * static_cast<long>(byte1)
        + 256 * static_cast<long>(byte2) + static_cast<long>(lbyte)) * sign_val;

    return height;
}


