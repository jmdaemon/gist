#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "gist.h"

TEST_CASE("JSON objects can be properly formatted") { 
  std::string fname = "test.h";
  std::string conts = "int x = 4;";
  std::string desc  = "Test file with declared variable";
  bool priv  = false;
  auto res = create_json(fname, conts, desc, priv);
  CHECK(res["files"][fname]["content"] == "int x = 4;");
}
