#include <horiba_cpp_sdk/communication/command.h>
#include <horiba_cpp_sdk/devices/single_devices/ccd.h>
#include <spdlog/spdlog.h>

namespace horiba::devices::single_devices {

using namespace nlohmann;

ChargeCoupledDevice::ChargeCoupledDevice(
    int id, std::shared_ptr<communication::Communicator> communicator)
    : Device(id, communicator) {}

void ChargeCoupledDevice::open() {
  Device::open();
  auto _ignored_response = Device::execute_command(
      communication::Command("ccd_open", {{"index", Device::device_id()}}));
}

void ChargeCoupledDevice::close() {
  auto _ignored_response = Device::execute_command(
      communication::Command("ccd_close", {{"index", Device::device_id()}}));
}

bool ChargeCoupledDevice::is_open() {
  auto response = Device::execute_command(
      communication::Command("ccd_isOpen", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  return json_results.at("open").get<bool>();
}

void ChargeCoupledDevice::restart() {
  auto _ignored_response = Device::execute_command(
      communication::Command("ccd_restart", {{"index", Device::device_id()}}));
}

nlohmann::json ChargeCoupledDevice::get_configuration() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getConfig", {{"index", Device::device_id()}}));
  auto results = response.json_results();
  return results["configuration"];
}

int ChargeCoupledDevice::get_gain_token() {
  auto response = Device::execute_command(
      communication::Command("ccd_getGain", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto gain = json_results.at("token").get<int>();
  return gain;
}

void ChargeCoupledDevice::set_gain(int gain_token) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_setGain", {{"index", Device::device_id()}, {"token", gain_token}}));
}

int ChargeCoupledDevice::get_speed_token() {
  auto response = Device::execute_command(
      communication::Command("ccd_getSpeed", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto speed = json_results.at("token").get<int>();
  return speed;
}

void ChargeCoupledDevice::set_speed(int speed_token) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_setSpeed",
      {{"index", Device::device_id()}, {"token", speed_token}}));
}

int ChargeCoupledDevice::get_parallel_speed_token() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getParallelSpeed", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto speed = json_results.at("token").get<int>();
  return speed;
}

void ChargeCoupledDevice::set_parallel_speed(int parallel_speed_token) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_setParallelSpeed",
      {{"index", Device::device_id()}, {"token", parallel_speed_token}}));
}

std::vector<int> ChargeCoupledDevice::get_fit_parameters() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getFitParams", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto fit_params = json_results.at("fitParameters").get<std::vector<int>>();
  return fit_params;
}

ChargeCoupledDevice::TimerResolution
ChargeCoupledDevice::get_timer_resolution() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getTimerResolution", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto timer_resolution = json_results.at("resolutionToken").get<int>();
  return static_cast<ChargeCoupledDevice::TimerResolution>(timer_resolution);
}

void ChargeCoupledDevice::set_timer_resolution(
    ChargeCoupledDevice::TimerResolution timer_resolution) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_setTimerResolution",
      {{"index", Device::device_id()}, {"resolutionToken", timer_resolution}

      }));
}

void ChargeCoupledDevice::set_acquisition_format(
    int number_of_rois, AcquisitionFormat acquisition_format) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_setAcqFormat", {{"index", Device::device_id()},
                           {"format", static_cast<int>(acquisition_format)},
                           {"numberOfRois", number_of_rois}}));
}

ChargeCoupledDevice::XAxisConversionType
ChargeCoupledDevice::get_x_axis_conversion_type() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getXAxisConversionType", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto x_axis_conversion_type = json_results.at("type").get<int>();

  return static_cast<ChargeCoupledDevice::XAxisConversionType>(
      x_axis_conversion_type);
}

void ChargeCoupledDevice::set_x_axis_conversion_type(
    ChargeCoupledDevice::XAxisConversionType conversion_type) {
  auto _ignored_response = Device::execute_command(
      communication::Command("ccd_setXAxisConversionType",
                             {{"index", Device::device_id()},
                              {"type", static_cast<int>(conversion_type)}}));
}

int ChargeCoupledDevice::get_acquisition_count() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getAcqCount", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto acquisition_count = json_results.at("count").get<int>();

  return acquisition_count;
}

void ChargeCoupledDevice::set_acquisition_count(int count) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_setAcqCount", {{"index", Device::device_id()}, {"count", count}}));
}

