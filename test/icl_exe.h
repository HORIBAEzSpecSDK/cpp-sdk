#ifndef ICL_EXE_H
#define ICL_EXE_H

#include <horiba_cpp_sdk/os/process.h>
#include <spdlog/spdlog.h>

#if _WIN32
#include <horiba_cpp_sdk/os/windows_process.h>

#include <chrono>
#include <thread>
#endif

namespace horiba::test {

/**
 * @brief Test fixture that will start and stop the icl.exe located at:
 * C:\Program Files\HORIBA Scientific\SDK\icl.exe
 */
class ICLExe {
 public:
  static const std::string ICL_EXE_PATH;
  static const std::string ICL_EXE_NAME;

  ICLExe() { spdlog::debug("[ICLExe] ICLExe"); }

  ~ICLExe() { spdlog::debug("[ICLExe] ~ICLExe"); }

  void start() {
#if _WIN32
    spdlog::debug("[ICLExe] starting {}...", ICL_EXE_NAME);
    this->icl_process = std::make_shared<horiba::os::WindowsProcess>(
        ICL_EXE_PATH, ICL_EXE_NAME);
    this->icl_process->start();
#else
    spdlog::debug("[ICLExe] On a Unix platform, skip starting {}",
                  ICL_EXE_NAME);
#endif
  }

  void stop() {
#if _WIN32
    this->icl_process->stop();
    while (this->icl_process->running()) {
      spdlog::debug("[ICLExe] Waiting for {} to stop", ICL_EXE_NAME);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    spdlog::debug("[ICLExe] {} stopped", ICL_EXE_NAME);
#else
    spdlog::debug("[ICLExe] On a Unix platform, skip stopping {}",
                  ICL_EXE_NAME);
#endif
  }

 private:
  std::shared_ptr<horiba::os::Process> icl_process{nullptr};
};

inline const std::string ICLExe::ICL_EXE_PATH =
    R"(C:\Program Files\HORIBA Scientific\SDK\)";
inline const std::string ICLExe::ICL_EXE_NAME = R"(icl.exe)";

}  // namespace horiba::test
#endif /* ifndef ICLExe */
