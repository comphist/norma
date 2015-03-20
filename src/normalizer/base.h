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
#ifndef NORMALIZER_BASE_H_
#define NORMALIZER_BASE_H_
#include<list>
#include<map>
#include<string>
#include<mutex>
#include<boost/filesystem.hpp>  // NOLINT[build/include_order]
#include"string_impl.h"
#include"result.h"
#include"training_data.h"

namespace Norma {
class Input;
class Output;
class IOBase;

namespace Normalizer {
class LexiconInterface;

/// Pure virtual base class for all normalizers
/** Your own normalizer in 4 easy steps:
 *  1. Extend this class. The compiler will tell you if you forgot to implement
 *     a method. For an example, look at the MapperNormalizer, a simple
 *     dictionary lookup normalization method.
 *  2. Create an object on the heap with the settings read from the cfg file
 *     as parameter.
 *  3. Register it somewhere with the Applicator object you're using with
 *     Applicator::register_method. Note that shipped normalizers are
 *     already automatically registered.
 *  4. You can now use the name you defined in Normalizer::name to add it to
 *     the chain in the cfg file.
 *  5. That's it already. Easy, wasn't it?
 **/
class Base {
 public:
     Base() = default;
     virtual ~Base() {}
     /// Initialization
     virtual void init() = 0;
     /// Initialization over a set of parameters
     virtual void init(const std::map<std::string, std::string>& params,
                       LexiconInterface* lexicon) {
        set_lexicon(lexicon);
        set_from_params(params);
        init();
     }
     virtual void set_from_params(const std::map<std::string, std::string>&
                                                                    params) = 0;
     /// Clear all trained parameters
     /** After calling this function, the normalizer should be in a clean,
         untrained state.  Only *trainable* parameters of the normalizer
         should be affected; in particular, this function should not change
         the lexicon pointer, any filenames of parameter files, or any
         normalizer-specific settings that are not the result of training.
      */
     virtual void clear() {}
     /// Retrieve lexicon pointer
     LexiconInterface* get_lexicon() const { return _lex; }
     /// Set lexicon pointer
     virtual void set_lexicon(LexiconInterface* lexicon) { _lex = lexicon; }
     /// Start training the normalizer given the Input/Output history
     /// return true when training is done
     virtual bool train(TrainingData* data) = 0;
     /// Normalize function
     virtual Result operator()(const string_impl& word) const = 0;
     /// Normalize to N best results
     virtual ResultSet operator()(const string_impl& word,
                                  unsigned int n) const = 0;
     /// Save parameters to file(s)
     virtual void save_params() = 0;
     /// This must return a unique name as const char*
     /// Note that uniqueness of the name is not enforced.
     virtual const char* name() const = 0;
     std::mutex mutex;

 protected:
     void log_message(Result* result, LogLevel level, std::string message) const {
         result->messages.push(make_message(level, name(), message));
     }
     std::string to_absolute(const std::string& path,
                             const std::map<std::string, std::string>& params) const {
         boost::filesystem::path p(path);
         if (p.is_relative() && params.count("parent_path") != 0) {
             p = boost::filesystem::path(params.at("parent_path"))
                 / p;
         }
         return p.string();
     }
     std::string with_extension(const std::string& path,
                                const std::string& extension) const {
         boost::filesystem::path p(path);
         p.replace_extension(extension);
         return p.string();
     }
     LexiconInterface* _lex = nullptr;

     inline Result make_result(const string_impl& word, double score) const
     { return Result(word, score, name()); }
};
}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_BASE_H_

