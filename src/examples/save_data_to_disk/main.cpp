#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;
using namespace std;

/**
 * @brief Saves acquisition data from a JSON string to a CSV file.
 *
 * @param json_data The JSON string containing acquisition data.
 * @param csv_filename The name of the CSV file to save the data to.
 */
void save_acquisition_data_to_csv(const string& json_data,
                                  const string& csv_filename) {
  json data = json::parse(json_data);

  const auto& x_data = data["acquisition"][0]["roi"][0]["xData"];
  const auto& y_data = data["acquisition"][0]["roi"][0]["yData"][0];

  ofstream file(csv_filename);
  if (!file.is_open()) {
    cerr << "Error opening file!" << endl;
    return;
  }

  file << "xData,yData\n";

  for (size_t i = 0; i < x_data.size(); ++i) {
    file << x_data[i] << "," << y_data[i] << "\n";
  }

  file.close();
}

/**
 * @brief Saves SpectrAcq3 data to a CSV file.
 *
 * @param json_data The JSON data containing SpectrAcq3 data.
 * @param csv_filename The name of the CSV file to save the data to.
 */
void save_spectracq3_data_to_csv(const vector<json>& json_data,
                                 const string& csv_filename) {
  vector<string> headers = {"wavelength",
                            "elapsedTime",
                            "currentSignal_value",
                            "currentSignal_unit",
                            "pmtSignal_value",
                            "pmtSignal_unit",
                            "ppdSignal_value",
                            "ppdSignal_unit",
                            "voltageSignal_value",
                            "voltageSignal_unit",
                            "eventMarker",
                            "overscaleCurrentChannel",
                            "overscaleVoltageChannel",
                            "pointNumber"};

  ofstream file(csv_filename);
  if (!file.is_open()) {
    cerr << "Error opening file!" << endl;
    return;
  }

  for (const auto& header : headers) {
    file << header << ",";
  }
  // Remove last comma
  file.seekp(-1, ios_base::end);
  file << "\n";

  for (const auto& data : json_data) {
    file << data["wavelength"] << "," << data["elapsedTime"] << ","
         << data["currentSignal"]["value"] << ","
         << data["currentSignal"]["unit"] << "," << data["pmtSignal"]["value"]
         << "," << data["pmtSignal"]["unit"] << ","
         << data["ppdSignal"]["value"] << "," << data["ppdSignal"]["unit"]
         << "," << data["voltageSignal"]["value"] << ","
         << data["voltageSignal"]["unit"] << "," << data["eventMarker"] << ","
         << data["overscaleCurrentChannel"] << ","
         << data["overscaleVoltageChannel"] << "," << data["pointNumber"]
         << "\n";
  }

  file.close();
}

int main() {
  string json_data = R"({
        "acquisition": [{
            "acqIndex": 1,
            "roi": [{
                "roiIndex": 1,
                "xBinning": 1,
                "xData": [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15],
                "xOrigin": 0,
                "xSize": 16,
                "yBinning": 4,
                "yData": [[602, 600, 600, 598, 599, 598, 598, 598, 597, 597, 598, 599, 599, 597, 600, 597]],
                "yOrigin": 0,
                "ySize": 4
            }]
        }],
        "timestamp": "2025.03.24 10:04:51.838"
    })";

  save_acquisition_data_to_csv(json_data, "acquisition_data.csv");

  vector<json> spectracq3_data = {
      {{"wavelength", 650},
       {"elapsedTime", 1.23},
       {"currentSignal", {{"value", 0.5}, {"unit", "mA"}}},
       {"pmtSignal", {{"value", 1.1}, {"unit", "V"}}},
       {"ppdSignal", {{"value", 2.0}, {"unit", "nA"}}},
       {"voltageSignal", {{"value", 0.75}, {"unit", "V"}}},
       {"eventMarker", "A"},
       {"overscaleCurrentChannel", false},
       {"overscaleVoltageChannel", false},
       {"pointNumber", 1}},
  };

  save_spectracq3_data_to_csv(spectracq3_data, "spectracq3_data.csv");

  return 0;
}
