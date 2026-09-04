#ifndef SIGMF_IO_METADATA_TRAITS_H
#define SIGMF_IO_METADATA_TRAITS_H

#include <jsoncons/json.hpp>
#include "sigmf_io/metadata.h"

namespace jsoncons { namespace reflect {

template <typename Json>
struct json_conv_traits<Json, sigmf_io::Metadata>
{
    using result_type = conversion_result<sigmf_io::Metadata>;

    static bool is(const Json& j) noexcept
    {
        // "global" is the one required top-level key; captures/annotations
        // default to empty when absent (per Metadata's own constructor).
        return j.is_object() && j.contains("global");
    }

    template <typename Alloc, typename TempAlloc>
    static result_type try_as(const allocator_set<Alloc, TempAlloc>&, const Json& j)
    {
        try
        {
            return result_type(sigmf_io::Metadata(j));
        }
        catch (...)
        {
            return result_type(jsoncons::unexpect, conv_errc::conversion_failed);
        }
    }

    template <typename Alloc, typename TempAlloc>
    static Json to_json(const allocator_set<Alloc, TempAlloc>& aset, const sigmf_io::Metadata& val)
    {
        return jsoncons::make_obj_using_allocator<Json>(aset.get_allocator(), val.to_json());
    }
};

}} // namespace jsoncons::reflect

#endif // SIGMF_IO_METADATA_TRAITS_H
