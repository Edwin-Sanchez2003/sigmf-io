#ifndef SIGMF_IO_CAPTURE_TRAITS_H
#define SIGMF_IO_CAPTURE_TRAITS_H

#include <jsoncons/json.hpp>
#include "sigmf_io/capture.h"

namespace jsoncons {

template <typename Json>
struct json_type_traits<Json, sigmf_io::Capture>
{
    using allocator_type = typename Json::allocator_type;

    static bool is(const Json& j) noexcept
    {
        // Minimal shape check: an object containing the one required
        // SigMF capture field. Refine later if you need to disambiguate
        // from other object types (e.g. in a variant/any context).
        return j.is_object() && j.contains("core:sample_start");
    }

    static sigmf_io::Capture as(const Json& j)
    {
        return sigmf_io::Capture(j);
    }

    static Json to_json(const sigmf_io::Capture& val, const allocator_type& alloc = allocator_type())
    {
        return Json(val.to_json(), semantic_tag::none, alloc);
    }
};

} // namespace jsoncons

#endif // SIGMF_IO_CAPTURE_TRAITS_H
