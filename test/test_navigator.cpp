#include <hyperspace_navigator.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

namespace hyperspace_navigator {

TEST_CASE("test_space_layout_2d_initialization")
{
    SpaceLayout layout = SpaceLayout({3, 2});
    uint64_t value = layout.numDimensions();
    REQUIRE(value == 2);
    REQUIRE(layout.dimensionSize(0) == 3);
    REQUIRE(layout.dimensionSize(1) == 2);
    REQUIRE(layout.layoutSize() == 3 * 2);
}

TEST_CASE("test_2d_space_map_creation")
{
    float space[3 * 3] = {0.F, 1.F, 2.F,
                          2.F, 3.F, 4.F,
                          5.F, 6.F, 7.F};
    SpaceMap map = SpaceMap(space, SpaceLayout({3, 3}));
    REQUIRE(map.time(map.cell({0, 0})) == Approx(0.F));
    REQUIRE(map.time(map.cell({2, 0})) == Approx(2.F));
    REQUIRE(map.time(map.cell({0, 2})) == Approx(5.F));
    REQUIRE(map.time(map.cell({2, 2})) == Approx(7.F));
}

TEST_CASE("test_create_cell_from_2d_space_offset")
{
    SpaceMap map = SpaceMap({}, SpaceLayout({3, 3}));
    SpaceCell cell = map.cell(0);
    REQUIRE(cell.dimensionIndex(0) == 0);
    REQUIRE(cell.dimensionIndex(1) == 0);
    REQUIRE(cell.numDimensions() == 2);

    cell = map.cell(2);
    REQUIRE(cell.dimensionIndex(0) == 2);
    REQUIRE(cell.dimensionIndex(1) == 0);
    REQUIRE(cell.numDimensions() == 2);

    cell = map.cell(4);
    REQUIRE(cell.dimensionIndex(0) == 1);
    REQUIRE(cell.dimensionIndex(1) == 1);
    REQUIRE(cell.numDimensions() == 2);

    cell = map.cell(8);
    REQUIRE(cell.dimensionIndex(0) == 2);
    REQUIRE(cell.dimensionIndex(1) == 2);
    REQUIRE(cell.numDimensions() == 2);
}

TEST_CASE("test_3d_space_map_creation")
{
    float space[2 * 2 * 2] = {0.F, 1.F,
                              2.F, 3.F,
                              4.F, 5.F,
                              6.F, 7.F};
    SpaceMap map = SpaceMap(space, SpaceLayout({2, 2, 2}));
    REQUIRE(map.time(map.cell({0, 0, 0})) == Approx(0));
    REQUIRE(map.time(map.cell({0, 0, 1})) == Approx(4.0f));
}

TEST_CASE("test_fastest_route_in_2d_space")
{
    float space[3 * 3] = {
        0.F, 1.F, 3.F,
        5.F, 2.F, 8.F,
        1.F, 5.F, 6.F};
    SpaceMap map = SpaceMap(space, SpaceLayout({3, 3}));
    NavigationPath navigationPath = map.fastestRoute(map.spaceStart(), map.spaceEnd());
    REQUIRE(navigationPath.numCells() == 5);
    float navigationTime = map.time(navigationPath);
    REQUIRE(navigationTime == Approx(14.F));
    std::vector<SpaceCell> pathCells = navigationPath.cells();
    REQUIRE(pathCells[0] == map.cell({0, 0}));
    REQUIRE(pathCells[1] == map.cell({1, 0}));
    REQUIRE(pathCells[2] == map.cell({1, 1}));
    REQUIRE(pathCells[3] == map.cell({1, 2}));
    REQUIRE(pathCells[4] == map.cell({2, 2}));
}

TEST_CASE("test_fastest_route_in_3d_space")
{
    float space[2 * 2 * 2] = {
        0.F, 1.F,
        2.F, 3.F,
        4.F, 5.F,
        6.F, 7.F};
    SpaceMap map = SpaceMap(space, SpaceLayout({2, 2, 2}));
    NavigationPath navigationPath = map.fastestRoute(map.spaceStart(), map.spaceEnd());
    REQUIRE(navigationPath.numCells() == 4);
    float navigationTime = map.time(navigationPath);
    REQUIRE(navigationTime == Approx(11.F));
    std::vector<SpaceCell> pathCells = navigationPath.cells();
    REQUIRE(pathCells[1] == map.cell({1, 0, 0}));
    REQUIRE(pathCells[2] == map.cell({1, 1, 0}));
    REQUIRE(pathCells[3] == map.cell({1, 1, 1}));
}

TEST_CASE("test_fastest_route_using_std_vector")
{
    std::vector<float> space={
        0.F, 1.F, 3.F,
        5.F, 2.F, 8.F,
        1.F, 5.F, 6.F };
    SpaceMap map = SpaceMap(space.data(), SpaceLayout({3, 3}));
    NavigationPath navigationPath = map.fastestRoute(map.spaceStart(), map.spaceEnd());
    REQUIRE(navigationPath.numCells() == 5);
    float navigationTime = map.time(navigationPath);
    REQUIRE(navigationTime == Approx(14.F));
}

TEST_CASE("test version")
{
    REQUIRE(HYPERSPACE_NAVIGATOR_VERSION_STRING == std::string("1.0.0"));
}
}//namespace hyperspace_navigator