std::pair<int, ChargeCoupledDevice::CleanCountMode>
ChargeCoupledDevice::get_clean_count() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getCleanCount", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto acquisition_count = json_results.at("count").get<int>();
  auto acquisition_mode = static_cast<ChargeCoupledDevice::CleanCountMode>(
      json_results.at("mode").get<int>());

  return {acquisition_count, acquisition_mode};
}
void ChargeCoupledDevice::set_clean_count(
    int count, ChargeCoupledDevice::CleanCountMode mode) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_setCleanCount", {{"index", Device::device_id()},
                            {"count", count},
                            {"mode", static_cast<int>(mode)}}));
}

int ChargeCoupledDevice::get_acquisition_data_size() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getDataSize", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto data_size = json_results.at("size").get<int>();

  return data_size;
}

double ChargeCoupledDevice::get_temperature() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getChipTemperature", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto temperature = json_results.at("temperature").get<double>();

  return temperature;
}

std::pair<int, int> ChargeCoupledDevice::get_chip_size() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getChipSize", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto x = json_results.at("x").get<int>();
  auto y = json_results.at("y").get<int>();

  return {x, y};
}

int ChargeCoupledDevice::get_exposure_time() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getExposureTime", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto time = json_results.at("time").get<int>();

  return time;
}

void ChargeCoupledDevice::set_exposure_time(int exposure_time_ms) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_setExposureTime",
      {{"index", Device::device_id()}, {"time", exposure_time_ms}}));
}

int ChargeCoupledDevice::get_em_gain() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getEMGain", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto gain = json_results.at("gain").get<int>();

  return gain;
}

void ChargeCoupledDevice::set_em_gain(int em_gain) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_setEMGain", {{"index", Device::device_id()}, {"gain", em_gain}}));
}

std::tuple<bool, int, int, int> ChargeCoupledDevice::get_trigger_input() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getTriggerIn", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto address = json_results.at("address").get<int>();
  auto event = json_results.at("event").get<int>();
  auto signal_type = json_results.at("signalType").get<int>();
  auto enabled = (address > -1 && event > -1 && signal_type > -1);

  return {enabled, address, event, signal_type};
}

void ChargeCoupledDevice::set_trigger_input(bool enabled, int address,
                                            int event, int signal_type) {
  if (!enabled) {
    address = -1;
    event = -1;
    signal_type = -1;
    auto _ignored_response = Device::execute_command(communication::Command(
        "ccd_setTriggerIn", {{"index", Device::device_id()},
                             {"enable", enabled},
                             {"address", address},
                             {"event", event},
                             {"signalType", signal_type}}));
    return;
  }

  auto config = get_configuration();
  auto it_triggers = config.find("triggers");
  if (it_triggers == config.end()) {
    throw std::runtime_error("Triggers not found in the configuration");
  }

  const auto& triggers = it_triggers;
  auto found_triggers = std::find_if(triggers->begin(), triggers->end(),
                                     [&address](const nlohmann::json& trigger) {
                                       return trigger["token"] == address;
                                     });

  if (found_triggers == triggers->end()) {
    throw std::runtime_error("Trigger address " + std::to_string(address) +
                             " not found in the configuration");
  }

  auto found_events = std::find_if(
      (*found_triggers)["events"].begin(), (*found_triggers)["events"].end(),
      [&event](const nlohmann::json& trigger_event) {
        return trigger_event["token"] == event;
      });

  if (found_events == (*found_triggers)["events"].end()) {
    throw std::runtime_error("Trigger event " + std::to_string(event) +
                             " not found in the configuration");
  }

  auto found_signal_types = std::find_if(
      (*found_events)["types"].begin(), (*found_events)["types"].end(),
      [&signal_type](const nlohmann::json& signal) {
        return signal["token"] == signal_type;
      });

  if (found_signal_types == (*found_events)["types"].end()) {
    throw std::runtime_error("Trigger type " + std::to_string(signal_type) +
                             " not found in the configuration");
  }

  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_setTriggerIn", {{"index", Device::device_id()},
                           {"enable", enabled},
                           {"address", address},
                           {"event", event},
                           {"signalType", signal_type}}));
}

std::tuple<bool, int, int, int> ChargeCoupledDevice::get_signal_output() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getSignalOut", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto address = json_results.at("address").get<int>();
  auto event = json_results.at("event").get<int>();
  auto signal_type = json_results.at("signalType").get<int>();
  auto enabled = (address > -1 && event > -1 && signal_type > -1);

  return {enabled, address, event, signal_type};
}

