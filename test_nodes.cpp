#define CATCH_CONFIG_MAIN
#include "catch.h"
#include "nodes.h"

TEST_CASE("Testing Node Class") {
    Node<float> test_node(2, 3.0);
    REQUIRE(test_node.timestamp == 2);
    REQUIRE(test_node.value == 3.0);

};