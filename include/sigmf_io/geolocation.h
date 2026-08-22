#ifndef SIGMF_IO_GEOLOCATION_H
#define SIGMF_IO_GEOLOCATION_H
/*
 * Geolocation
 *
 * A class that acts as a convenient interface to SigMF Geolocation objects
 * (GeoJSON "Point" objects). Enforces the SigMF core specification
 * constraints on the core fields ("type", "coordinates": longitude,
 * latitude, optional altitude) while still allowing arbitrary additional
 * keys/values/nested structures via jsoncons.
 *
 * TODO: extend to allow for global's older version of geolocation...
 * TODO: update geolocation to have a jsoncons::json object constructor.
 * TODO: extend for alternative representaitons for geolocation (global).
 * TODO: update to inherit from JSONBase...
 */
#include <optional>
#include <string>

#include <jsoncons/json.hpp>

namespace sigmf_io {

// TODO: Implement to WGS84 GeoJSON specification.
class Geolocation
{
    // Geolocation public interface.
public:
    Geolocation() = default;
    //Geolocation(const jsoncons::json& data);
    Geolocation(double latitude, double longitude);
    Geolocation(double latitude, double longitude, double altitude);

    // core "type" field - always "Point" per SigMF/GeoJSON spec. Read-only.
    std::string get_type() const { return "Point"; }

    // core "coordinates" field accessors.
    double latitude() const { return latitude_; }
    double longitude() const { return longitude_; }
    std::optional<double> altitude() const { return altitude_; }

    // core "coordinates" field mutators - throw on out-of-spec values.
    void set_latitude(double latitude);
    void set_longitude(double longitude);
    void set_altitude(double altitude);
    void clear_altitude();

    // (de)serialization to/from a SigMF-compliant jsoncons::json object.
    jsoncons::json to_json() const;
    static Geolocation from_json(const jsoncons::json& j);

    // Arbitrary additional keys/values/complex structures that are not
    // part of the SigMF core spec for geolocation. These are merged in
    // alongside "type"/"coordinates" at serialization time, and any
    // unrecognized fields found by fromJson() are captured here.
    jsoncons::json extra = jsoncons::json::object();

    // Hidden helpers/member variables.
private:
    static void validate_latitude(double latitude);
    static void validate_longitude(double longitude);

    double latitude_{0.0};
    double longitude_{0.0};
    std::optional<double> altitude_;
};

} // end sigmf_io namespace

#endif // SIGMF_IO_GEOLOCATION_H
