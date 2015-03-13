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
#include<iostream>
#include<sstream>
#include<fstream>
#include<boost/program_options.hpp>  //NOLINT[build/include_order]
#include<boost/filesystem.hpp>       //NOLINT[build/include_order]
#include"config.h"
#include"normalizer/exceptions.h"
#include"normalizer/lexicon.h"

namespace cfg = boost::program_options;

void check_if_file_exists(const std::string& name, bool status);

int main(int argc, char* argv[]) {
    cfg::options_description desc("Options");
    desc.add_options()
        ("help,h", "Display this helpful message.")
        ("automaton,a", cfg::value<std::string>()->required(),
         "File containing a lexicon FST in binary format.")
        ("labels,l", cfg::value<std::string>()->required(),
         "File containing labels for the given lexicon FST.")
        ("words,w", cfg::value<std::string>()->required(),
         "File containing one lexicon entry per line in plain text format.")
        ("compile,c", cfg::bool_switch()->default_value(false),
         "Generate the lexicon FST and a labels file from the entries "
         "in the (plain text) words file.")
        ("extract,x", cfg::bool_switch()->default_value(false),
         "Write all lexicon entries in the lexicon FST with the associated "
         "labels file to the words file (in plain text).")
        ("no-optimize,n", cfg::bool_switch()->default_value(false),
         "When using --compile, don't perform additional optimizations "
         "on the FST. This will result in larger file sizes. Use this switch "
         "if compilation takes too long otherwise.")
        ("force", cfg::bool_switch()->default_value(false),
         "Force the operation when output files already exist; in this case, "
         "entries are appended to the lexicon FST (when using -c) or the "
         "words file (when using -x).")
        ;  //NOLINT[whitespace/semicolon]
    cfg::variables_map m;
    try {
        cfg::store(cfg::parse_command_line(argc, argv, desc), m);
        if (m.count("help")) {
            std::cout << NORMA_NAME << " " << NORMA_VERSION
                      << " lexicon compilation/extraction tool"
                      << std::endl
                      << "(c) 2013-2015 Marcel Bollmann, Florian Petran"
                      << std::endl << std::endl
                      << desc << std::endl;
            return 0;
        }
        cfg::notify(m);
        if (m["compile"].as<bool>() == m["extract"].as<bool>()) {
            throw cfg::error("Need exactly one of --compile/-c "
                             "or --extract/-x.");
        }
    }
    catch(cfg::error e) {
        std::cerr << "Error parsing command-line options: "
                  << e.what() << std::endl;
        return 1;
    }

    int return_code = 0;
    try {
        // ###################### COMPILE ######################
        if (m["compile"].as<bool>()) {
            // check files
            if (!m["force"].as<bool>()) {
                check_if_file_exists(m["automaton"].as<std::string>(), false);
                check_if_file_exists(m["labels"].as<std::string>(), false);
            }
            check_if_file_exists(m["words"].as<std::string>(), true);
            // set up stuff
            Norma::Normalizer::Lexicon lex;
            lex.set_lexfile(m["automaton"].as<std::string>());
            lex.set_symfile(m["labels"].as<std::string>());
            try {
                lex.init();
            } catch(Norma::Normalizer::init_error e) {}  // this is expected
            std::ifstream file;
            file.open(m["words"].as<std::string>(), std::ios::in);
            // read words
            std::cout << "Reading words and creating FST..." << std::endl;
            std::string line = "";
            while (getline(file, line)) {
                if (file.eof())
                    break;
                std::istringstream iss(line);
                string_impl word;
                iss >> word;
                lex.add(word);
            }
            file.close();
            if (!m["no-optimize"].as<bool>()) {
                std::cout << "Optimizing FST..." << std::endl;
                lex.optimize();
            }
            std::cout << "Saving..." << std::endl;
            lex.save_params();
        // ###################### EXTRACT ######################
        } else if (m["extract"].as<bool>()) {
            // check files
            if (!m["force"].as<bool>())
                check_if_file_exists(m["words"].as<std::string>(), false);
            // open words file for writing
            std::ofstream file;
            file.open(m["words"].as<std::string>(), std::ios::app);
            // read lexicon
            std::cout << "Opening and parsing lexicon FST..." << std::endl;
            Norma::Normalizer::Lexicon lex;
            lex.set_lexfile(m["automaton"].as<std::string>());
            lex.set_symfile(m["labels"].as<std::string>());
            lex.init();
            auto entries = lex.entries();
            // write to file
            std::cout << "Writing to words file..." << std::endl;
            for (auto entry : entries) {
                file << entry << std::endl;
            }
            file.close();
        }
        std::cout << "Done." << std::endl;
        // #####################################################
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
            throw;  // rethrow for post-mortem analysis
        }
    }

    return return_code;
}

void check_if_file_exists(const std::string& name, bool status) {
    if (boost::filesystem::exists(name) == status)
        return;
    if (status) {
        throw std::runtime_error("required file does not exist: " + name);
    } else {
        throw std::runtime_error("file already exists and --force not specified: " + name);
    }
}
