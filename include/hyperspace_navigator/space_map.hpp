#ifndef HYPERSPACE_NAVIGATOR_SPACE_MAP_HPP
#define HYPERSPACE_NAVIGATOR_SPACE_MAP_HPP

#include <string>
#include <vector>
#include <queue>
#include <list>

/***
 * Library to navigate through N-Dimensional Hyperspace
 */
namespace hyperspace_navigator {

using SpaceIndex = std::vector<uint64_t>;

/***
 * This class defines the layout of our space
 */
class SpaceLayout
{
    std::vector<uint64_t> _dimensionSizes;

  private:
    SpaceLayout()
    {
    }

  public:
    /***
     * Constructs a SpaceLayout
     * @example SpaceLayout({3,3}) represents a 2d 3x3 space
     * @param dimensionSizes The number of cells for each dimension respectively
     */
    SpaceLayout(const std::vector<uint64_t>& dimensionSizes) : _dimensionSizes(dimensionSizes)
    {
    }

    /***
     * Number of dimension of this Layout
     * @return The number of dimensions
     */
    uint64_t numDimensions() const
    {
        return _dimensionSizes.size();
    }

    /***
     * The size in SpaceCells of the specified dimension
     * @param index Index of the dimension
     * @return The size in SpaceCells
     */
    uint64_t dimensionSize(uint64_t index) const
    {
        return _dimensionSizes[index];
    }

    /***
     * Given a dimension, give me the offset in the Space Flat Representation
     * @param dimension The dimension index
     * @return The offset of the related cell in space
     */
    uint64_t dimensionOffset(uint64_t dimension) const
    {
        uint64_t res = 1;
        if (dimension == 0)
        {
            return res;
        }
        uint64_t currDimension = dimension;
        while (currDimension > 0)
        {
            res *= dimensionSize(currDimension - 1);
            currDimension--;
        }
        return res;
    }

    /***
     * The size of the dimension under lastDimensionIndex considering the Flat Space representation
     * @param lastDimensionIndex The target index
     * @return The offset of the dimension just before lastDimensionIndex
     */
    uint64_t subDimensionLayoutSize(uint64_t lastDimensionIndex)
    {
        uint64_t size = 1;
        for (uint64_t i = 0; i < lastDimensionIndex; ++i)
        {
            size *= _dimensionSizes[i];
        }
        return size;
    }

    /***
     * The Size of the layout. Ex: a {3,3} layout size is 9
     * @return The size in cells of the layout.
     */
    uint64_t layoutSize()
    {
        uint64_t size = 1;
        std::for_each(_dimensionSizes.begin(), _dimensionSizes.end(), [&](uint64_t dim) { size *= dim; });
        return size;
    }

    /***
     * Builds an undefined layout
     * @return An undefined layout (See isUndefined())
     */
    static SpaceLayout undefined()
    {
        return {};
    }

    /***
     * Determines if this layout is undefined
     * @return True when the layout is undefined
     */
    bool isUndefined()
    {
        return _dimensionSizes.empty();
    }
};


/***
 * Representation of a Cell in the Space Map. We navigate through cells.
 */
class SpaceCell
{
  private:
    SpaceIndex _index;
    SpaceLayout _layout;

    explicit SpaceCell() : _layout(SpaceLayout::undefined())
    {
    }

  public:
    /***
     * Builds a SpaceCell given its index and its layout. Please use SpaceMap to build cells.
     * @param index Index of the cell
     * @param layout Space layout for this cell.
     */
    SpaceCell(SpaceIndex index, const SpaceLayout& layout) : _index(index), _layout(layout)
    {
    }

    /***
     * Builds a cell from spaceOffset. It calculates its indexes.
     * @param spaceOffset The offset in a flat space.
     * @param layout The space layout
     */
    SpaceCell(uint64_t spaceOffset, SpaceLayout& layout) : _layout(layout)
    {
        uint64_t currDimension = layout.numDimensions() - 1;
        _index = SpaceIndex(layout.numDimensions(), 0);
        while (currDimension > 0)
        {
            uint64_t subDimSize = layout.subDimensionLayoutSize(currDimension);
            _index[currDimension] = spaceOffset / subDimSize;
            currDimension--;
            if (currDimension == 0)
            {
                _index[currDimension] = spaceOffset % subDimSize;
            }
        }
    }

    /***
     * Calculates the offset in the FlatSpace of this Cell.
     * @return The offset in the FlatSpace representation
     */
    uint64_t spaceOffset() const
    {
        uint64_t res = 0;
        uint64_t currentDimension = 0;
        SpaceIndex::const_iterator it;
        for (it = _index.begin(); it < _index.end(); ++it)
        {
            res += *it * _layout.dimensionOffset(currentDimension++);
        }
        return res;
    }

