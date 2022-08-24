#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "gist.h"

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
    //const char* fname = "test.h";
    //const char* conts = "int x = 4;";
    //const char* desc  = "Test file with declared variable";
    //bool priv  = false;
    args.gist.filename  = (char*) "test.h";
    args.gist.conts     = (char*) "int x = 4;";
    args.gist.desc      = (char*) "Test file with declared variable";
    args.priv           = false;

    auto res = create_json(args);
    //nlohmann::json req;
    //req["description"] = desc;
    //req["public"] = !priv;
    //req["files"][fname]["content"] = conts;

    CHECK(res["files"]["test.h"]["content"] == "int x = 4;");
  }

}
