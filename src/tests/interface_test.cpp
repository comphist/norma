/* Copyright 2013-2017 Marcel Bollmann, Florian Petran
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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Interface
#include<iostream>
#include<sstream>
#include<string>
#include<boost/test/included/unit_test.hpp>  // NOLINT[build/include_order]
#include<boost/test/output_test_stream.hpp>  // NOLINT[build/include_order]
#include"config.h"
#include"cycle.h"
#include"training_data.h"
#include"interface/input.h"
#include"interface/output.h"
#include"interface/iobase.h"

class IStreamWrapper {
  public:
      void put_line(const std::string& instr) {
          buffer = std::cin.rdbuf();
          mock_stream << instr;
          std::cin.rdbuf(mock_stream.rdbuf());
      }
  private:
      std::streambuf* buffer;
      std::stringstream mock_stream;
};

struct InterfaceTestFixture {
    Norma::Input *input = nullptr;
    Norma::Output *output = nullptr;
    Norma::TrainingData *tdata = nullptr;
    Norma::Cycle *c = nullptr;

    IStreamWrapper cin_wrapper;
    boost::test_tools::output_test_stream test_stdout;

    InterfaceTestFixture() {
        tdata = new Norma::TrainingData();
    }

    ~InterfaceTestFixture() {
        delete tdata;
        if (input != nullptr)
            delete input;
        if (output != nullptr)
            delete output;
    }
};

BOOST_FIXTURE_TEST_SUITE(InterfaceTest, InterfaceTestFixture)

struct cout_redirect {
    cout_redirect(std::streambuf* new_buffer)
        : old(std::cout.rdbuf(new_buffer)) {}
    ~cout_redirect() {
        std::cout.rdbuf(old);
    }
    std::streambuf* old;
};

BOOST_AUTO_TEST_CASE(NormalOutputTest) {
    // test normal (non-interactive output)
    // it doesn't do much except print the result with
    // or without probability
    // TODO check log level?
    output = new Norma::Output();
    output->initialize(c, input, tdata);

    BOOST_REQUIRE(output->thread_suitable());

    Norma::Normalizer::Result res("foo", 0.5);

    {
        cout_redirect guard(test_stdout.rdbuf());
        output->put_line(&res, false, Norma::Normalizer::LogLevel::SILENT);
    }
    BOOST_CHECK(test_stdout.is_equal("foo\n"));

    {
        cout_redirect guard(test_stdout.rdbuf());
        output->put_line(&res, true, Norma::Normalizer::LogLevel::SILENT);
    }
    BOOST_CHECK(test_stdout.is_equal("foo\t0.5\n"));
}

BOOST_AUTO_TEST_CASE(FileInputTest) {
    BOOST_CHECK_THROW(Norma::FileInput("fakefile.txt"), std::runtime_error);

    input = new Norma::FileInput(std::string(TEST_BASE_DIR)
                               + "/fileinput.txt");
    output = new Norma::Output();
    input->initialize(c, output, tdata);
    output->initialize(c, input, tdata);

    // TODO(fpetran) check if lines are added to TrainingData
    BOOST_REQUIRE(input->thread_suitable());
    BOOST_CHECK_EQUAL(input->get_line(), "foo");
    BOOST_CHECK(tdata->empty());
    BOOST_CHECK_EQUAL(input->get_line(), "foobar");
    BOOST_CHECK(!input->request_train());
    BOOST_CHECK_EQUAL(input->get_line(), "bar\tbaz");
    BOOST_CHECK(input->request_train());
    BOOST_CHECK_EQUAL(input->get_line(), "anshelmus\tanselm");
    BOOST_CHECK(input->request_train());
    BOOST_CHECK_EQUAL(input->get_line(), "bla");
    // XXX if we seek after the end, it returns an empty string
    // maybe it should throw out_of_range or something
    BOOST_CHECK_EQUAL(input->get_line(), "");
    BOOST_CHECK(input->request_quit());
}

BOOST_AUTO_TEST_SUITE_END()

