#ifndef FW21DECODER_H
#define FW21DECODER_H

#include <cstddef>
#include <ctime>
#include <string_view>
#include <vector>

namespace fw21 {

struct FW21Timeseries {
    // constructor
    FW21Timeseries(std::ptrdiff_t NTIMES) : NT(NTIMES) {
        date_time.reserve(NT);
        air_temperature.reserve(NT);
        relative_humidity.reserve(NT);
        precipitation.reserve(NT);
        wind_speed.reserve(NT);
        wind_direction.reserve(NT);
        solar_radiation.reserve(NT);
        gust_speed.reserve(NT);
        gust_direction.reserve(NT);
        snow_flag.reserve(NT);
    }

    // move constructor
    FW21Timeseries(FW21Timeseries&& other) noexcept : NT(other.NT) {
        date_time = std::move(other.date_time);
        air_temperature = std::move(other.air_temperature);
        relative_humidity = std::move(other.relative_humidity);
        precipitation = std::move(other.precipitation);
        wind_speed = std::move(other.wind_speed);
        wind_direction = std::move(other.wind_direction);
        solar_radiation = std::move(other.solar_radiation);
        gust_speed = std::move(other.gust_speed);
        gust_direction = std::move(other.gust_direction);
        snow_flag = std::move(other.snow_flag);
    }

    std::vector<double> date_time;
    std::vector<double> air_temperature;
    std::vector<double> relative_humidity;
    std::vector<double> precipitation;
    std::vector<double> wind_speed;
    std::vector<double> wind_direction;
    std::vector<double> solar_radiation;
    std::vector<double> gust_speed;
    std::vector<double> gust_direction;
    std::vector<int> snow_flag;

    const std::ptrdiff_t NT;
    static FW21Timeseries decode_fw21(std::string_view data_buffer);
};

}  // namespace fw21

#endif
