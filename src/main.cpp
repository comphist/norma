/* Copyright 2013-2015 Marcel Bollmann, Florian Petran
 *
 * This file is part of Norma.
 *
 * Norma is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * Norma is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with Norma.  If not, see <http://www.gnu.org/licenses/>.
 */
#include<string>
#include<fstream>
#include<iostream>
#include<map>
#include<vector>
#include<stdexcept>
#include<boost/program_options.hpp>  //NOLINT[build/include_order]
#include<boost/filesystem.hpp>       //NOLINT[build/include_order]
#include"config.h"
#include"string_impl.h"
#include"cycle.h"
#include"interface.h"

namespace cfg = boost::program_options;

int main(int argc, char* argv[]) {
    cfg::options_description desc("Interface options");
    desc.add_options()
        ("help,h", "Display this helpful message.")
        ("config,c", cfg::value<std::string>(),
         "Configuration file to be loaded on start-up")
        ("file,f", cfg::value<std::string>(),
         "Input file for the normalization; can contain either one or two "
         "wordforms (tab-separated) per line.  Two wordforms per line are "
         "interpreted as training pairs of historical and modern cognates.")
        ("perfilemode,p", cfg::bool_switch()->default_value(false),
         "Use per-file mode for normalizer parameters.  With this mode, if "
         "a parameter file is not specified in the configuration file, "
         "the input filename (with a parameter-specific file extension) "
         "will be used instead. "
         "Has no effect when not using the '-f' option.")
        ("verbose,v", cfg::bool_switch()->default_value(false),
         "Enable verbose status output to stderr")
        ("sync,s", cfg::bool_switch()->default_value(false),
         "Run synchronously (don't start multiple threads).")
        ("plugin-base,P",
         cfg::value<std::string>()->default_value(NORMA_DEFAULT_PLUGIN_BASE),
         "Base directory for the normalizer plugins."
         "Default value: current directory")
        ;  //NOLINT[whitespace/semicolon]
    cfg::options_description desc_config("Normalization options");
    desc_config.add_options()
        ("train,t", cfg::bool_switch()->default_value(false),
         "Train only, do not generate normalizations.")
        ("saveonexit", cfg::bool_switch()->default_value(false),
         "Save normalizer parameter files when exiting.  Without this option, "
         "changes resulting from normalizer training only have an effect "
         "during the current session and are discarded afterwards.")
        ("normalizers", cfg::value<std::string>(),
         "Normalizer chain as a comma-separated list")
        ;  //NOLINT[whitespace/semicolon]
    cfg::options_description desc_all("All options");
    desc_all.add(desc).add(desc_config);

    cfg::variables_map m;
    try {
        cfg::store(cfg::parse_command_line(argc, argv, desc_all), m);
    }
    catch(cfg::error e) {
        std::cerr << "Error parsing command-line options: "
                  << e.what() << std::endl;
        return 1;
    }
    cfg::parsed_options opts(&desc_all);
    // file_opts contains unrecognized options from the config file
    // -- this will typically be normalizer-specific settings
    //    that neither can nor should be registered here
    std::map<std::string, std::string> file_opts;
    if (m.count("config")) {
        std::ifstream cfg_file;
        cfg_file.open(m["config"].as<std::string>());
        if (!cfg_file.is_open()) {
            std::cerr << "Error: Config file not found: "
                      << m["config"].as<std::string>()
                      << std::endl;
            return 1;
        }
        opts = cfg::parse_config_file(cfg_file, desc_all, true);
        cfg::store(opts, m);
        // stupid boost::po returns unregistered options as vector of strings,
        // so we need to read them into a map.
        std::vector<std::string> opts_vec =
            cfg::collect_unrecognized(opts.options, cfg::exclude_positional);
        for (size_t i = 1; i < opts_vec.size(); i += 2)
            file_opts[opts_vec[i-1]] = opts_vec[i];
        if (!file_opts.count("parent_path")) {
            boost::filesystem::path p(m["config"].as<std::string>());
            file_opts["parent_path"] =
                boost::filesystem::canonical(p.parent_path()).string();
        }
    }
    cfg::notify(m);

    if (m.count("help")) {
        std::cout << NORMA_NAME << " " << NORMA_VERSION << ", "
                  << "a spelling normalization tool"
                  << std::endl
                  << "Copyright (c) 2013-2015 Marcel Bollmann, Florian Petran"
                  << std::endl << std::endl;
        std::cout << desc_all << std::endl;
        return 0;
    }

    if (!m.count("normalizers")) {
        // maybe checking this string and even splitting it up into a vector
        // could be done using a Boost 'notifier' attached to the option?
        std::cerr << "Error: Normalizer chain not specified!" << std::endl;
        return 1;
    }

    Norma::Input *input = nullptr;
    Norma::Output *output = nullptr;
    int return_code = 0;

    try {
        if (m.count("file")) {
            input = new Norma::FileInput(m["file"].as<std::string>());
            if (m["perfilemode"].as<bool>())
                file_opts["perfilemode.input"] = m["file"].as<std::string>();
        } else {
            std::cerr << "Please specify an input file! (see -h for help)"
                      << std::endl;
            return 1;
        }
        output = new Norma::Output();
        Norma::Cycle c;
        c.init(input, output, file_opts);
        c.init_chain(m["normalizers"].as<std::string>(),
                     m["plugin-base"].as<std::string>());
        if (m["sync"].as<bool>())
            c.set_thread(false);
        // v-- this doesn't work for some reason
        if (m["train"].as<bool>())
            c.set("normalize", false);
        c.start();
        if (m["saveonexit"].as<bool>())
            c.save_params();
    } catch(...) {
        return_code = 1;
        try {
            throw;
        } catch(std::bad_function_call e) {
            std::cerr << "Bad function call: " << e.what() << std::endl;
        } catch(std::runtime_error e) {
            std::cerr << "Runtime error: " << e.what() << std::endl;
        } catch(std::out_of_range e) {
            std::cerr << "Out of range: " << e.what() << std::endl;
        } catch(std::logic_error e) {
            std::cerr << "Logic error: " << e.what() << std::endl;
        } catch(...) {
            std::cerr << "Unknown error! Something horrible happened."
                      << std::endl;
            if (input != nullptr)
                delete input;
            if (output != nullptr)
                delete output;
            throw;  // rethrow for post-mortem analysis
        }
    }

    // cleanup
    if (input != nullptr)
        delete input;
    if (output != nullptr)
        delete output;

    return return_code;
}
