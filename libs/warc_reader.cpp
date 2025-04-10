#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <string_view>
#include "warc_reader.hpp"
#include "url_filter.hpp"

const size_t BUFFER_SIZE = 4*1024 * 1024;

int processWarc() {
    std::ifstream warcFile("data/warc/CC-MAIN-20180420081400-20180420101400-00000.warc", std::ios::in | std::ios::binary);
    std::vector<WarcRecord> retval;
    int count=0;

    std::vector<char> buffer(BUFFER_SIZE);
    std::string leftover;


    while (warcFile) {
        warcFile.read(buffer.data(), buffer.size());
        std::streamsize bytesRead = warcFile.gcount();
        if (bytesRead == 0) break;

        std::string chunk;
        chunk.reserve(leftover.size() + bytesRead);
        chunk.append(leftover);
        chunk.append(buffer.data(),bytesRead);
        leftover.clear();

        size_t pos = 0;

        while (true) {
            size_t record_start = chunk.find("WARC/", pos);
            if (record_start == std::string::npos) {
                leftover = chunk.substr(pos);
                break;
            }

            size_t next_record = chunk.find("WARC/", record_start + 5);
            // If we didn’t find the next record, the rest of the chunk might be incomplete.
            if (next_record == std::string::npos) {
                leftover = chunk.substr(record_start);
                break;
            }

            // Only slice now that we know we have a full record block
            std::string_view record(chunk.data() + record_start, next_record - record_start);

            std::string_view record_id, uri, date;
            size_t content_length = 0;

            size_t header_pos = 0;
            while (true) {
                size_t line_end = record.find('\n', header_pos);
                if (line_end == std::string::npos) break;

                std::string_view line = record.substr(header_pos, line_end - header_pos);
                header_pos = line_end + 1;

                if (line.empty() || line == "\r") break;

                if (line.starts_with("WARC-Record-ID:")) {
                    record_id = line.substr(15);
                } else if (line.starts_with("WARC-Target-URI:")) {
                    uri = line.substr(17);
                } else if (line.starts_with("WARC-Date:")) {
                    date = line.substr(10);
                } else if (line.starts_with("Content-Length:")) {
                    content_length = std::stoul(std::string(line.substr(15)));
                }
            }

            // Find payload start
            size_t payload_start = record.find("\r\n\r\n", header_pos);
            if (payload_start != std::string::npos) {
                payload_start += 4;
            } else {
                payload_start = record.find("\n\n", header_pos);
                if (payload_start != std::string::npos) {
                    payload_start += 2;
                } else {
                    // malformed record, skip
                    pos = next_record;
                    continue;
                }
            }

            // Payload offset relative to chunk
            size_t absolute_payload_start = record_start + payload_start;

            if (absolute_payload_start + content_length > chunk.size()) {
                // payload isn't fully in this chunk yet — save full record to leftover
                leftover = chunk.substr(record_start);
                break;
            }

            // Now safely extract payload
            std::string_view payload(chunk.data() + absolute_payload_start, content_length);
            if(filterUrl(uri)){
                count++;
                
            }
            pos = next_record;
            
        }
    }
    return count;
}
