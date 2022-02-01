#include "std_compat/memory.h"
#include "libpressio_ext/cpp/compressor.h"
#include "libpressio_ext/cpp/data.h"
#include "libpressio_ext/cpp/options.h"
#include "libpressio_ext/cpp/pressio.h"
#include "roibin_impl.h"

namespace libpressio { namespace binning_ns {

class binning_compressor_plugin : public libpressio_compressor_plugin {
public:
  struct pressio_options get_options_impl() const override
  {
    struct pressio_options options;
    return options;
  }

  struct pressio_options get_configuration_impl() const override
  {
    struct pressio_options options;
    set(options, "pressio:thread_safe", static_cast<int32_t>(pressio_thread_safety_multiple));
    set(options, "pressio:stability", "experimental");
    return options;
  }

  struct pressio_options get_documentation_impl() const override
  {
    struct pressio_options options;
    set(options, "pressio:description", R"(preforms a binning operation on the input data on compression and extrapolates on decompression)");
    return options;
  }


  int set_options_impl(struct pressio_options const& options) override
  {
    (void)options;
    return 0;
  }

  int compress_impl(const pressio_data* input,
                    struct pressio_data* output) override
  {
    try {
    switch(input->dtype()) {
      case pressio_float_dtype:
        *output = compress_typed<float>(*input);
        break;
      case pressio_double_dtype:
        *output = compress_typed<double>(*input);
        break;
      case pressio_int8_dtype:
        *output = compress_typed<int8_t>(*input);
        break;
      case pressio_int16_dtype:
        *output = compress_typed<int16_t>(*input);
        break;
      case pressio_int32_dtype:
        *output = compress_typed<int32_t>(*input);
        break;
      case pressio_int64_dtype:
        *output = compress_typed<int64_t>(*input);
        break;
      case pressio_uint8_dtype:
        *output = compress_typed<uint8_t>(*input);
        break;
      case pressio_uint16_dtype:
        *output = compress_typed<uint16_t>(*input);
        break;
      case pressio_uint32_dtype:
        *output = compress_typed<uint32_t>(*input);
        break;
      case pressio_uint64_dtype:
        *output = compress_typed<uint64_t>(*input);
        break;
      default:
        throw std::runtime_error("unsupported type");
    }
    return 0;
    } catch(std::exception const& ex) {
      return set_error(1, ex.what());
    }
  }

private:
  template <class T>
  pressio_data compress_typed( pressio_data const& input_data) {
    roibin_ns::indexer<4> id{
      input_data.get_dimension(0),
      input_data.get_dimension(1),
      input_data.get_dimension(2),
      input_data.get_dimension(3)
    };
    auto bins_v = this->bins.to_vector<size_t>();
    roibin_ns::indexer<4> bins{bins_v.begin(), bins_v.end()};
    roibin_ns::indexer<4> binned_storage = roibin_ns::to_binned_index(id, bins);

    pressio_data binned = pressio_data::owning(
        input_data.dtype(),
        {binned_storage[0], binned_storage[1], binned_storage[2], binned_storage[3]}
        );

    roibin_ns::bin_omp(id, binned_storage, bins, static_cast<T const*>(input_data.data()), static_cast<T*>(binned.data()));
    return binned;
  }

public:

  int decompress_impl(const pressio_data* input,
                      struct pressio_data* output) override
  {
    try {
    switch(output->dtype()) {
      case pressio_float_dtype:
        decompress_typed<float>(*input, *output);
        break;
      case pressio_double_dtype:
        decompress_typed<double>(*input, *output);
        break;
      case pressio_int8_dtype:
        decompress_typed<int8_t>(*input, *output);
        break;
      case pressio_int16_dtype:
        decompress_typed<int16_t>(*input, *output);
        break;
      case pressio_int32_dtype:
        decompress_typed<int32_t>(*input, *output);
        break;
      case pressio_int64_dtype:
        decompress_typed<int64_t>(*input, *output);
        break;
      case pressio_uint8_dtype:
        decompress_typed<uint8_t>(*input, *output);
        break;
      case pressio_uint16_dtype:
        decompress_typed<uint16_t>(*input, *output);
        break;
      case pressio_uint32_dtype:
        decompress_typed<uint32_t>(*input, *output);
        break;
      case pressio_uint64_dtype:
        decompress_typed<uint64_t>(*input, *output);
        break;
      default:
        throw std::runtime_error("unsupported type");
    }
    return 0;
    } catch(std::exception const& ex) {
      return set_error(1, ex.what());
    }
  }
private:
  template <class T>
  void decompress_typed( pressio_data const& input_data, pressio_data& output_data) {
    roibin_ns::indexer<4> id{
      output_data.get_dimension(0),
      output_data.get_dimension(1),
      output_data.get_dimension(2),
      output_data.get_dimension(3)
    };
    auto bins_v = this->bins.to_vector<size_t>();
    roibin_ns::indexer<4> bins{bins_v.begin(), bins_v.end()};
    roibin_ns::indexer<4> binned_storage = roibin_ns::to_binned_index(id, bins);

    roibin_ns::restore_omp(id, binned_storage, bins, static_cast<T const*>(input_data.data()), static_cast<T*>(output_data.data()));
  }

public:
  int major_version() const override { return 0; }
  int minor_version() const override { return 0; }
  int patch_version() const override { return 1; }
  const char* version() const override { return "0.0.1"; }
  const char* prefix() const override { return "binning"; }

  pressio_options get_metrics_results_impl() const override {
    return {};
  }

  std::shared_ptr<libpressio_compressor_plugin> clone() override
  {
    return compat::make_unique<binning_compressor_plugin>(*this);
  }

  pressio_data bins{2,2,1,1};
};

static pressio_register compressor_many_fields_plugin(compressor_plugins(), "binning", []() {
  return compat::make_unique<binning_compressor_plugin>();
});

} }
