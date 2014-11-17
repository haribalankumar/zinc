/**
 * FILE : datastore/map.hpp
 * 
 * Implements a semi-dense multidimensional array addressed by sets
 * of labels.
 */
/* OpenCMISS-Zinc Library
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <vector>
#include "datastore/labels.hpp"
#include "datastore/mapindexing.hpp"
#include "general/block_array.hpp"
#include "general/message.h"

class DsMapBase : public cmzn::RefCounted
{
protected:
	std::string name; // optional
	bool dense;
	int labelsArraySize;
	DsLabels **labelsArray;

	DsMapBase(int labelsArraySizeIn, DsLabels **labelsArrayIn);
	DsMapBase(const DsMapBase&); // not implemented
	virtual ~DsMapBase();
	DsMapBase& operator=(const DsMapBase&); // not implemented

	static bool checkLabelsArrays(int labelsArraySizeIn, DsLabels **labelsArrayIn);

public:
	DsMapIndexing *createIndexing();

	std::string getName() const
	{
		return this->name;
	}

	void setName(const std::string& nameIn)
	{
		this->name = nameIn;
	}

	int getLabelsArraySize() const
	{
		return this->labelsArraySize;
	}

	/**
	 * @param labelsNumber  Index from 0 to labelsArraySize-1
	 * @return  Accessed pointer to DsLabels or 0 if invalid number or error
	 */
	DsLabels* getLabels(int labelsNumber) const
	{
		if ((0 <= labelsNumber) && (labelsNumber < this->labelsArraySize))
			return cmzn::ACCESS(this->labelsArray[labelsNumber]);
		return 0;
	}
};

typedef cmzn::RefHandle<DsMapBase> HDsMapBase;

template <typename ValueType>
class DsMap : public DsMapBase
{
	// number of indexes allocated per DsLabels; can be more or less than DsLabels::indexSize 
	DsLabelIndex *indexSizes;
	// memory offset for a unit of each index labels:
	// equal to product of following indexSizes; largest for first, 1 for last
	DsMapAddressType *offsets;
	// values indexed by indexSizes[i] over labelsArray
	block_array<DsMapAddressType, ValueType> values;
	// for non-dense, flag indexed as for values which is true if value exists, false if not
	bool_array<DsMapAddressType> value_exists;

	DsMap(int labelsArraySizeIn, DsLabels **labelsArrayIn);
	virtual ~DsMap();
	DsMapAddressType getMaxDenseSize() const;
	bool setNotDense();
	bool copyValues(int labelsNumber, DsMapAddressType oldBaseIndex,
		DsMapAddressType newBaseIndex, DsMapAddressType *newOffsets,
		DsLabelIndex *copySize, block_array<DsMapAddressType, ValueType>& dest_values,
		bool_array<DsMapAddressType>& dest_value_exists) const;
	bool resize(DsLabelIndex *newIndexSizes);
	int validIndexCount(DsMapIndexing& indexing, DsMapAddressType number_of_values,
		const char *methodName) const;

public:

	static DsMap<ValueType> *create(int labelsArraySizeIn, DsLabels **labelsArrayIn);
	static DsMap<ValueType> *create(std::vector<HDsLabels>& labelsVector);

	int getValues(DsMapIndexing& indexing, DsMapAddressType number_of_values, ValueType *outValues) const;

	int getValuesSparse(DsMapIndexing& indexing, DsMapAddressType number_of_values, ValueType *outValues,
		int *valueExists, int& valuesRead) const;

	int setValues(DsMapIndexing& indexing, DsMapAddressType number_of_values, ValueType *inValues);

	bool isDenseAndCompleteOnLabels(int labelsNumber) const;
};

template <typename ValueType>
DsMap<ValueType>::DsMap(int labelsArraySizeIn, DsLabels **labelsArrayIn) :
	DsMapBase(labelsArraySizeIn, labelsArrayIn),
	indexSizes(new DsLabelIndex[labelsArraySizeIn]),
	offsets(new DsMapAddressType[labelsArraySizeIn])
{
	for (int i = 0; i < labelsArraySize; i++)
	{
		this->indexSizes[i] = 0;
		// default offset = 1 sets up single DsLabels indexing. Multiple DsLabels indexing will call resize
		this->offsets[i] = 1;
	}
}

template <typename ValueType>
DsMap<ValueType>::~DsMap()
{
	delete[] this->indexSizes;
	delete[] this->offsets;
}

