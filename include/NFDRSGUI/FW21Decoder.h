#ifndef FW21DECODER_H
#define FW21DECODER_H

#include <cstddef>
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

    // copy constructor
    FW21Timeseries(FW21Timeseries& other) : NT(other.NT) {
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

        std::copy(other.date_time.begin(), other.date_time.end(),
                  date_time.begin());
        std::copy(other.air_temperature.begin(), other.air_temperature.end(),
                  air_temperature.begin());
        std::copy(other.relative_humidity.begin(),
                  other.relative_humidity.end(), relative_humidity.begin());
        std::copy(other.precipitation.begin(), other.precipitation.end(),
                  precipitation.begin());
        std::copy(other.wind_speed.begin(), other.wind_speed.end(),
                  wind_speed.begin());
        std::copy(other.wind_direction.begin(), other.wind_direction.end(),
                  wind_direction.begin());
        std::copy(other.solar_radiation.begin(), other.solar_radiation.end(),
                  solar_radiation.begin());
        std::copy(other.gust_speed.begin(), other.gust_speed.end(),
                  gust_speed.begin());
        std::copy(other.gust_direction.begin(), other.gust_direction.end(),
                  gust_direction.begin());
        std::copy(other.snow_flag.begin(), other.snow_flag.end(),
                  snow_flag.begin());
    }

    // move constructor
    FW21Timeseries(FW21Timeseries&& other) : NT(other.NT) {
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

    std::vector<int> date_time;
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
};

FW21Timeseries decode_fw21(std::string_view data_buffer);

}  // namespace fw21

#endif