    /***
     * Two cells are the same if they have the same offset
     * @param left First cell to compare
     * @param right Second cell to compare
     * @return True if the cells are the same
     */
    friend bool operator==(const SpaceCell& left, const SpaceCell& right)
    {
        return left.spaceOffset() == right.spaceOffset();
    }

    /***
     * Calculates the cells that are adjacent to the current cell. Adjacent cells have allways hamming distance of 1
     * @return A vector with all the adjacent cells.
     */
    std::vector<SpaceCell> getAdjacentCells() const
    {
        std::vector<SpaceCell> cellsList;
        // Hamming distance of 1
        for (uint64_t i = 0; i < _layout.numDimensions(); i++)
        {
            SpaceIndex cellIndex = _index;
            if (cellIndex[i] < _layout.dimensionSize(i) - 1)
            {
                cellIndex[i] = cellIndex[i] + 1;
                cellsList.emplace_back(cellIndex, _layout);
            }
        }
        return cellsList;
    }

    /***
     * The index of an specific dimension. Ex: [2,1] dimension 1 index here is 1.
     * @param dimension The dimension , starting with 0
     * @return The index of this cell for the dimension
     */
    uint64_t dimensionIndex(uint64_t dimension) const
    {
        return _index[dimension];
    }

    /***
     * The number of dimensions of this cell's layou
     * @return
     */
    uint64_t numDimensions() const
    {
        return _layout.numDimensions();
    }

    /***
     * Builds an undefined cell. See(isUndefined())
     * @return a cell that is undefined.
     */
    static SpaceCell undefined()
    {
        return SpaceCell();
    }

    /***
     * Determines if the cell is undefined
     * @return True if the cell is undefined
     */
    bool isUndefined()
    {
        return _layout.isUndefined();
    }

    /***
     * Determines if the cell is defined
     * @return True if the cell is defined
     */
    bool isDefined()
    {
        return !isUndefined();
    }

    /***
     * The index of this cell in the SpaceMap
     * @return a SpaceIndex
     */
    SpaceIndex index()
    {
        return _index;
    }
};

/***
 * Represents a Path to navigate. It contains a sorted list of SpaceCells.
 */
class NavigationPath
{
    std::list<SpaceCell> _cells;

  public:
    /***
     * Adds a SpaceCell to the path
     * @param cell the cell to add
     */
    void add(const SpaceCell& cell)
    {
        _cells.push_front(cell);
    }

    /***
     * Returns the cells ordered
     * @return A vector with all the cells in the Path
     */
    std::vector<SpaceCell> cells() const
    {
        return std::vector<SpaceCell>(_cells.begin(), _cells.end());
    }

    /***
     * A list of the path cell's indexes
     * @return A vector with all the indexes of the path
     */
    std::vector<SpaceIndex> indexes() const
    {
        std::vector<SpaceIndex> res;
        for (SpaceCell cell : cells())
        {
            res.push_back(cell.index());
        }
        return res;
    }

    /***
     * The number of cells for this path
     * @return The number of cells for the path
     */
    uint64_t numCells() const
    {
        return _cells.size();
    }
};

/***
 * Helper class to be used by PriorityQueue to store An Offset and its time.
 * It is a Plain Object.
 */
class OffsetAndTime
{
    uint64_t _offset;
    float _time;

  public:
    /***
     * Builds OffsetAndTime
     * @param offset The offset in the plain space
     * @param time Related time to this offset
     */
    OffsetAndTime(uint64_t offset, float time) : _offset(offset), _time(time)
    {
    }

    /***
     * Retrieves the offset
     * @return The offset
     */
    uint64_t getOffset() const { return _offset; }

    /***
     * Retrieves the time
     * @return The time
     */
    float getTime() const { return _time; }
};

/***
 * Helper class to be used by a priority Queue to compare different OffsetAndtime
 */
class offsetAndTimeComparator
{
  public:
    /***
     * Source is greater if it have a greater time
     * @param source Source operator
     * @param dest Destination operator
     * @return 1 if it is greater -1 if not
     */
    int operator()(const OffsetAndTime& source, const OffsetAndTime& dest)
    {
        return source.getTime() > dest.getTime();
    }
};


/***
 * The representation of a map of the entire space.
 */
class SpaceMap
{
  private:
    float* _space;
    SpaceLayout _layout;

  public:
    /***
     * Construct a SpaceMap given a space memory pointer and its layout
     * @param space Pointer to space representation
     * @param layout How is the space layed out (See: SpaceLayout)
     */
    SpaceMap(float* space, const SpaceLayout& layout) : _space(space), _layout(layout)
    {
    }

