import pandas as pd
import numpy as np

def main():

    df = pd.read_csv("../data/2024-03-CHEY-firewx.csv", parse_dates=["DateTime"])
    df["DateTime"] = df["DateTime"].astype('int').div(10**9)

    header_start_str = "#ifndef NFDRS4_TEST_DATA\n#define NFDRS4_TEST_DATA\n"
    header_start_str += "#include <cstdint>\n"
    header_end_str = "\n#endif\n"
    size_str = f"constexpr int N = {df['RELH'].shape[0]};\n"
    time_str = "constexpr int64_t time[] = {"
    relh_str = "constexpr double relh[] = {"
    tmpc_str = "constexpr double tmpc[] = {"
    wspd_str = "constexpr double wspd[] = {"
    wdir_str = "constexpr double wdir[] = {"
    rain_str = "constexpr double rain[] = {"
    pres_str = "constexpr double pres[] = {"
    srad_str = "constexpr double srad[] = {"

    time_str += ','.join(f"{int(val)}" for val in df['DateTime'])
    relh_str += ','.join(f"{val}f" for val in df['RELH'])
    tmpc_str += ','.join(f"{val}f" for val in df['TAIR'])
    wspd_str += ','.join(f"{val}f" for val in df['WSPD'])
    wdir_str += ','.join(f"{val}f" for val in df['WDIR'])
    rain_str += ','.join(f"{val}f" for val in df['RAIN'])
    pres_str += ','.join(f"{val}f" for val in df['PRES'])
    srad_str += ','.join(f"{val}f" for val in df['SRAD'])

    time_str += "};\n"
    relh_str += "};\n"
    tmpc_str += "};\n"
    wspd_str += "};\n"
    wdir_str += "};\n"
    rain_str += "};\n"
    pres_str += "};\n"
    srad_str += "};\n"

    with open("../include/NFDRSGUI/data.h", 'w') as outfile:
        outfile.write(header_start_str)
        outfile.write(time_str)
        outfile.write(size_str)
        outfile.write(relh_str)
        outfile.write(tmpc_str)
        outfile.write(wspd_str)
        outfile.write(wdir_str)
        outfile.write(rain_str)
        outfile.write(pres_str)
        outfile.write(srad_str)
        outfile.write(header_end_str)



if __name__ == "__main__":
    main()
