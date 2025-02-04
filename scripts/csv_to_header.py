import pandas as pd
import numpy as np


def main():

    df = pd.read_csv("../data/2024-03-CHEY-firewx.csv",
                     parse_dates=["DateTime"])
    df["DateTime"] = df["DateTime"].astype('int').div(10**9)
    df["WMAX"] = np.around(df["WMAX"], decimals=2)
    # Go from accumulation since 00Z to 1-hour accum
    df["RAIN"] = np.around(np.abs(df["RAIN"].diff(periods=1)), decimals=3)
    df["RAIN"] = df["RAIN"].fillna(0)

    header_start_str = "#ifndef NFDRS4_TEST_DATA\n#define NFDRS4_TEST_DATA\n"
    header_end_str = "\n#endif\n"

    size_str = f"static constexpr int NSTATIC = {df['RELH'].shape[0]};\n"
    time_str = "static constexpr double timestamp[] = {"
    relh_str = "static constexpr double relh[] = {"
    tmpc_str = "static constexpr double tmpc[] = {"
    wspd_str = "static constexpr double wspd[] = {"
    wdir_str = "static constexpr double wdir[] = {"
    gust_str = "static constexpr double gust[] = {"
    rain_str = "static constexpr double rain[] = {"
    pres_str = "static constexpr double pres[] = {"
    srad_str = "static constexpr double srad[] = {"

    time_str += ','.join(f"{val}" for val in df['DateTime'])
    relh_str += ','.join(f"{val}" for val in df['RELH'])
    tmpc_str += ','.join(f"{val}" for val in df['TAIR'])
    wspd_str += ','.join(f"{val}" for val in df['WSPD'])
    wdir_str += ','.join(f"{val}" for val in df['WDIR'])
    gust_str += ','.join(f"{val}" for val in df['WMAX'])
    rain_str += ','.join(f"{val}" for val in df['RAIN'])
    pres_str += ','.join(f"{val}" for val in df['PRES'])
    srad_str += ','.join(f"{val}" for val in df['SRAD'])

    time_str += "};\n"
    relh_str += "};\n"
    tmpc_str += "};\n"
    wspd_str += "};\n"
    wdir_str += "};\n"
    gust_str += "};\n"
    rain_str += "};\n"
    pres_str += "};\n"
    srad_str += "};\n"

    with open("../include/NFDRSGUI/data.h", 'w') as outfile:
        outfile.write(header_start_str)
        outfile.write(size_str)
        outfile.write(time_str)
        outfile.write(relh_str)
        outfile.write(tmpc_str)
        outfile.write(wspd_str)
        outfile.write(wdir_str)
        outfile.write(gust_str)
        outfile.write(rain_str)
        outfile.write(pres_str)
        outfile.write(srad_str)
        outfile.write(header_end_str)


if __name__ == "__main__":
    main()
