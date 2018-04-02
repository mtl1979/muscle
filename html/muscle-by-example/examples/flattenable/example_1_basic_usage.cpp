#include "system/SetupSystem.h"         // for CompleteSetupSystem
#include "util/MiscUtilityFunctions.h"  // for PrintHexBytes()
#include "util/String.h"

using namespace muscle;

static void PrintExampleDescription()
{
   printf("\n");
   printf("This program demonstrates the Flattenable interface by making up an\n");
   printf("example class (GPSCoordinate) that implements the Flattenable interface,\n");
   printf("and using that interface to convert a GPSCoordinate object into a flat\n");
   printf("array of uint8's, and then restoring it back from them.\n");
   printf("\n");
}

enum {
   TYPECODE_GPS_COORDINATE = 1196446531 // 'GPSC' (arbitrary value generated by muscle/test/calctypecode.cpp)
};

/** This class represents a GPS coordinate (latitude, longitude, altitude).  Its sole
  * purpose is to generate how to subclass the Flattenable interface.
  */
class GPSCoordinate : public Flattenable
{
public:
   GPSCoordinate() : _latitude(0.0f), _longitude(0.0f), _altitude(0.0f)
   {
      // empty
   }

   GPSCoordinate(float latitude, float longitude, float altitude)
      : _latitude(latitude), _longitude(longitude), _altitude(altitude)
   {
      // empty
   }

   void SetLatitude( float lat) {_latitude  = lat;}
   void SetLongitude(float lon) {_longitude = lon;}
   void SetAltitude( float alt) {_altitude  = alt;}

   float GetLatitude()  const {return _latitude;}
   float GetLongitude() const {return _longitude;}
   float GetAltitude()  const {return _altitude;}

   // Returns true because our FlattenedSize() method will always return
   // the same byte-count regardless of what our current state is.
   virtual bool IsFixedSize() const {return true;}

   virtual uint32 TypeCode() const {return TYPECODE_GPS_COORDINATE;}

   // Returns the number of bytes our Flatten() method will write out if called on this object
   virtual uint32 FlattenedSize() const
   {
      return sizeof(_latitude) + sizeof(_longitude) + sizeof(_altitude);
   }

   virtual void Flatten(uint8 *buffer) const
   {
      uint8 * writeTo = buffer;

      muscleCopyOut(writeTo, B_HOST_TO_LENDIAN_IFLOAT(_latitude));
      writeTo += sizeof(_latitude);

      muscleCopyOut(writeTo, B_HOST_TO_LENDIAN_IFLOAT(_longitude));
      writeTo += sizeof(_longitude);

      muscleCopyOut(writeTo, B_HOST_TO_LENDIAN_IFLOAT(_altitude));
      writeTo += sizeof(_altitude);
   }

   virtual status_t Unflatten(const uint8 *buffer, uint32 numBytes)
   {
      if (numBytes < FlattenedSize()) return B_ERROR;

      const uint8 * readFrom = buffer;
      _latitude  = B_LENDIAN_TO_HOST_IFLOAT(muscleCopyIn<uint32>(readFrom));
      readFrom += sizeof(_latitude);

      _longitude = B_LENDIAN_TO_HOST_IFLOAT(muscleCopyIn<uint32>(readFrom));
      readFrom += sizeof(_longitude);

      _altitude = B_LENDIAN_TO_HOST_IFLOAT(muscleCopyIn<uint32>(readFrom));
      readFrom += sizeof(_altitude);

      // success!
      return B_NO_ERROR;
   }

   // Returns our current state as a human-readable String
   String ToString() const
   {
      return String("LAT=%1 LONG=%2 ALT=%3").Arg(_latitude).Arg(_longitude).Arg(_altitude);
   }

private:
   float _latitude;
   float _longitude;
   float _altitude;
};

int main(int argc, char ** argv)
{
   CompleteSetupSystem css;

   PrintExampleDescription();

   GPSCoordinate gps(50.2390641f, -119.2671924f, 500.0f);
   printf("Original GPS is: %s\n", gps.ToString()());

   // now let's Flatten() the object into a flat-sized buffer
   uint8 tempBuf[128];  // 128 bytes ought to be enough for anyone
   gps.Flatten(tempBuf);

   printf("\n");
   printf("Flattened representation is:\n");
   PrintHexBytes(tempBuf, gps.FlattenedSize());
   printf("\n");

   // Now let's Unflatten() the data again, retrieving it back from the byte-buffer
   GPSCoordinate anotherGPS;
   if (anotherGPS.Unflatten(tempBuf, gps.FlattenedSize()) == B_NO_ERROR)
   {
      printf("Recovered from flat-buffer:  %s\n", anotherGPS.ToString()());
   }
   else printf("Error, Unflatten() failed!?\n");

   printf("\n");
   return 0;
}
