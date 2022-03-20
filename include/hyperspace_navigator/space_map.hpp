#ifndef HYPERSPACE_NAVIGATOR_SPACE_MAP_HPP
#define HYPERSPACE_NAVIGATOR_SPACE_MAP_HPP

#include <string>
#include <vector>
#include <queue>
#include <list>

//TODO:
// * Documentation

namespace hyperspace_navigator {

using SpaceIndex = std::vector<uint64_t>;

class SpaceLayout {
    std::vector<uint64_t> _dimensionSizes;

  private:
    SpaceLayout() {
    }

  public:
    SpaceLayout(const std::vector<uint64_t>& dimensionSizes) : _dimensionSizes(dimensionSizes) {
    }

    uint64_t numDimensions() const {
        return _dimensionSizes.size();
    }

    uint64_t dimensionSize(uint64_t index) const {
        return _dimensionSizes[index];
    }

    uint64_t dimensionOffset(uint64_t dimension) const{
        uint64_t res = 1;
        if (dimension == 0) {
            return res;
        }
        uint64_t currDimension = dimension;
        while (currDimension > 0) {
            res*=dimensionSize(currDimension-1);
            currDimension--;
        }
        return res;
    }

    uint64_t subDimensionLayoutSize(uint64_t lastDimensionIndex) {
        uint64_t size = 1;
        for (uint64_t i = 0; i < lastDimensionIndex; ++i) {
            size*= _dimensionSizes[i];
        }
        return size;
    }

    uint64_t layoutSize() {
        uint64_t size = 1;
        std::for_each(_dimensionSizes.begin(), _dimensionSizes.end(), [&](uint64_t dim) {size*= dim;});
        return size;
    }

    static SpaceLayout undefined() {
        return {};
    }

    bool isUndefined() {
        return _dimensionSizes.empty();
    }


};

class SpaceCell {
  private:
    SpaceIndex _index;
    SpaceLayout _layout;

    explicit SpaceCell() : _layout(SpaceLayout::undefined()) {
    }


  public:
    SpaceCell(SpaceIndex index, const SpaceLayout& layout) : _index(index), _layout(layout) {
    }

    SpaceCell(uint64_t spaceOffset,  SpaceLayout& layout) : _layout(layout) {
        uint64_t currDimension = layout.numDimensions() -1;
        _index = SpaceIndex(layout.numDimensions(), 0);
        while (currDimension > 0) {
            uint64_t subDimSize = layout.subDimensionLayoutSize(currDimension);
            _index[currDimension] = spaceOffset / subDimSize;
            currDimension--;
            if (currDimension == 0) {
                _index[currDimension] = spaceOffset % subDimSize;
            }
        }
    }

    uint64_t spaceOffset() const {
        uint64_t res = 0;
        uint64_t currentDimension = 0;
        SpaceIndex::const_iterator it;
        for (it = _index.begin(); it < _index.end(); ++it){
            res += *it * _layout.dimensionOffset(currentDimension++);
        }
        return res;
    }

    friend bool operator==(const SpaceCell& left, const SpaceCell& right) {
        return left.spaceOffset() == right.spaceOffset();
    }

    std::vector<SpaceCell> getAdjacentCells() const {
        std::vector<SpaceCell> cellsList;
        //Hamming distance of 1
        for (uint64_t i = 0; i < _layout.numDimensions(); i++) {
            SpaceIndex cellIndex = _index;
            if (cellIndex[i] < _layout.dimensionSize(i) -1)
            {
                cellIndex[i] = cellIndex[i] + 1;
                cellsList.emplace_back(cellIndex, _layout);
            }
        }
        return cellsList;
    }

    uint64_t dimensionIndex(uint64_t dimension) const {
        return _index[dimension];
    }

    uint64_t numDimensions() const {
        return _layout.numDimensions();
    }

    static SpaceCell undefined() {
        return SpaceCell();
    }

    bool isUndefined() {
        return _layout.isUndefined();
    }

    bool isDefined() {
        return !isUndefined();
    }

    SpaceIndex index() {
        return _index;
    }
};


class NavigationPath {
    std::list<SpaceCell> _cells;

  public:
    void add(const SpaceCell& cell) {
        _cells.push_front(cell);
    }

