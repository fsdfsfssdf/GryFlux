#pragma once

#include <filesystem>
#include <memory>
#include <string_view>

#include "framework/data_producer.h"
#include "opencv2/opencv.hpp"
#include "utils/logger.h"
#include "utils/unified_allocator.h"


#include "package.h"

namespace GryFlux {
namespace ZeroDCE {

class ImageProducer : public GryFlux::DataProducer {
public:
  ImageProducer(GryFlux::StreamingPipeline &pipeline,
                std::atomic<bool> &running, CPUAllocator *allocator,
                std::string_view dataset_path,
                std::size_t max_frames = static_cast<std::size_t>(-1));
  ~ImageProducer();

protected:
  void run() override;

private:
  int frame_count_;
  std::size_t max_frames_;
  std::filesystem::path dataset_path_;
};

} // namespace ZeroDCE
} // namespace GryFlux