    /***
     * The first cell in the space
     * @return The starting point, the cell with the lowest index in the space.
     */
    SpaceCell spaceStart()
    {
        SpaceIndex startIndex;
        for (uint64_t i = 0; i < _layout.numDimensions(); ++i)
        {
            startIndex.push_back(0);
        }
        return cell(startIndex);
    }

    /***
     * The last cell in the space
     * @return The cell with the highest indexes in the space
     */
    SpaceCell spaceEnd()
    {
        SpaceIndex startIndex;
        for (uint64_t i = 0; i < _layout.numDimensions(); ++i)
        {
            startIndex.push_back(_layout.dimensionSize(i) - 1);
        }
        return cell(startIndex);
    }

    /***
     * Builds a cell for this map for the specified index
     * @param index The index in the map
     * @return SpaceCell with the map index and layou
     */
    SpaceCell cell(const SpaceIndex& index)
    {
        return SpaceCell(index, _layout);
    }

    /***
     * Builds a cell for this map with the specified offset in the plain space
     * @param offset Offset in the flat space representation using the layout of the map
     * @return SpaceCell
     */
    SpaceCell cell(uint64_t offset)
    {
        return SpaceCell(offset, _layout);
    }

    /***
     * Number of cells that this map contains
     * @return The number of the cells of the map
     */
    uint64_t numCells()
    {
        return _layout.layoutSize();
    }

    /***
     * Time to cross a specific cell
     * @param cell The cell
     * @return The time to cross the cell
     */
    float time(const SpaceCell& cell)
    {
        return spaceTime(cell);
    }

    /***
     * Time to cross the specific path
     * @param path The path
     * @return Time to cross all the cells in the path
     */
    float time(const NavigationPath& path)
    {
        const std::vector<SpaceCell>& pathCells = path.cells();
        std::vector<SpaceCell>::const_iterator it;
        float timeResult = 0;
        for (it = pathCells.begin(); it < pathCells.end(); ++it)
            timeResult += time(*it);

        return timeResult;
    }

    /***
     * Builds a navigation path given a list of indexes
     * @param indexes The indexes of the NavigationPath
     * @return NavigationPath with all the cells built
     */
    NavigationPath navigationPath(std::vector<SpaceIndex> indexes)
    {
        NavigationPath res;
        for (SpaceIndex index : indexes)
        {
            res.add(cell(index));
        }
        return res;
    }

    /***
     * Given a source and destination Cells it returns the fastest route to navigate from the source to the destination
     * @param fromCell Starting point SpaceCell
     * @param targetCell End point SpaceCell
     * @return A navigation path containing all the cells to navigate in order to follow the fastest route
     */
    NavigationPath fastestRoute(SpaceCell fromCell, SpaceCell targetCell)
    {
        std::vector<float> timeList(numCells(), std::numeric_limits<float>::max());
        std::vector<SpaceCell> previousCell(numCells(), SpaceCell::undefined());
        std::priority_queue<OffsetAndTime, std::vector<OffsetAndTime>, offsetAndTimeComparator> priorityQueue;

        priorityQueue.push(OffsetAndTime(fromCell.spaceOffset(), 0));
        timeList[fromCell.spaceOffset()] = 0;
        bool targetCellReached = false;
        while (!priorityQueue.empty() && !targetCellReached)
        {
            uint64_t visitedOffset = priorityQueue.top().getOffset();
            priorityQueue.pop();
            SpaceCell visitedCell = cell(visitedOffset);
            targetCellReached = visitedCell == targetCell;
            std::vector<SpaceCell> adjacentList = visitedCell.getAdjacentCells();
            std::vector<SpaceCell>::const_iterator it;
            for (it = adjacentList.begin(); it != adjacentList.end(); ++it)
            {
                uint64_t vOffset = it->spaceOffset();
                float newTime = timeList[visitedOffset] + spaceTime(*it);
                if (timeList[vOffset] > newTime)
                {
                    timeList[vOffset] = newTime;
                    priorityQueue.push(OffsetAndTime(vOffset, newTime));
                    previousCell[vOffset] = visitedCell;
                }
            }
        }
        NavigationPath path = NavigationPath();
        SpaceCell uCell = targetCell;
        if (previousCell[uCell.spaceOffset()].isDefined() || uCell == targetCell)
        {
            while (uCell.isDefined())
            {
                path.add(uCell);
                uCell = previousCell[uCell.spaceOffset()];
            }
        }

        return path;
    }

  private:
    float spaceTime(SpaceCell cell)
    {
        return _space[cell.spaceOffset()];
    }
};

} // namespace hyperspace_navigator

#endif