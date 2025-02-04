#ifndef METEOGRAM_H
#define METEOGRAM_H

#include <cstddef>
#include <memory>

namespace nfdrs {

using met_dbl_t = std::unique_ptr<double[]>;
using met_int_t = std::unique_ptr<int[]>;

struct Meteogram {
    const std::ptrdiff_t N;

    met_dbl_t m_timestamp;
    met_dbl_t m_relh;
    met_dbl_t m_tair;
    met_dbl_t m_wspd;
    met_dbl_t m_wdir;
    met_dbl_t m_gust;
    met_dbl_t m_rain;
    met_dbl_t m_pres;
    met_dbl_t m_srad;
    met_int_t m_firewx_cat;

    Meteogram(std::ptrdiff_t NVALS) : N(NVALS) {
        m_timestamp = std::make_unique<double[]>(NVALS);
        m_relh = std::make_unique<double[]>(NVALS);
        m_tair = std::make_unique<double[]>(NVALS);
        m_wspd = std::make_unique<double[]>(NVALS);
        m_wdir = std::make_unique<double[]>(NVALS);
        m_gust = std::make_unique<double[]>(NVALS);
        m_rain = std::make_unique<double[]>(NVALS);
        m_pres = std::make_unique<double[]>(NVALS);
        m_srad = std::make_unique<double[]>(NVALS);
        m_firewx_cat = std::make_unique<int[]>(NVALS);
    };

    Meteogram(const double timestamp[], const double relh[],
              const double tair[], const double wspd[], const double wdir[],
              const double gust[], const double rain[], const double pres[],
              const double srad[], std::ptrdiff_t NVALS)
        : N(NVALS) {
        m_timestamp = std::make_unique<double[]>(NVALS);
        m_relh = std::make_unique<double[]>(NVALS);
        m_tair = std::make_unique<double[]>(NVALS);
        m_wspd = std::make_unique<double[]>(NVALS);
        m_wdir = std::make_unique<double[]>(NVALS);
        m_gust = std::make_unique<double[]>(NVALS);
        m_rain = std::make_unique<double[]>(NVALS);
        m_pres = std::make_unique<double[]>(NVALS);
        m_srad = std::make_unique<double[]>(NVALS);
        m_firewx_cat = std::make_unique<int[]>(NVALS);

        std::memcpy(m_timestamp.get(), timestamp, NVALS * sizeof(double));
        std::memcpy(m_relh.get(), relh, NVALS * sizeof(double));
        std::memcpy(m_tair.get(), tair, NVALS * sizeof(double));
        std::memcpy(m_wspd.get(), wspd, NVALS * sizeof(double));
        std::memcpy(m_wdir.get(), wdir, NVALS * sizeof(double));
        std::memcpy(m_gust.get(), gust, NVALS * sizeof(double));
        std::memcpy(m_rain.get(), rain, NVALS * sizeof(double));
        std::memcpy(m_pres.get(), pres, NVALS * sizeof(double));
        std::memcpy(m_srad.get(), srad, NVALS * sizeof(double));
    }
};

}  // namespace nfdrs

#endif