    std::vector<SpaceCell> cells() const{
        return std::vector<SpaceCell>(_cells.begin(), _cells.end());
    }

    std::vector<SpaceIndex> indexes() const{
        std::vector<SpaceIndex> res;
        for (SpaceCell cell: cells()) {
            res.push_back(cell.index());
        }
        return res;
    }

    uint64_t numCells() const {
        return _cells.size();
    }

};

class OffsetAndTime
{
    uint64_t _offset;
    float _time;
  public:
    OffsetAndTime(uint64_t offset, float time) : _offset(offset), _time(time) {
    }
    uint64_t getOffset() const { return _offset; }
    float getTime() const { return _time; }
};

class offsetAndTimeComparator
{
  public:
    int operator() (const OffsetAndTime& source, const OffsetAndTime& dest) {
        return source.getTime() > dest.getTime();
    }
};

class SpaceMap {
  private:
    float* _space;
    SpaceLayout _layout;

  public:
    SpaceMap(float* space, const SpaceLayout& layout) : _space(space), _layout(layout) {
    }

    SpaceCell spaceStart() {
        SpaceIndex startIndex;
        for (uint64_t i = 0; i < _layout.numDimensions(); ++i)
        {
            startIndex.push_back(0);
        }
        return cell(startIndex);
    }

    SpaceCell spaceEnd() {
        SpaceIndex startIndex;
        for (uint64_t i = 0; i < _layout.numDimensions(); ++i)
        {
            startIndex.push_back(_layout.dimensionSize(i) -1);
        }
        return cell(startIndex);
    }

    SpaceCell cell(const SpaceIndex& index) {
        return SpaceCell(index, _layout);
    }

    SpaceCell cell(uint64_t offset) {
        return SpaceCell(offset, _layout);
    }

    uint64_t numCells() {
        return _layout.layoutSize();
    }

    float time(const SpaceCell& cell) {
        return spaceTime(cell);
    }

    float time(const NavigationPath& path) {
        const std::vector<SpaceCell>& pathCells = path.cells();
        std::vector<SpaceCell>::const_iterator  it;
        float timeResult = 0;
        for (it=pathCells.begin(); it < pathCells.end(); ++it)
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
        for (SpaceIndex index: indexes ) {
            res.add(cell(index));
        }
        return res;
    }

    NavigationPath fastestRoute(SpaceCell fromCell, SpaceCell targetCell) {
        std::vector<float> timeList(numCells(), std::numeric_limits<float>::max());
        std::vector<SpaceCell> previousCell(numCells(), SpaceCell::undefined() );
        std::priority_queue<OffsetAndTime, std::vector<OffsetAndTime>, offsetAndTimeComparator> priorityQueue;

        priorityQueue.push(OffsetAndTime(fromCell.spaceOffset(), 0));
        timeList[fromCell.spaceOffset()] = 0;
        bool targetCellReached = false;
        while(!priorityQueue.empty() && !targetCellReached ) {
            uint64_t visitedOffset = priorityQueue.top().getOffset();
            priorityQueue.pop();
            SpaceCell visitedCell = cell(visitedOffset);
            targetCellReached = visitedCell == targetCell;
            std::vector<SpaceCell> adjacentList = visitedCell.getAdjacentCells();
            std::vector<SpaceCell>::const_iterator it;
            for (it = adjacentList.begin(); it != adjacentList.end(); ++it){
                uint64_t vOffset = it->spaceOffset();
                float newTime = timeList[visitedOffset] + spaceTime(*it);
                if (timeList[vOffset] > newTime) {
                    timeList[vOffset] = newTime;
                    priorityQueue.push(OffsetAndTime(vOffset, newTime));
                    previousCell[vOffset] = visitedCell;
                }
            }
        }
        NavigationPath path = NavigationPath();
        SpaceCell uCell = targetCell;
        if (previousCell[uCell.spaceOffset()].isDefined() || uCell == targetCell){
            while (uCell.isDefined()) {
                path.add(uCell);
                uCell = previousCell[uCell.spaceOffset()];
            }
        }

        return path;
    }

  private:
    float spaceTime(SpaceCell cell) {
        return _space[cell.spaceOffset()];
    }

};

}

#endif