#ifndef SIGMFGEOLOCATION_H
#define SIGMFGEOLOCATION_H
/*
 * SigMFGeoLocation
 *
 * A class that acts as a convenient interface to SigMF Geolocation objects
 * (GeoJSON "Point" objects). Enforces the SigMF core specification
 * constraints on the core fields ("type", "coordinates": longitude,
 * latitude, optional altitude) while still allowing arbitrary additional
 * keys/values/nested structures via jsoncons.
 *
 * TODO: extend to allow for global's older version of captures...
 */
#include <jsoncons/json.hpp>
#include <optional>
#include <string>

// TODO: Implement to WGS84 GeoJSON specification.
class SigMFGeoLocation
{
    // SigMFGeoLocation public interface.
public:
    SigMFGeoLocation() = default;
    SigMFGeoLocation(double latitude, double longitude);
    SigMFGeoLocation(double latitude, double longitude, double altitude);

    // core "type" field - always "Point" per SigMF/GeoJSON spec. Read-only.
    std::string getType() const { return "Point"; }

    // core "coordinates" field accessors.
    double getLatitude() const { return latitude_; }
    double getLongitude() const { return longitude_; }
    std::optional<double> getAltitude() const { return altitude_; }

    // core "coordinates" field mutators - throw on out-of-spec values.
    void setLatitude(double latitude);
    void setLongitude(double longitude);
    void setAltitude(double altitude);
    void clearAltitude();

    // (de)serialization to/from a SigMF-compliant jsoncons::json object.
    jsoncons::json toJson() const;
    static SigMFGeoLocation fromJson(const jsoncons::json& j);

    // Arbitrary additional keys/values/complex structures that are not
    // part of the SigMF core spec for geolocation. These are merged in
    // alongside "type"/"coordinates" at serialization time, and any
    // unrecognized fields found by fromJson() are captured here.
    jsoncons::json extra = jsoncons::json::object();

    // Hidden helpers/member variables.
private:
    static void validateLatitude(double latitude);
    static void validateLongitude(double longitude);

    double latitude_{0.0};
    double longitude_{0.0};
    std::optional<double> altitude_;
};
#endif // SIGMFGEOLOCATION_H
