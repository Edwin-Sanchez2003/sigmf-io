#ifndef SIGMF_IO_METADATA_H
#define SIGMF_IO_METADATA_H

#include <fstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <utility>

#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpointer/jsonpointer.hpp>

#include "sigmf_io/spec_validator_factory.h"    // default_spec_validator(), make_spec_validator()
#include "sigmf_io/validation_context.h"        // ValidationLevel, ValidationContext
#include "sigmf_io/spec_validator_base.h"
#include "sigmf_io/global.h"
#include "sigmf_io/capture.h"
#include "sigmf_io/annotation.h"

namespace sigmf_io {

class Metadata
{
public:
    Global global;
    static constexpr std::string META_EXT = ".sigmf-meta";
    static constexpr std::string DATA_EXT = ".sigmf-data";
public:
    Metadata(
        Global g = Global(),
        std::vector<Capture> caps = {},
        std::vector<Annotation> anns = {},
        ValidationContext validation_context = default_validation_context());
    explicit Metadata(const std::string& meta_path, ValidationContext validation_context = default_validation_context());
    explicit Metadata(const jsoncons::json& meta, ValidationContext validation_context = default_validation_context());

    std::string meta_path() const { return this->meta_path_; }
    std::string data_path() const;

    jsoncons::json to_json() const;

    void save(const std::string& file_path, bool overwrite = false);

    bool is_ncd() const; // checks if its a ncd using the metadata.

    // --- Read-only access. Always returned sorted ascending by sample_start. ---
    const std::vector<Capture>& captures() const { return this->captures_; }
    const std::vector<Annotation>& annotations() const { return this->annotations_; }

    const Capture& capture_at(std::size_t index) const;
    const Annotation& annotation_at(std::size_t index) const;

    // Inserts, accepting either an lvalue (copied) or rvalue (moved) argument.
    void add_capture(Capture capture);
    void add_annotation(Annotation annotation);

    // Removes the element at `index`. Throws std::out_of_range if index is invalid.
    void remove_capture(std::size_t index);
    void remove_annotation(std::size_t index);

    // Replaces the element at `index` with `capture`/`annotation`, re-inserting it
    // in sorted position. To edit in place: copy out via capture_at()/annotation_at(),
    // mutate the copy, then pass it back here.
    void update_capture(std::size_t index, Capture capture);
    void update_annotation(std::size_t index, Annotation annotation);

    // Returns the index of the capture containing sample_idx, throwing if none does.
    // Assumes this->captures is sorted ascending by sample_start (required by SigMF spec).
    int64_t find_capture_containing(int64_t sample_idx) const;

    // get all annotations that completely or partially overlap with the sample range: [sample_start, sample_stop)
    std::vector<Annotation> get_annotations_in_range(int64_t sample_start, int64_t sample_stop) const;
    // get all captures that completely or partially overlap with the sample range: [sample_start, sample_stop)
    std::vector<int64_t> get_captures_in_range(int64_t sample_start, int64_t sample_stop) const;

    void set_validation_context(ValidationContext validation_context);
    const ValidationContext& validation_context() const { return this->validation_context_; }

private:
    std::string meta_path_;
    ValidationContext validation_context_;
    std::vector<Capture> captures_;
    std::vector<Annotation> annotations_;

    // sets the validation context to global, captures, and annotations.
    void propagate_validation_context();

    static std::vector<Capture> sort_by_sample_start(std::vector<Capture> caps);
    static std::vector<Annotation> sort_by_sample_start(std::vector<Annotation> anns);

    // Inserts `item` into `container` (already sorted ascending by sample_start)
    // at the position that preserves sort order. Shared by add_capture/add_annotation
    // and by edit_capture/edit_annotation's re-insertion step.
    template <typename T>
    static void insert_sorted(std::vector<T>& container, T item);

    bool ends_with(const std::string& value, const std::string& ending) const;

    static jsoncons::json load_json(const std::string& meta_path);
};

template <typename T>
void Metadata::insert_sorted(std::vector<T>& container, T item)
{
    auto it = std::lower_bound(container.begin(), container.end(), item,
                               [](const T& a, const T& b) { return a.sample_start() < b.sample_start(); });
    container.insert(it, std::move(item));
}

} // end sigmf_io namespace

#include "sigmf_io/metadata_traits.h"

#endif // SIGMF_IO_METADATA_H
