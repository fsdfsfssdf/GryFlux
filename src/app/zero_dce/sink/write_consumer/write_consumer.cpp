#include "write_consumer.h"

#include <chrono>
#include <filesystem>
#include <thread>

#include "package.h"

namespace GryFlux {
namespace ZeroDCE {

WriteConsumer::WriteConsumer(GryFlux::StreamingPipeline &pipeline,
                             std::atomic<bool> &running,
                             CPUAllocator *allocator,
                             std::string_view write_path)
    : GryFlux::DataConsumer(pipeline, running, allocator),
      processed_frames_(0) {
  if (!write_path.empty()) {
    std::filesystem::create_directories(write_path);
    output_path_ = write_path.data();
    LOG.info("[ZeroDCE::WriteConsumer] Output path set to: %s",
             write_path.data());
  } else {
    LOG.error("[ZeroDCE::WriteConsumer] Invalid output path");
  }
}

void WriteConsumer::run() {
  LOG.info("[ZeroDCE::WriteConsumer] Consumer started");

  while (shouldContinue()) {
    std::shared_ptr<DataObject> output;
    if (getData(output)) {
      auto result = std::dynamic_pointer_cast<ImagePackage>(output);
      if (!result) {
        continue;
      }

      ++processed_frames_;
      const cv::Mat &img = result->get_data();

      std::string base_name = result->get_filename();
      if (base_name.empty()) {
        base_name = "sr_output_" + std::to_string(processed_frames_) + ".png";
      }

      const std::filesystem::path output_file =
          std::filesystem::path(output_path_) / base_name;
      if (!cv::imwrite(output_file.string(), img)) {
        LOG.error("[ZeroDCE::WriteConsumer] Failed to write image: %s",
                  output_file.string().c_str());
        continue;
      }

      LOG.info("[ZeroDCE::WriteConsumer] Frame %d processed -> %s",
               processed_frames_, output_file.filename().string().c_str());
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  LOG.info("[ZeroDCE::WriteConsumer] Processed frames: %d", processed_frames_);
  LOG.info("[ZeroDCE::WriteConsumer] Consumer finished");
}

} // namespace ZeroDCE
} // namespace GryFlux
