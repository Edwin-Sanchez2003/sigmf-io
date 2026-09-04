#ifndef SIGMF_IO_ANNOTATION_TRAITS_H
#define SIGMF_IO_ANNOTATION_TRAITS_H

#include <jsoncons/json.hpp>
#include "sigmf_io/annotation.h"

namespace jsoncons { namespace reflect {

template <typename Json>
struct json_conv_traits<Json, sigmf_io::Annotation>
{
    using result_type = conversion_result<sigmf_io::Annotation>;

    static bool is(const Json& j) noexcept
    {
        return j.is_object() && j.contains("core:sample_start");
    }

    template <typename Alloc, typename TempAlloc>
    static result_type try_as(const allocator_set<Alloc, TempAlloc>&, const Json& j)
    {
        try
        {
            return result_type(sigmf_io::Annotation(j));
        }
        catch (...)
        {
            return result_type(jsoncons::unexpect, conv_errc::conversion_failed);
        }
    }

    template <typename Alloc, typename TempAlloc>
    static Json to_json(const allocator_set<Alloc, TempAlloc>& aset, const sigmf_io::Annotation& val)
    {
        return jsoncons::make_obj_using_allocator<Json>(aset.get_allocator(), val.to_json());
    }
};

}} // namespace jsoncons::reflect

#endif // SIGMF_IO_ANNOTATION_TRAITS_H
