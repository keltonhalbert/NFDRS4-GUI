#include <NFDRSGUI/FW21Decoder.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace fw21 {

std::time_t parse_datetime_to_unix_time(const std::string& datetime_str) {
    std::tm tm = {};
    std::istringstream ss(datetime_str);

    // Parse the datetime part (excluding the timezone)
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

    if (ss.fail()) {
        std::cerr << "Error parsing datetime string." << std::endl;
        return -1;
    }

    // Convert tm to time_t (UTC time)
    std::time_t utc_time = std::mktime(&tm);
    if (utc_time == -1) {
        std::cerr << "Error converting tm to time_t." << std::endl;
        return -1;
    }

    // Now parse the timezone offset (e.g., -06:00)
    int hours_offset = 0, minutes_offset = 0;
    char sign;
    std::string timezone_str =
        datetime_str.substr(19);  // Extract the timezone part

    std::istringstream timezone_stream(timezone_str);
    timezone_stream >> sign >> hours_offset;
    timezone_stream.ignore(1);  // Skip the colon
    timezone_stream >> minutes_offset;

    if (timezone_stream.fail()) {
        std::cerr << "Error parsing timezone offset." << std::endl;
        return -1;
    }

    // Adjust the time based on the timezone offset
    int offset_seconds = (hours_offset * 3600) + (minutes_offset * 60);
    if (sign == '-') {
        utc_time += offset_seconds;
    } else {
        utc_time -= offset_seconds;
    }

    return utc_time;
}

void parse_row(FW21Timeseries& ts_data, const std::string_view buffer,
               const char delimiter = ',') {
    std::ptrdiff_t row_start = 0;
    std::ptrdiff_t row_end = 0;
    std::ptrdiff_t row_idx = 0;
    std::ptrdiff_t item_size = 0;

    while ((row_end = buffer.find(delimiter, row_start)) !=
           std::string_view::npos) {
        std::string element(buffer.substr(row_start, row_end - row_start));
        size_t idx;
        switch (row_idx) {
            // Date-Time
            case 1: {
                // handle converting string to UNIX timestamp
                std::time_t unix_time = parse_datetime_to_unix_time(element);
                ts_data.date_time.push_back(static_cast<double>(unix_time));
                break;
            }

            // Temperature
            case 2: {
                double val =
                    (element != "") ? std::stod(element, &idx) : std::nan("");
                ts_data.air_temperature.push_back(val);
                break;
            }

            // Relative Humidity
            case 3: {
                double val =
                    (element != "") ? std::stod(element, &idx) : std::nan("");
                ts_data.relative_humidity.push_back(val);
                break;
            }

            // Precipitation
            case 4: {
                double val =
                    (element != "") ? std::stod(element, &idx) : std::nan("");
                ts_data.precipitation.push_back(val);
                break;
            }

            // Wind Speed
            case 5: {
                double val =
                    (element != "") ? std::stod(element, &idx) : std::nan("");
                ts_data.wind_speed.push_back(val);
                break;
            }

            // Wind Direction
            case 6: {
                double val =
                    (element != "") ? std::stod(element, &idx) : std::nan("");
                ts_data.wind_direction.push_back(val);
                break;
            }

            // Gust Speed
            case 7: {
                double val =
                    (element != "") ? std::stod(element, &idx) : std::nan("");
                ts_data.gust_speed.push_back(val);
                break;
            }

            // Gust Direction
            case 8: {
                double val =
                    (element != "") ? std::stod(element, &idx) : std::nan("");
                ts_data.gust_direction.push_back(val);
                break;
            }

            // Snow flag
            case 9: {
                int val = (element != "") ? std::stoi(element, &idx) : 0;
                ts_data.snow_flag.push_back(val);
                break;
            }

            // Solar Radiation
            case 10: {
                double val =
                    (element != "") ? std::stod(element, &idx) : std::nan("");
                ts_data.solar_radiation.push_back(val);
                break;
            }
        }
        row_start = row_end + 1;
        row_idx += 1;
    }
}

FW21Timeseries FW21Timeseries::decode_fw21(std::string_view data_buffer) {
    std::ptrdiff_t n_lines =
        std::count(data_buffer.begin(), data_buffer.end(), '\n');
    std::ptrdiff_t n_chars = data_buffer.size();
    FW21Timeseries ts_data = FW21Timeseries(n_lines);

    for (std::ptrdiff_t line_idx = 0; line_idx < n_lines; ++line_idx) {
        std::ptrdiff_t row_start = 0;
        std::ptrdiff_t row_end = data_buffer.find_first_of('\n');
        std::ptrdiff_t row_size = row_end - row_start;
        // we don't want to exceed the bounds of our
        // string_view array
        if (row_end < n_chars) {
            std::string_view row(data_buffer.substr(row_start, row_size));

            // We want to skip the header string field
            // and just parse the meteorological data
            if (line_idx > 0) {
                parse_row(ts_data, row);
            }
            // advance the view forward
            data_buffer.remove_prefix(row_size + 1);
        }
    }

    // parse the last row
    std::string_view row(data_buffer.substr(0));
    parse_row(ts_data, row);

    ts_data.calc_fire_cat();

    return ts_data;
}

void FW21Timeseries::calc_fire_cat() {
    for (std::ptrdiff_t idx = 0; idx < this->NT; ++idx) {
        double wspd = this->wind_speed[idx];
        double relh = this->relative_humidity[idx];
        double tair = this->air_temperature[idx];
        int spc_cat_val = 0;
        if ((wspd >= 15) && (relh <= 25) && (tair >= 45)) {
            spc_cat_val = 1;
        }
        if ((wspd >= 20) && (relh <= 20) && (tair >= 50)) {
            spc_cat_val = 2;
        }
        if ((wspd >= 30) && (relh <= 15) && (tair >= 60)) {
            spc_cat_val = 3;
        }

        this->spc_cat[idx] = spc_cat_val;
    }
}

}  // namespace fw21
