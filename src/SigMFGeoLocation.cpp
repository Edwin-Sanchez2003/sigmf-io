#include "SigMFGeoLocation.h"
#include <cmath>
#include <stdexcept>

SigMFGeoLocation::SigMFGeoLocation(double latitude, double longitude)
{
    setLatitude(latitude);
    setLongitude(longitude);
}

SigMFGeoLocation::SigMFGeoLocation(double latitude, double longitude, double altitude)
{
    setLatitude(latitude);
    setLongitude(longitude);
    setAltitude(altitude);
}

// -90 to +90, per GeoJSON/WGS84 convention used by SigMF.
void SigMFGeoLocation::validateLatitude(double latitude)
{
    if (std::isnan(latitude) || (latitude < -90.0) || (latitude > 90.0))
        throw std::out_of_range(
            "SigMFGeoLocation: latitude must be in range [-90, 90], got: " +
            std::to_string(latitude));
}

// -180 to +180, per GeoJSON/WGS84 convention used by SigMF.
void SigMFGeoLocation::validateLongitude(double longitude)
{
    if (std::isnan(longitude) || (longitude < -180.0) || (longitude > 180.0))
        throw std::out_of_range(
            "SigMFGeoLocation: longitude must be in range [-180, 180], got: " +
            std::to_string(longitude));
}

void SigMFGeoLocation::setLatitude(double latitude)
{
    validateLatitude(latitude);
    this->latitude_ = latitude;
}

void SigMFGeoLocation::setLongitude(double longitude)
{
    validateLongitude(longitude);
    this->longitude_ = longitude;
}

void SigMFGeoLocation::setAltitude(double altitude)
{
    if (std::isnan(altitude))
        throw std::invalid_argument("SigMFGeoLocation: altitude must not be NaN");
    this->altitude_ = altitude;
}

void SigMFGeoLocation::clearAltitude()
{
    this->altitude_.reset();
}

jsoncons::json SigMFGeoLocation::toJson() const
{
    // Start from any custom/extra fields so the enforced core fields
    // always take precedence if there's ever a key collision.
    jsoncons::json j = this->extra;

    j["type"] = getType();

    jsoncons::json coords = jsoncons::json::array();
    coords.push_back(longitude_); // GeoJSON order is [lon, lat, (alt)]
    coords.push_back(latitude_);
    if (altitude_.has_value())
        coords.push_back(*altitude_);
    j["coordinates"] = coords;

    return j;
}

SigMFGeoLocation SigMFGeoLocation::fromJson(const jsoncons::json& j)
{
    if (!j.is_object())
        throw std::runtime_error("SigMFGeoLocation: expected a JSON object");

    if (!j.contains("type") || j["type"].as<std::string>() != "Point")
        throw std::runtime_error("SigMFGeoLocation: \"type\" must be \"Point\"");

    if (!j.contains("coordinates") || !j["coordinates"].is_array())
        throw std::runtime_error("SigMFGeoLocation: \"coordinates\" must be an array");

    const auto& coords = j["coordinates"];
    if (coords.size() < 2 || coords.size() > 3)
        throw std::runtime_error(
            "SigMFGeoLocation: \"coordinates\" must have 2 or 3 elements "
            "[longitude, latitude, (altitude)]");

    SigMFGeoLocation geo;
    geo.setLongitude(coords[0].as<double>());
    geo.setLatitude(coords[1].as<double>());
    if (coords.size() == 3)
        geo.setAltitude(coords[2].as<double>());

    // Preserve any additional/custom fields (everything except the two
    // core keys) so round-tripping doesn't lose user data.
    jsoncons::json extra = jsoncons::json::object();
    for (const auto& member : j.object_range())
    {
        if (member.key() != "type" && member.key() != "coordinates")
            extra[member.key()] = member.value();
    }
    geo.extra = extra;

    return geo;
}