/** @return  number of values that would be stored if map were dense */
template <typename ValueType>
DsMapAddressType DsMap<ValueType>::getMaxDenseSize() const
{
	DsMapAddressType maxDenseSize = 1;
	for (int i = 0; i < labelsArraySize; i++)
	{
		maxDenseSize *= this->indexSizes[i];
	}
	return maxDenseSize;
}

/** Set value_exists flags to true for all values in map.
 * Must call prior to inserting values non-densely.
 * @return  true on success, false if failed / status remains dense.
 */
template <typename ValueType>
bool DsMap<ValueType>::setNotDense()
{
	if (!dense)
		return true;
	//display_message(INFORMATION_MESSAGE, "In DsMap::setNotDense  Map %s %d\n", this->name.c_str(), getMaxDenseSize());
	dense = false;
	if (value_exists.setAllTrue(getMaxDenseSize()))
		return true;
	// failed: restore dense status and return false
	display_message(ERROR_MESSAGE,
		"DsMap::setNotDense.  Failed to convert %s to non-dense",
		this->name.c_str());
	value_exists.clear();
	dense = true;
	return false;
}

/** Recursively copies values from internal arrays to dest arrays
 * @return  true on complete success, false otherwise.
 */
template <typename ValueType>
bool DsMap<ValueType>::copyValues(int labelsNumber,
	DsMapAddressType oldBaseIndex,
	DsMapAddressType newBaseIndex,
	DsMapAddressType *newOffsets,
	DsLabelIndex *copySize,
	block_array<DsMapAddressType, ValueType>& dest_values,
	bool_array<DsMapAddressType>& dest_value_exists) const
{
	DsLabelIndex thisCopySize = copySize[labelsNumber];
	DsMapAddressType oldIndex = oldBaseIndex;
	DsMapAddressType newIndex = newBaseIndex;
	DsLabelIndex index;
	if (labelsNumber == (labelsArraySize - 1))
	{
		ValueType value;
		for (index = 0; index < thisCopySize; index++)
		{
			if (!dense && !value_exists.getBool(oldIndex))
				continue;
			if (!values.getValue(oldIndex, value))
			{
				display_message(ERROR_MESSAGE, "DsMap::copyValues  Map %s is missing a value\n",
					this->name.c_str());
				return false;
			}
			if (!dest_values.setValue(newIndex, value))
				return false;
			bool oldValue;
			if (!dense && !dest_value_exists.setBool(oldIndex, true, oldValue))
				return false;
      newIndex++;
      oldIndex++;
		}
	}
	else
	{
		for (index = 0; index < thisCopySize; index++)
		{
			if (!copyValues(labelsNumber + 1, oldIndex, newIndex,
				newOffsets, copySize, dest_values, dest_value_exists))
				return false;
			oldIndex += offsets[labelsNumber];
			newIndex += newOffsets[labelsNumber];
		}
	}
	return true;
}

/** Resizes existing values array. Very expensive for large maps.
 * Atomic.
 * GRC: Can be made more efficient.
 * @param newIndexSizes  array of new sizes for each DsLabels in the labelsArray
 * @return  true on success, false on failure - with arrays unchanged
 */
template <typename ValueType>
bool DsMap<ValueType>::resize(DsLabelIndex *newIndexSizes)
{
#if defined (DEBUG_CODE)
	display_message(INFORMATION_MESSAGE, "DsMap::resize  %s\n", this->name.c_str());
	for (int i = 0; i < labelsArraySize; i++)
	{
		display_message(INFORMATION_MESSAGE, "    Labels %s: %u -> %u\n",
			this->labelsArray[i]->getName().c_str(), this->indexSizes[i], newIndexSizes[i]);
	}
#endif /* defined (DEBUG_CODE) */

	// values to copy is minimum of this->indexSizes and newIndexSizes
	DsLabelIndex *copySize = new DsLabelIndex[labelsArraySize];
	DsMapAddressType *newOffsets = new DsMapAddressType[labelsArraySize];
	if ((!copySize) || (!newOffsets))
		return false;
	for (int i = labelsArraySize - 1; 0 <= i; i--)
	{
		copySize[i] = this->indexSizes[i];
		if (newIndexSizes[i] < copySize[i])
			copySize[i] = newIndexSizes[i];
		if (i == (labelsArraySize - 1))
			newOffsets[i] = 1;
		else
			newOffsets[i] = newOffsets[i+1]*newIndexSizes[i+1];
	}

	// no copy if currently empty, but need to set this->indexSizes and offsets
	if (getMaxDenseSize() > 0)
	{
		block_array<DsMapAddressType, ValueType> dest_values;
		bool_array<DsMapAddressType> dest_value_exists;
		if (!copyValues(/*labelsNumber*/0, /*oldBaseIndex*/0, /*newBaseIndex*/0,
			newOffsets, copySize, dest_values, dest_value_exists))
		{
			display_message(WARNING_MESSAGE,
				"DsMap::resize  Not enough memory to resize map %s\n",
				this->name.c_str());
			delete[] copySize;
			delete[] newOffsets;
			return false;
		}
		values.swap(dest_values);
		value_exists.swap(dest_value_exists);
	}
	for (int i = 0; i < labelsArraySize; i++)
	{
		this->indexSizes[i] = newIndexSizes[i];
		offsets[i] = newOffsets[i];
	}
	delete[] copySize;
	delete[] newOffsets;
	return true;
}

