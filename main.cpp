#include <iostream>
#include "big_int.h"
#include "functions.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main_callback(int argc, char* argv[]) {
    po::options_description desc("Options");
    desc.add_options()
            ("help,h", "Please select sign or verify mode according to you needs." \
                       "Please provide correct arguments to the key pair." \
                       "There is no varranty of correctness of singature or verification is parameters are invalid PASS ALL NUMBERS IN HEX")
            ("sign,s", po::value<bool>()->default_value(false), "Sign message mode")
            ("verify,v", po::value<bool>()->default_value(false), "Verify digital signature mode")
            ("message,m", po::value<std::string>()->default_value(""), "Message to sing/verify")
            ("n", po::value<std::string>()->default_value(""), "Modulp n")
            ("e", po::value<std::string>()->default_value(""), "Public key e")
            ("d", po::value<std::string>()->default_value(""), "Secret key d")
            ("p", po::value<std::string>()->default_value(""), "Prime p")
            ("q", po::value<std::string>()->default_value(""), "Prime q");

    po::variables_map opts;

    try {
        po::store(po::parse_command_line(argc, argv, desc), opts);

        if (opts.count("help")) {
            desc.print(std::cout);
            return 1;
        }
    } catch (const std::exception& ce) {
        std::cout << "Exception occured while parsing commanf line arguments: "
                  << typeid (ce).name() << " " << ce.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "Unknown exception occured while parsing commanf line arguments" << std::endl;
    }

    bool sign_m = opts["sign"].as<bool>();
    bool verify_m = opts["verify"].as<bool>();

    if ((sign_m && verify_m) || (!sign_m && !verify_m)) {
        std::cout << "Both sign and verify modes chosen or none of them. Aborting...";
        return 1;
    }

    if (sign_m) {
        auto message = opts["message"].as<std::string>();
        if (message.empty()) {
            std::cout << "Message is emtpy!!! Aborting...";
            return 1;
        }


        auto n_str = opts["n"].as<std::string>();
        auto d_str = opts["d"].as<std::string>();
        auto q_str = opts["q"].as<std::string>();
        auto p_str = opts["p"].as<std::string>();


        if (d_str.empty()) {
            std::cout << "empty private key";
            return 1;
        }

        big_int* n = nullptr, p = nullptr, q = nullptr;

        if (d_str.empty()) {
            return 1;
        }

        big_int * d = new big_int(d_str);

        if (n_str.empty()) {
            if (p_str.empty() || q_str.empty()) {
                return 1;
            } else {
                p = new big_int (p_str);
                q = new big_int (q_str);
                n = new big_int (p);
                n->mul(&q);
            }
        }

        big_int* m = new big_int (message);

        auto ds = sign(m, d, n);

        std::cout << "s: " << ds[1]->showNumHex();

        return 0;
    }

    if (verify_m) {
        auto message = opts["message"].as<std::string>();
        if (message.empty()) {
            std::cout << "Message is emtpy!!! Aborting...";
            return 1;
        }

        auto base = opts["base"].as<int>();
        if (base == 0) {
            std::cout << "Wrong base given. Aborting";
            return 1;
        }


        auto e_str = opts["e"].as<std::string>();
        auto n_str = opts["n"].as<std::string>();
        auto m_str = opts["m"].as<std::string>();
        auto s_str = opts["s"].as<std::string>();

        if (e_str.empty() || n_str.empty() || n_str.empty() || s_str.empty()) {
            std::cout << "Invalid public key given. Aborting...";
        }

        big_int* e = new big_int (e_str);
        big_int* n = new big_int (n_str);
        big_int* m = new big_int (m_str);
        big_int* s = new big_int (s_str);


        std::cout << "result: " << verify(m, s, n, e);


        return 0;
    }

    return -1;
}

int main(int argc, char* argv[])
{
    auto res = main_callback(argc, argv);
    std::cout << endl;

    return res;
}
