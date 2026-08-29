#include "sigmf_io/recording.h"
#include "sigmf_io/dataset.h"
#include "sigmf_io/datatype.h"
#include <jsoncons/json.hpp>
#include <string>
#include <fstream>

namespace sigmf_io {

// Recording::Recording(const std::string& file_path)
// {
//     std::string data_path;
//     if (this->ends_with(file_path, this->META_EXT)) {
//         // --- .sigmf-meta case ---
//         this->meta_path_ = file_path;

//         std::ifstream is(this->meta_path_);
//         if (!is.is_open()) {
//             throw std::runtime_error("Failed to open: " + this->meta_path_);
//         }
//         this->meta_ = jsoncons::json::parse(is);

//         bool has_dataset_field =
//             this->meta_.contains("global") &&
//             this->meta_["global"].contains("core:dataset");

//         if (!has_dataset_field) {
//             // No core:dataset field -> compliant recording, same base name, .sigmf-data extension.
//             data_path = this->meta_path_;
//             std::string::size_type pos = data_path.rfind(this->META_EXT);
//             data_path.replace(pos, this->META_EXT.size(), this->DATA_EXT);
//         } else {
//             // core:dataset present -> non-conforming dataset; filename only, same directory as .sigmf-meta.
//             std::string dataset_filename = this->meta_["global"]["core:dataset"].as<std::string>();

//             std::string::size_type slash_pos = this->meta_path_.rfind('/');
//             std::string dir = (slash_pos != std::string::npos)
//                                   ? this->meta_path_.substr(0, slash_pos + 1)
//                                   : "";

//             data_path = dir + dataset_filename;
//         }
//     }
//     else if (this->ends_with(file_path, DATA_EXT)) {
//         // --- .sigmf-data case ---
//         data_path = file_path;

//         this->meta_path_ = file_path;
//         std::string::size_type pos = this->meta_path_.rfind(DATA_EXT);
//         this->meta_path_.replace(pos, DATA_EXT.size(), META_EXT);

//         std::ifstream is(this->meta_path_);
//         if (!is.is_open()) {
//             throw std::runtime_error("Failed to open: " + this->meta_path_);
//         }
//         this->meta_ = jsoncons::json::parse(is);
//     }
//     else {
//         throw std::invalid_argument(
//             "file_path must end with .sigmf-meta or .sigmf-data: " + file_path);
//     }

//     // Initialize SigMFDataset object.
//     bool has_num_channels =
//         this->meta_.contains("global") &&
//         this->meta_["global"].contains("core:num_channels");
//     int64_t num_channels = 1;
//     if (has_num_channels) {
//         num_channels = this->meta_["global"]["core:num_channels"].as<int64_t>();
//     }

//     bool has_trailing_bytes =
//         this->meta_.contains("global") &&
//         this->meta_["global"].contains("core:trailing_bytes");
//     int64_t trailing_bytes = 0;
//     if (has_trailing_bytes) {
//         trailing_bytes = this->meta_["global"]["core:trailing_bytes"].as<int64_t>();
//     }

//     bool has_offset =
//         this->meta_.contains("global") &&
//         this->meta_["global"].contains("core:offset");
//     int64_t offset = 0;
//     if (has_offset) {
//         offset = this->meta_["global"]["core:offset"].as<int64_t>();
//     }

//     this->dataset.emplace(
//         data_path,
//         SigMFDataType(this->meta_["global"]["core:datatype"].as<std::string>()),
//         num_channels,
//         trailing_bytes,
//         offset
//     );
// }

} // end sigmf_io namespace
