#include <NFDRSGUI/FW21Decoder.h>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string_view>

namespace fw21 {

void parse_row(FW21Timeseries& ts_data, std::string_view buffer,
               char delimiter = ',') {
    std::cout << buffer << std::endl;
}

FW21Timeseries decode_fw21(std::string_view data_buffer) {
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

    printf("Done!\n");
    return ts_data;
}

}  // namespace fw21