template <typename ValueType>
inline int DsMap<ValueType>::validIndexCount(DsMapIndexing& indexing, DsMapAddressType number_of_values,
	const char *methodName) const
{
	if (!indexing.indexesMap(this))
	{
		display_message(ERROR_MESSAGE, "%s.  Invalid indexing for map %s",
			methodName, this->name.c_str());
		return 0;
	}
	DsMapAddressType index_entry_count = indexing.getEntryCount();
	if (0 == index_entry_count)
	{
		display_message(ERROR_MESSAGE, "%s.  Invalid indexing specifies zero values.",
			methodName, this->name.c_str());
		return 0;
	}
	if (number_of_values != index_entry_count)
	{
		display_message(ERROR_MESSAGE,
			"%s.  Index specifies %u values, %u supplied for map %s.",
			methodName, index_entry_count, number_of_values, this->name.c_str());
		return 0;
	}
	return 1;
}

template <typename ValueType>
DsMap<ValueType> *DsMap<ValueType>::create(int labelsArraySizeIn, DsLabels **labelsArrayIn)
{
	if (!DsMapBase::checkLabelsArrays(labelsArraySizeIn, labelsArrayIn))
		return 0;
	return new DsMap<ValueType>(labelsArraySizeIn, labelsArrayIn);
}

template <typename ValueType>
DsMap<ValueType> *DsMap<ValueType>::create(std::vector<HDsLabels>& labelsVector)
{
	int labelsArraySize = static_cast<int>(labelsVector.size());
	DsLabels **labelsArray = new DsLabels*[labelsArraySize];
	if (!labelsArray)
		return 0;
	for (int i = 0; i < labelsArraySize; ++i)
		labelsArray[i] = labelsVector[i].getObject();
	DsMap<ValueType> *map = DsMap<ValueType>::create(labelsArraySize, labelsArray);
	delete[] labelsArray;
	return map;
}

