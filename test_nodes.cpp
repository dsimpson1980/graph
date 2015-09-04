#define CATCH_CONFIG_MAIN
#include "catch.h"
#include "nodes.h"

TEST_CASE("Testing Node Class") {
    Node<float> test_node(2, 3.0);
    Node<float> test_node2(2, 4.0);
    REQUIRE(test_node.timestamp == 2);
    REQUIRE(test_node.value == 3.0);

    SECTION("Addition of nodes") {
        Node<float> add_node = test_node + test_node2;

        REQUIRE(add_node.timestamp == 2);
        REQUIRE(add_node.value == 7.0);
    }
}

TEST_CASE("Testing NodeClipLower") {
    Node<float> test_node(2, 3.0);
    NodeClipLower<float> test_node_lower(test_node, 4.0);
    test_node_lower.evaluate();
    REQUIRE(test_node_lower.timestamp == 2);
    REQUIRE(test_node_lower.value == 4.0);
};

TEST_CASE("Testing NodeClipUpper") {
    Node<float> test_node(2, 3.0);
    NodeClipUpper<float> test_node_upper(test_node, 2.0);
    test_node_upper.evaluate();
    REQUIRE(test_node_upper.timestamp == 2);
    REQUIRE(test_node_upper.value == 2.0);
}

TEST_CASE("Testing NodeClip") {
    Node<float> test_node(2, 3.0);

    SECTION("Check lower") {
        NodeClip<float> test_node_clip(test_node, 4.0, 6.0);
        test_node_clip.evaluate();
        REQUIRE(test_node_clip.timestamp == 2);
        REQUIRE(test_node_clip.value == 4.0);
    }

    SECTION("Check upper") {
        NodeClip<float> test_node_clip(test_node, 1.0, 2.0);
        test_node_clip.evaluate();
        REQUIRE(test_node_clip.timestamp == 2);
        REQUIRE(test_node_clip.value == 2.0);
    }

}

TEST_CASE("Testing NodeAbs") {
    Node<float> test_node(2, float(-1.0));

    NodeAbs<float> test_node_abs(test_node);
    test_node_abs.evaluate();
    REQUIRE(test_node_abs.timestamp == 2);
    REQUIRE(test_node_abs.value == 1.0);
}