void ChargeCoupledDevice::set_signal_output(bool enabled, int address,
                                            int event, int signal_type) {
  if (!enabled) {
    address = -1;
    event = -1;
    signal_type = -1;
    auto _ignored_response = Device::execute_command(communication::Command(
        "ccd_setSignalOut", {{"index", Device::device_id()},
                             {"enable", enabled},
                             {"address", address},
                             {"event", event},
                             {"signalType", signal_type}}));
    return;
  }

  auto config = get_configuration();
  auto it_signals = config.find("signals");
  if (it_signals == config.end()) {
    throw std::runtime_error("Signals not found in the configuration");
  }

  const auto& signals = it_signals;
  auto found_triggers = std::find_if(signals->begin(), signals->end(),
                                     [&address](const nlohmann::json& trigger) {
                                       return trigger["token"] == address;
                                     });

  if (found_triggers == signals->end()) {
    throw std::runtime_error("Signal address " + std::to_string(address) +
                             " not found in the configuration");
  }

  auto found_events = std::find_if(
      (*found_triggers)["events"].begin(), (*found_triggers)["events"].end(),
      [&event](const nlohmann::json& trigger_event) {
        return trigger_event["token"] == event;
      });

  if (found_events == (*found_triggers)["events"].end()) {
    throw std::runtime_error("Signal event " + std::to_string(event) +
                             " not found in the configuration");
  }

  auto found_signal_types = std::find_if(
      (*found_events)["types"].begin(), (*found_events)["types"].end(),
      [&signal_type](const nlohmann::json& signal) {
        return signal["token"] == signal_type;
      });

  if (found_signal_types == (*found_events)["types"].end()) {
    throw std::runtime_error("Signal type " + std::to_string(signal_type) +
                             " not found in the configuration");
  }

  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_setSignalOut", {{"index", Device::device_id()},
                           {"enable", enabled},
                           {"address", address},
                           {"event", event},
                           {"signalType", signal_type}}));
}

bool ChargeCoupledDevice::get_acquisition_ready() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getAcquisitionReady", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto ready = json_results.at("ready").get<bool>();

  return ready;
}

void ChargeCoupledDevice::set_acquisition_start(bool open_shutter) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_acquisitionStart",
      {{"index", Device::device_id()}, {"openShutter", open_shutter}}));
}

void ChargeCoupledDevice::set_region_of_interest(int roi_index, int x_origin,
                                                 int y_origin, int x_size,
                                                 int y_size, int x_bin,
                                                 int y_bin) {
  auto _ignored_response = Device::execute_command(
      communication::Command("ccd_setRoi", {{"index", Device::device_id()},
                                            {"roiIndex", roi_index},
                                            {"xOrigin", x_origin},
                                            {"yOrigin", y_origin},
                                            {"xSize", x_size},
                                            {"ySize", y_size},
                                            {"xBin", x_bin},
                                            {"yBin", y_bin}}));
}

std::any ChargeCoupledDevice::get_acquisition_data() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getAcquisitionData", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  return json_results.at("acquisition");
}

bool ChargeCoupledDevice::get_acquisition_busy() {
  auto response = Device::execute_command(communication::Command(
      "ccd_getAcquisitionBusy", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto ready = json_results.at("isBusy").get<bool>();

  return ready;
}

void ChargeCoupledDevice::abort_acquisition() {
  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_acquisitionAbort", {{"index", Device::device_id()}}));
}

void ChargeCoupledDevice::set_center_wavelength(int monochromator_id,
                                                double wavelength) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "ccd_setCenterWavelength", {{"index", Device::device_id()},
                                  {"monoIndex", monochromator_id},
                                  {"wavelength", wavelength}}));
}

std::vector<double> ChargeCoupledDevice::range_mode_center_wavelenghts(
    int monochromator_id, double start_wavelength, double end_wavelength,
    double pixel_overlap) {
  auto response = Device::execute_command(communication::Command(
      "ccd_calculateRangeModePositions", {{"index", Device::device_id()},
                                          {"monoIndex", monochromator_id},
                                          {"start", start_wavelength},
                                          {"end", end_wavelength},
                                          {"overlap", pixel_overlap}}));
  auto json_results = response.json_results();
  auto wavelengths =
      json_results.at("centerWavelengths").get<std::vector<double>>();

  return wavelengths;
}
} /* namespace horiba::devices::single_devices */