template <typename ValueType>
int DsMap<ValueType>::getValues(DsMapIndexing& indexing,
	unsigned int number_of_values, ValueType *outValues) const
{
	if ((number_of_values == 0) || !(outValues))
		return 0;
	if (!validIndexCount(indexing, number_of_values, "DsMap::getValues"))
		return 0;

	// iterate to get values in identifier order for each indexing DsLabels
	if (!indexing.iterationBegin())
	{
		display_message(ERROR_MESSAGE,
			"DsMap::getValues  Failed to begin iteration over indexing for map %s\n",
			this->name.c_str());
		return 0;
	}
	int i;
	int return_code = 1;
	DsMapAddressType valueIndex = 0;
	unsigned int value_number;
	DsLabelIndex index;
	bool iterResult = true;
	for (value_number = 0; iterResult && (value_number < number_of_values); value_number++)
	{
		valueIndex = 0;
		for (i = 0; i < labelsArraySize; i++)
		{
			index = indexing.getIterationIndex(i);
			if (index >= this->indexSizes[i])
			{
				return_code = 0;
				break;
			}
			valueIndex += index*offsets[i];
		}
		if (!return_code)
			break;
		if (!dense && !value_exists.getBool(valueIndex))
		{
#if 0 // DEBUG_CODE
			display_message(ERROR_MESSAGE,
				"DsMap::getValues  Value doesn't exist at index %d", valueIndex);
			for (int i = 0; i < labelsArraySize; i++)
			{
				index = indexing.getIterationIndex(i);
				display_message(INFORMATION_MESSAGE, "Labels %d %s : index %d indexSizes = %d (#values %d)\n",
					i, this->labelsArray[i]->getName().c_str(), index, this->indexSizes[i], number_of_values);
			}
			if (labelsArraySize == 2)
			{
				for (int i = 0; i < this->indexSizes[0]; i++)
				{
					display_message(INFORMATION_MESSAGE, "%3d :", i);
					int ix = i*offsets[0];
					for (int j = 0; j < this->indexSizes[1]; j++)
					{
						ix += offsets[1];
						display_message(INFORMATION_MESSAGE, " %d", (int)value_exists.getBool(ix));
					}
					display_message(INFORMATION_MESSAGE, "\n");
				}
			}
#endif
			return_code = 0;
			break;
		}
		if (!values.getValue(valueIndex, outValues[value_number]))
		{
			return_code = 0;
			break;
		}
		iterResult = indexing.iterationNext();
	}
	indexing.iterationEnd();

	if (return_code && (value_number < number_of_values))
	{
		display_message(ERROR_MESSAGE,
			"DsMap::getValues  Only %u out of %u values iterated for map %s\n",
			value_number, number_of_values, this->name.c_str());
		return_code = 0;
	}
	else if (iterResult && (value_number >= number_of_values))
	{
		display_message(ERROR_MESSAGE,
			"DsMap::getValues  Iteration past end of values for map %s\n",
			this->name.c_str());
		return_code = 0;
	}
	return return_code;
}

template <typename ValueType>
int DsMap<ValueType>::getValuesSparse(DsMapIndexing& indexing,
	unsigned int number_of_values, ValueType *outValues, int *valueExists, int& valuesRead) const
{
	if ((number_of_values == 0) || !(outValues) || !(valueExists))
		return 0;
	if (!validIndexCount(indexing, number_of_values, "DsMap::getValuesSparse"))
		return 0;

	// iterate to get values in identifier order for each indexing DsLabels
	if (!indexing.iterationBegin())
	{
		display_message(ERROR_MESSAGE,
			"DsMap::getValuesSparse  Failed to begin iteration over indexing for map %s\n",
			this->name.c_str());
		return 0;
	}
	int i;
	valuesRead = 0;
	int return_code = 1;
	DsMapAddressType valueIndex = 0;
	unsigned int value_number;
	DsLabelIndex index;
	bool iterResult = true;
	for (value_number = 0; iterResult && (value_number < number_of_values); value_number++)
	{
		valueIndex = 0;
		for (i = 0; i < labelsArraySize; i++)
		{
			index = indexing.getIterationIndex(i);
			if (index >= this->indexSizes[i])
			{
				return_code = 0;
				break;
			}
			valueIndex += indexing.getIterationIndex(i)*offsets[i];
		}
		if (!return_code)
			break;
		if (!dense && !value_exists.getBool(valueIndex))
		{
			valueExists[value_number] = 0;
		}
		else if (values.getValue(valueIndex, outValues[value_number]))
		{
			valueExists[value_number] = 1;
			++valuesRead;
		}
		else
		{
			return_code = 0;
			break;
		}
		iterResult = indexing.iterationNext();
	}
	indexing.iterationEnd();

	if (return_code && (value_number < number_of_values))
	{
		display_message(ERROR_MESSAGE,
			"DsMap::getValuesSparse  Only %u out of %u values iterated for map %s\n",
			value_number, number_of_values, this->name.c_str());
		return_code = 0;
	}
	else if (iterResult && (value_number >= number_of_values))
	{
		display_message(ERROR_MESSAGE,
			"DsMap::getValuesSparse  Iteration past end of values for map %s\n",
			this->name.c_str());
		return_code = 0;
	}
	return return_code;
}

