#pragma once


class Utils {
public:
    static std::vector<std::string> args_to_strings(int argc, char *argv[]) {
        std::vector<std::string> args;
        args.reserve(argc);
        for (int i = 0; i < argc; i++) {
            args.emplace_back(argv[i]);
        }
        args.erase(args.begin());
        return args;
    }
};

