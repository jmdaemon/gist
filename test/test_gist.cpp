#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "gist.h"

#include <fmt/core.h>
#include <file.h>

// Constants
static const std::string gist_file = "./samples/gists.p1.json";

// Helper Functions
void test_date(nlohmann::json json_res, std::string date, std::string date_type, RELTIME reltime, int size) {
    std::vector<nlohmann::json> results;
    for (auto gist: json_res)
      results.push_back(search_date(gist, date, date_type, reltime));
    results = filter_null(results);
    fmt::print("{}\n", results.size());
    show_results(results, true);
    CHECK(results.size() == size); // We should get the right number of results
}

// Sanity checks
TEST_CASE("JSON objects can be properly formatted") { 
  SUBCASE("C++ strings are formatted properly") {
    std::string fname = "test.h";
    std::string conts = "int x = 4;";
    std::string desc  = "Test file with declared variable";
    bool priv  = false;
    auto res = create_json(fname, conts, desc, priv);
    CHECK(res["files"][fname]["content"] == "int x = 4;");
  }
  SUBCASE("C strings are formatted properly") {
    arguments args;
    args.gist.filename  = (char*) "test.h";
    args.gist.conts     = (char*) "int x = 4;";
    args.gist.desc      = (char*) "Test file with declared variable";
    args.priv           = false;

    auto res = create_json(args);
    CHECK(res["files"]["test.h"]["content"] == "int x = 4;");
  }
}

TEST_CASE("search_date() filters the correct dates") {
  const char* file = read_file(gist_file.c_str());
  auto gists(file);
  auto date = "2022-08-23T21:37:26Z";
  auto json_res = nlohmann::json::parse(gists);
  auto date_type = "created_at";

  SUBCASE("Filter exact matches only") {
    fmt::print("Exact matches\n");
    test_date(json_res, date, date_type, EXACT, 1);
  }
  SUBCASE("Filter only later dates") {
    fmt::print("Matches after date\n");
    test_date(json_res, date, date_type, AFTER, 0);
  }
  SUBCASE("Filter only earlier dates") {
    fmt::print("Matches before date\n");
    test_date(json_res, date, date_type, BEFORE, 29);
  }
  free((void*) file);
}