// unsigned int may eventually be too small for number_of_values.
template <typename ValueType>
int DsMap<ValueType>::setValues(DsMapIndexing& indexing,
	unsigned int number_of_values, ValueType *inValues)
{
	if ((number_of_values == 0) || !(inValues))
		return 0;
	if (!validIndexCount(indexing, number_of_values, "DsMap::setValues"))
		return 0;

	int i, j;
	indexing.calculateIndexLimits();
	bool resizeInner = false;
	bool clearDense = false;
	for (i = 0; i < labelsArraySize; i++)
	{
		if (indexing.getIndexLimit(i) > this->indexSizes[i])
		{
			if (i > 0)
			{
				resizeInner = true;
			}
			if (dense && !clearDense)
			{
				if (indexing.isDenseOnLabels(i))
				{
					for (j = 0; j < labelsArraySize; j++)
					{
						if ((i != j) && ((indexing.getIndexLimit(j) < this->indexSizes[j]) ||
							(!indexing.isDenseOnLabels(j))))
						{
							clearDense = true;
							break;
						}
					}
				}
				else if (indexing.isDenseOnLabelsAbove(i, this->indexSizes[i] - 1))
				{
					for (j = 0; j < labelsArraySize; j++)
					{
						if ((i != j) && ((indexing.getIndexLimit(j) != this->indexSizes[j]) ||
							(!indexing.isDenseOnLabels(j))))
						{
							clearDense = true;
							break;
						}
					}					
				}
				else
				{
					clearDense = true;
				}
			}
		}
	}

	if (clearDense)
	{
		if (!setNotDense())
			return 0;
	}

	if (resizeInner)
	{
		DsLabelIndex *newIndexSizes = new DsLabelIndex[labelsArraySize];
		if (!newIndexSizes)
			return 0;
		for (i = 0; i < labelsArraySize; i++)
		{
			newIndexSizes[i] = this->indexSizes[i];
			// first DsLabels: never resize since it can freely grow
			// inner DsLabels: resize to MAX(this->indexSizes, indexLimit)
			// GRC in some cases it may be better to go straight to DsLabels::indexSize
			if ((i > 0) && (indexing.getIndexLimit(i) > newIndexSizes[i]))
				newIndexSizes[i] = indexing.getIndexLimit(i);
		}
		if (!resize(newIndexSizes))
			return 0;
		delete[] newIndexSizes;
	}
	if (0 < labelsArraySize)
	{
		DsLabelIndex newIndexSize0 = indexing.getIndexLimit(0);
		if (newIndexSize0 > this->indexSizes[0])
			this->indexSizes[0] = newIndexSize0;
	}

	// iterate to set values in identifier order for each indexing DsLabels
	DsMapAddressType valueIndex = 0;
	unsigned int value_number;

	if (!indexing.iterationBegin())
	{
		display_message(ERROR_MESSAGE,
			"DsMap::setValues  Failed to begin iteration over indexing for map %s\n",
			this->name.c_str());
		return 0;
	}
	bool oldValue;
	int return_code = 1;
	bool iterResult = true;
	for (value_number = 0; iterResult && (value_number < number_of_values); value_number++)
	{
		valueIndex = 0;
		for (i = 0; i < labelsArraySize; i++)
		{
			valueIndex += indexing.getIterationIndex(i)*offsets[i];
		}
		if (!values.setValue(valueIndex, inValues[value_number]))
		{
			display_message(ERROR_MESSAGE,
				"DsMap::setValues  Failed to set value in map %s\n",
				this->name.c_str());
			return_code = 0;
			break;
		}
		if (!dense)
		{
			if (!value_exists.setBool(valueIndex, true, oldValue))
			{
				display_message(ERROR_MESSAGE,
					"DsMap::setValues  Failed to set value_exists flag int map %s\n",
					this->name.c_str());
				return_code = 0;
				break;
			}
		}
		iterResult = indexing.iterationNext();
	}
	indexing.iterationEnd();

	if (return_code && (value_number < number_of_values))
	{
		display_message(ERROR_MESSAGE,
			"DsMap::setValues  Only %u out of %u values iterated for map %s\n",
			value_number, number_of_values, this->name.c_str());
		return_code = 0;
	}
	else if (iterResult && (value_number >= number_of_values))
	{
		display_message(ERROR_MESSAGE,
			"DsMap::setValues  Iteration past end of values for map %s\n",
			this->name.c_str());
		return_code = 0;
	}
	// FUTURE_CODE: propagate change message
	return return_code;
}

template <typename ValueType>
bool DsMap<ValueType>::isDenseAndCompleteOnLabels(int labelsNumber) const
{
	HDsLabels labels (this->getLabels(labelsNumber));
	if (labels)
	{
		if (this->dense && (this->indexSizes[labelsNumber] == labels->getSize()))
			return true;
		// GRC to complete!
	}
	return false;
}

typedef cmzn::RefHandle< DsMap< int > > HDsMapInt;
typedef cmzn::RefHandle< DsMap< double > > HDsMapDouble;
