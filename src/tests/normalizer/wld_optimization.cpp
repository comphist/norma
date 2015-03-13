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
#include"wld_optimization.h"
#include<string>
#include<fstream>
#include<iostream>
#include<iomanip>
#include<sstream>
#include<map>
#include<vector>
#include<stdexcept>
#include<chrono>
#include<boost/program_options.hpp>  //NOLINT[build/include_order]
#include<boost/filesystem.hpp>       //NOLINT[build/include_order]
#include"config.h"
#include"gfsm_wrapper.h"
#include"string_impl.h"
#include"normalizer/lexicon.h"
#include"normalizer/result.h"

using Norma::Normalizer::Result;
using Norma::Normalizer::Lexicon;
using std::chrono::steady_clock;
using std::chrono::duration;

namespace {
void strip_boundary_symbol(std::vector<string_impl>* vec) {
    if (vec->back() == Lexicon::SYMBOL_BOUNDARY)
        vec->pop_back();
}

Result BenchmarkWLD::operator()(const string_impl& word) const {
    _cascade->set_max_paths(1);
    auto results = _cascade->lookup_nbest(word);
    for (auto stringpath : results) {
        std::vector<string_impl> output = stringpath.output;
        strip_boundary_symbol(&output);
        return make_result(Gfsm::implode(output),
                           stringpath.weight);
    }
    return make_result(word, 0.0);
}
}  // namespace

namespace cfg = boost::program_options;

void normalize_and_time(BenchmarkWLD& wld, string_impl word_impl) {  //NOLINT[runtime/references]
    // normalize the word & time the process
    auto time_before = steady_clock::now();
    Result result = wld(word_impl);
    auto time_after  = steady_clock::now();
    auto time_taken  = time_after - time_before;

    std::cout << result.word
              << std::setw(12) << result.score
              << std::setw(12)
              << duration<double, std::milli>(time_taken).count()
              << std::endl;
}

void perform_benchmark(BenchmarkWLD& wld, std::string word,    //NOLINT[runtime/references]
                       double max_weight, bool reinit) {
    string_impl word_impl;
    std::stringstream ss;
    ss << word;
    ss >> word_impl;
    int wordcol = word.length() + 5;
    double last_weight = 0.0;

    std::cout << "Starting benchmark on input: " << word_impl
              << std::endl << std::endl
              << std::setw(12) << "max_weight"
              << std::setw(9) << "max_ops"
              << std::setw(wordcol) << "result"
              << std::setw(12) << "weight"
              << std::setw(12) << "time (ms)"
              << std::endl << std::setfill('-')
              << std::setw(45+wordcol) << "--"
              << std::setfill(' ')
              << std::setprecision(2) << std::fixed
              << std::endl;

    std::vector<double> weights { 0.0, 0.1, 0.25, 0.5, 1.0, 2.0, 3.0,
                                  4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 };
    for (double w : weights) {
      if (w > max_weight)
          break;
      if (reinit)
          wld.init();
      std::cout << std::setw(12) << w;
      wld.set_max_weight(w);
      last_weight = w;
      std::cout << std::setw(9) << -1;
      std::cout << std::setw(wordcol);
      normalize_and_time(wld, word_impl);
    }

    std::vector<unsigned int> ops { 100, 250, 500, 1000, 2500, 5000,
                                    10000, 25000, 50000, 100000 };
    for (unsigned int o : ops) {
      if (reinit)
          wld.init();
      std::cout << std::setw(12) << last_weight;
      wld.set_max_weight(last_weight);
      wld.set_max_ops(o);
      std::cout << std::setw(9) << o;
      std::cout << std::setw(wordcol);
      normalize_and_time(wld, word_impl);
    }
}

int main(int argc, char* argv[]) {
    // copypasta from main.cpp, for the most part
    cfg::options_description desc_all("Available options");
    desc_all.add_options()
        ("help,h", "Display this helpful message.")
        ("config,c", cfg::value<std::string>(),
         "Configuration file to be loaded on start-up")
        ("word,w", cfg::value<std::string>(),
         "Word to be normalized")
        ("maxweight,m", cfg::value<double>(),
         "Maximum weight to test")
        ("reinit,r", cfg::bool_switch()->default_value(false),
         "Reinitialize the normalizer after each trial")
        ;  //NOLINT[whitespace/semicolon]

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
        std::cout << "Benchmark for WLD normalizer with various settings,"
                  << std::endl << "for "
                  << NORMA_NAME << " " << NORMA_VERSION
                  << std::endl
                  << "(c) 2012-2014 Marcel Bollmann, Florian Petran"
                  << std::endl << std::endl;
        std::cout << desc_all << std::endl;
        return 0;
    }

    if (!m.count("config")) {
        std::cerr << "Error: Configuration file required!" << std::endl;
        return 1;
    }
    if (!m.count("word")) {
        std::cerr << "Error: No word given!" << std::endl;
        return 1;
    }

    Lexicon lex;
    lex.set_from_params(file_opts);
    lex.init();
    BenchmarkWLD wld;
    wld.set_from_params(file_opts);
    wld.set_lexicon(&lex);
    wld.init();
    double max_weight = (m.count("maxweight") > 0) ?
                          m["maxweight"].as<double>() : 100.0;
    perform_benchmark(wld, m["word"].as<std::string>(),
                      max_weight, m["reinit"].as<bool>());

    return 0;
}
