import pandas as pd
import numpy as np

def main():

    df = pd.read_csv("../data/2024-03-CHEY-firewx.csv")

    header_start_str = "#ifndef NFDRS4_TEST_DATA\n#define NFDRS4_TEST_DATA\n"
    header_end_str = "\n#endif\n"
    size_str = f"constexpr int N = {df['RELH'].shape[0]};\n"
    relh_str = "constexpr double relh[] = {"
    tmpc_str = "constexpr double tmpc[] = {"
    wspd_str = "constexpr double wspd[] = {"
    wdir_str = "constexpr double wdir[] = {"
    rain_str = "constexpr double rain[] = {"
    pres_str = "constexpr double pres[] = {"
    srad_str = "constexpr double srad[] = {"

    relh_str += ','.join(f"{val}f" for val in df['RELH'])
    tmpc_str += ','.join(f"{val}f" for val in df['TAIR'])
    wspd_str += ','.join(f"{val}f" for val in df['WSPD'])
    wdir_str += ','.join(f"{val}f" for val in df['WDIR'])
    rain_str += ','.join(f"{val}f" for val in df['RAIN'])
    pres_str += ','.join(f"{val}f" for val in df['PRES'])
    srad_str += ','.join(f"{val}f" for val in df['SRAD'])

    relh_str += "};\n"
    tmpc_str += "};\n"
    wspd_str += "};\n"
    wdir_str += "};\n"
    rain_str += "};\n"
    pres_str += "};\n"
    srad_str += "};\n"

    with open("../include/NFDRSGUI/data.h", 'w') as outfile:
        outfile.write(header_start_str)
        outfile.write(size_str)
        outfile.write(relh_str);
        outfile.write(tmpc_str);
        outfile.write(wspd_str);
        outfile.write(wdir_str);
        outfile.write(rain_str);
        outfile.write(pres_str);
        outfile.write(srad_str);
        outfile.write(header_end_str)



if __name__ == "__main__":
    main()
