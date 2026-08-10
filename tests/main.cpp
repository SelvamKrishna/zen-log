#include <zen/log.hpp>

using namespace zen;

void test_real_world_scenario() {
    std::cout << "\n=== Real-World: File Processing Pipeline ===\n";

    static log_tag io_tag {"IO", ansi_color::BLUE};
    static log_tag parse_tag {"PARSER", ansi_color::GREEN};
    static log_tag validate_tag {"VALIDATOR", ansi_color::YELLOW};

    struct FileProcessor {
        std::string filename;

        bool read_file() {
            log_process read_op(fstr("Reading file: {}", filename), &io_tag);

            // Simulate reading
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            bool file_exists = true;
            bool permissions_ok = true;

            if (!file_exists) {
                read_op.log_failure(fstr("File '{}' not found", filename));
                return false;
            }

            if (!permissions_ok) {
                read_op.log_failure(fstr("Permission denied for '{}'", filename));
                return false;
            }

            read_op.log_success();
            return true;
        }

        bool parse_file() {
            log_process parse_op("Parsing file contents", &parse_tag);

            std::this_thread::sleep_for(std::chrono::milliseconds(30));

            // Simulate parsing issues
            bool format_valid = true;
            if (!format_valid) {
                parse_op.log_failure(fstr("Invalid format in '{}'", filename));
                return false;
            }

            parse_op.log_success();
            return true;
        }

        bool validate_data() {
            log_process validate_op("Validating processed data", &validate_tag);

            std::this_thread::sleep_for(std::chrono::milliseconds(20));

            bool data_consistent = true;
            if (!data_consistent) {
                validate_op.log_fallback("Data consistency check failed, attempting repair");
                // Try to repair
                bool repair_success = true;
                if (repair_success) {
                    validate_op.log_success();
                    return true;
                } else {
                    validate_op.log_failure("Data repair failed");
                    return false;
                }
            }

            validate_op.log_success();
            return true;
        }

        bool process() {
            log_process pipeline(fstr("Processing file: {}", filename));

            if (!read_file()) {
                pipeline.log_failure("Pipeline failed at read stage");
                return false;
            }

            if (!parse_file()) {
                pipeline.log_failure("Pipeline failed at parse stage");
                return false;
            }

            if (!validate_data()) {
                pipeline.log_failure("Pipeline failed at validation stage");
                return false;
            }

            pipeline.log_success();
            return true;
        }
    };

    // Test successful processing
    {
        FileProcessor processor{"data.json"};
        processor.process();
    }

    std::cout << "\n";

    // Test failed processing
    {
        FileProcessor processor{"missing.csv"};
        processor.process();
    }
}

int main() {
    logger::init();
    test_real_world_scenario();
    return 0;
}
