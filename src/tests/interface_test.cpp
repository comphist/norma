#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Interface
#include<iostream>
#include<sstream>
#include<boost/test/included/unit_test.hpp>  // NOLINT[build/include_order]
#include"cycle.h"
#include"training_data.h"
#include"interface/input.h"
#include"interface/output.h"
#include"interface/iobase.h"
#include<cstdio>

#define BOOST_CHECK_STDOUT(stdwrap, X)   \
    BOOST_CHECK_EQUAL(stdwrap.get_content(), X); stdwrap.flush()

class OStreamWrapper {
  public:
      OStreamWrapper() { save(); }
      ~OStreamWrapper() { load(); }
      std::string get_content() {
          // get content of mock stream and clear
          std::string result, line;
          while (getline(mock_stream, line))
              result += line + "\n";
          //getline(mock_stream, result, '\n');
          mock_stream.str("");
          mock_stream.clear();
          return result;
      }
      void flush() {
          // dump stream contents to stdout and clear, then re-route to mock
          // stream. it needs to be flushed after each assert, otherwise
          // the assert output can't be displayed
          load();
          std::cout << buffer;
          save();
      }
  private:
      std::streambuf* buffer;
      std::stringstream mock_stream;
      /// save cout to buffer and redirect to mock_stream
      void save() { buffer = std::cout.rdbuf(mock_stream.rdbuf()); }
      /// restore cout from buffer
      void load() { buffer = std::cout.rdbuf(buffer); }
};

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

    OStreamWrapper cout_wrapper;
    IStreamWrapper cin_wrapper;

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

BOOST_AUTO_TEST_CASE(NormalOutputTest) {
    // test normal (non-interactive output)
    // it doesn't do much except print the result with
    // or without probability
    // TODO check log level?
    output = new Norma::Output();
    output->initialize(c, input, tdata);

    BOOST_REQUIRE(output->thread_suitable());
    cout_wrapper.flush();

    Norma::Normalizer::Result res("foo", 0.5);

    output->put_line(&res, false, Norma::Normalizer::LogLevel::SILENT);
    BOOST_CHECK_STDOUT(cout_wrapper, "foo\n");

    output->put_line(&res, true, Norma::Normalizer::LogLevel::SILENT);
    BOOST_CHECK_STDOUT(cout_wrapper, "foo\t0.5\n");
}

BOOST_AUTO_TEST_CASE(InteractiveOutputTest) {
    input = new Norma::ShellInput();
    input->initialize(c, output, tdata);
    // if we want to test functions that Input doesn't have
    // we can't do that via a pointer to base
    Norma::InteractiveOutput *o = new Norma::InteractiveOutput();
    output = o;
    output->initialize(c, input, tdata);
    BOOST_REQUIRE(!output->thread_suitable());
    cout_wrapper.flush();

    Norma::Normalizer::Result res1("foo", 0.5);
    Norma::Normalizer::Result res2("baz", 0.5);

    // test correction in validation
    cin_wrapper.put_line("bar\n");
    std::string validated = o->validate("foo");
    BOOST_CHECK_EQUAL(validated, "bar");
    cout_wrapper.flush();

    // test confirmation in validation
    cin_wrapper.put_line("\n");
    validated = o->validate("foo");
    BOOST_CHECK_EQUAL(validated, "foo");
    cout_wrapper.flush();

    // test entire put_line method
    auto training_pair = tdata->begin();
    cin_wrapper.put_line("\n");
    output->put_line(&res1, true, Norma::Normalizer::LogLevel::SILENT);
    BOOST_CHECK_STDOUT(cout_wrapper, "foo\t0.5\n? \n");
    BOOST_CHECK_EQUAL(training_pair->target(), "foo");
    cout_wrapper.flush();

    // XXX check training_pair->source() ?
    cin_wrapper.put_line("bar\n");
    output->put_line(&res2, false, Norma::Normalizer::LogLevel::SILENT);
    ++training_pair;
    BOOST_CHECK_STDOUT(cout_wrapper, "baz\n? \n");
    BOOST_CHECK_EQUAL(training_pair->target(), "bar");
    cout_wrapper.flush();
}

BOOST_AUTO_TEST_SUITE_END()
