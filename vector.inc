#if defined _vector_included
    #endinput
#endif
#define _vector_included

#include <a_samp>
#include <YSI_Data\y_iterate>
#include <YSI_Coding\y_inline>
#include <YSI_Coding\y_malloc>

#define VEC_DEFAULT_GROWTH		(16)

enum e_VEC_DATA()
{
	e_VEC_DATA_LEN,
	//First 30 bits are for storing the growth the next 2 bits are for storing flags
	e_VEC_DATA_ATTRIBUTES
}

enum e_VEC_FLAGS (<<= 2)
{
	e_VEC_FLAGS_IS_ORDERED = 1,
	e_VEC_FLAGS_IS_READ_ONLY
}
static const VEC_FLAGS_COUNT = 2;

enum E_VEC_ERROR()
{
	E_VEC_ERROR_OK,
	E_VEC_ERROR_NO_ALLOC,
	E_VEC_ERROR_READ_ONLY,
	E_VEC_ERROR_NOT_FOUND,
	E_VEC_ERROR_INVALID_INDEX
}

iterfunc stock Vector(&iterstate, cur, Vec:vector, start = 0, end = -1)
{
	new len = Vec_Len(vector);
	if(end == -1 || end > len)
	{
		end = len;
	}
	if(start < 0)
	{
		start = 0;
	}
	if(cur == cellmin)
	{
		iterstate = start-1;
	}
	if(iterstate++ >= end-1)
	{
		return cellmin;
	}
	return Vec_Get(vector, iterstate);
}
#define iterstart@Vector iterstate(cellmin, 0)

#define Vec_GetAsArray(%0,%1)			Malloc_GetData(Alloc:%0,(%1)+_:e_VEC_DATA)
#define Vec_GetString(%0)				Malloc_GetData(Alloc:%0,_:e_VEC_DATA)

#define INVALID_VECTOR			(Vec:NO_ALLOC)

stock bool:Vec_IsOrdered(Vec:vector)
{
	return bool:(Malloc_Get(Alloc:vector, e_VEC_DATA_ATTRIBUTES) & _:e_VEC_FLAGS_IS_ORDERED);
}

stock Vec_SetOrdered(Vec:vector, bool:set)
{
	new attr = Malloc_Get(Alloc:vector, e_VEC_DATA_ATTRIBUTES);
	if(set)
	{
		Malloc_Set(Alloc:vector, e_VEC_DATA_ATTRIBUTES, attr | _:e_VEC_FLAGS_IS_ORDERED);
	}
	else
	{
		Malloc_Set(Alloc:vector, e_VEC_DATA_ATTRIBUTES, attr & ~_:e_VEC_FLAGS_IS_ORDERED);
	}
}

stock bool:Vec_IsReadOnly(Vec:vector)
{
	return bool:(Malloc_Get(Alloc:vector, e_VEC_DATA_ATTRIBUTES) & _:e_VEC_FLAGS_IS_READ_ONLY);
}

stock Vec_SetReadOnly(Vec:vector, bool:set)
{
	new attr = Malloc_Get(Alloc:vector, e_VEC_DATA_ATTRIBUTES);
	if(set)
	{
		Malloc_Set(Alloc:vector, e_VEC_DATA_ATTRIBUTES, attr | _:e_VEC_FLAGS_IS_READ_ONLY);
	}
	else
	{
		Malloc_Set(Alloc:vector, e_VEC_DATA_ATTRIBUTES, attr & ~_:e_VEC_FLAGS_IS_READ_ONLY);
	}
}

stock Vec_Growth(Vec:vector)
{
	return Malloc_Get(Alloc:vector, e_VEC_DATA_ATTRIBUTES) >> VEC_FLAGS_COUNT;
}

stock Vec_SetGrowth(Vec:vector, growth)
{
	new flags = Malloc_Get(Alloc:vector, e_VEC_DATA_ATTRIBUTES) / 4;
	Malloc_Set(Alloc:vector, e_VEC_DATA_ATTRIBUTES, growth << VEC_FLAGS_COUNT | flags);
}

stock Vec:Vec_New(capacity, bool:isOrdered = false, growth = VEC_DEFAULT_GROWTH)
{
	new Vec:newVec = Vec:calloc(capacity + _:e_VEC_DATA);
	if(newVec == INVALID_VECTOR)
		return INVALID_VECTOR;
	Vec_SetGrowth(newVec, growth);
	Vec_SetOrdered(newVec, isOrdered);
	return Vec:newVec;
}

stock Vec:Vec_NewFromArray(capacity, const arr[], size = sizeof arr, bool:isOrdered = false, growth = VEC_DEFAULT_GROWTH)
{
	new Vec:vector = Vec_New(capacity >= size ? capacity : size, isOrdered, growth);
	if(vector == INVALID_VECTOR)
		return INVALID_VECTOR;
	memcpy(Vec_GetAsArray(vector, 0), arr, 0, size * 4, size);
	Vec_SetLen(vector, size);
	return vector;
}

stock Vec:Vec_NewString(const str[], growth = VEC_DEFAULT_GROWTH)
{
	new len = strlen(str);
	new Vec:string = Vec_NewFromArray(len + growth, str, len, true, growth);
	if(string == INVALID_VECTOR)
		return INVALID_VECTOR;
	//Make sure the null terminator is deleted next time we append.
	Vec_SetLen(string, len);
	return string;
}

stock Vec_Len(Vec:vector)
{
	return Malloc_Get(Alloc:vector, e_VEC_DATA_LEN);
}

stock E_VEC_ERROR:Vec_ReSize(& Vec:vector, newSize)
{
	new Alloc:newVector = realloc(Alloc:vector, newSize, true);
	if(newVector == NO_ALLOC)
		return E_VEC_ERROR_NO_ALLOC;

	vector = Vec:newVector;
	return E_VEC_ERROR_OK;
}

stock Vec_Capacity(Vec:vector)
{
	return Malloc_GetSlotSize(Alloc:vector) - _:e_VEC_DATA;
}

static stock Vec_SetLen(Vec:vector, len)
{
	Malloc_Set(Alloc:vector, e_VEC_DATA_LEN, len);
}

stock E_VEC_ERROR:Vec_Append(&Vec:vector, value)
{
	if(Vec_IsReadOnly(vector))
		return E_VEC_ERROR_READ_ONLY;
	new size =  Vec_Capacity(vector);
	if(size <= Vec_Len(vector))
	{
		if(Vec_ReSize(vector, size + Vec_Growth(vector) + _:e_VEC_DATA + 1) != E_VEC_ERROR_OK)
			return E_VEC_ERROR_NO_ALLOC;
	}
	new len = Vec_Len(vector);
	Malloc_Set(Alloc:vector,  len + _:e_VEC_DATA, value);
	Malloc_Set(Alloc:vector, e_VEC_DATA_LEN, len + 1);
	return E_VEC_ERROR_OK;
}

stock E_VEC_ERROR:Vec_AppendChar(&Vec:vector, value)
{
	new E_VEC_ERROR:err = Vec_Append(vector, value);
	if(err != E_VEC_ERROR_OK)
		return err;
	//make sure to add a null terminator
	err = Vec_Append(vector, '\0');
	if(err != E_VEC_ERROR_OK)
		return err;
	//make sure the null terminator gets deleted next time we append
	Vec_SetLen(vector, Vec_Len(vector) - 1);
	return E_VEC_ERROR_OK;
}

stock E_VEC_ERROR:Vec_Set(&Vec:vector, index, value)
{
	if(Vec_IsReadOnly(vector))
		return E_VEC_ERROR_READ_ONLY;
	if(index >= Vec_Capacity(vector))
	{
		if(Vec_ReSize(vector, index + Vec_Growth(vector) + _:e_VEC_DATA + 1) != E_VEC_ERROR_OK)
			return E_VEC_ERROR_NO_ALLOC;
	}

	Malloc_Set(Alloc:vector, _:e_VEC_DATA + index, value);
	if(index >= Vec_Len(vector))
		Malloc_Set(Alloc:vector, e_VEC_DATA_LEN, index+1);

	return E_VEC_ERROR_OK;
}

stock E_VEC_ERROR:Vec_RemoveAt(Vec:vector, index)
{
	if(Vec_IsReadOnly(vector))
		return E_VEC_ERROR_READ_ONLY;
	new len = Vec_Len(vector);
	if(!Vec_IsOrdered(vector))
	{
		Vec_Set(vector, index, Vec_Get(vector, len - 1));
		Vec_Set(vector, len, 0);
		Vec_SetLen(vector, len - 1);
	}
	else
	{
		for(new i = index; i < len - 1; ++i)
		{
			Vec_Set(vector, i, Vec_Get(vector, i+1));
		}
		Vec_Set(vector, len - 1, 0);
		Vec_SetLen(vector, len - 1);
	}
	return E_VEC_ERROR_OK;
}

stock E_VEC_ERROR:Vec_RemoveFirstBy(Vec:vector, Func:predicate<i>)
{
	if(Vec_IsReadOnly(vector))
		return E_VEC_ERROR_READ_ONLY;
	for(new i = 0; i < Vec_Len(vector); ++i)
	{
		if(@.predicate(Vec_Get(vector, i)))
		{
			Vec_RemoveAt(vector, i);
			return E_VEC_ERROR_OK;
		}
	}
	return E_VEC_ERROR_NOT_FOUND;
}

stock E_VEC_ERROR:Vec_RemoveFirstElement(Vec:vector, value)
{
	inline matchValue(val)
	{
		inline_return val == value;
	}
	return Vec_RemoveFirstBy(vector, using inline matchValue);
}

stock E_VEC_ERROR:Vec_RemoveLastBy(Vec:vector, Func:predicate<i>)
{
	if(Vec_IsReadOnly(vector))
		return E_VEC_ERROR_READ_ONLY;

	for(new i = Vec_Len(vector) - 1; i >= 0; --i)
	{
		if(@.predicate(Vec_Get(vector, i)))
		{
			Vec_RemoveAt(vector, i);
			return E_VEC_ERROR_OK;
		}
	}
	return E_VEC_ERROR_NOT_FOUND;
}

stock E_VEC_ERROR:Vec_RemoveLastElement(Vec:vector, value)
{
	inline matchValue(val)
	{
		inline_return val == value;
	}

	return Vec_RemoveLastBy(vector, using inline matchValue);
}

stock E_VEC_ERROR:Vec_RemoveAllBy(Vec:vector, Func:predicate<i>)
{
	if(Vec_IsReadOnly(vector))
		return E_VEC_ERROR_READ_ONLY;

	new found = false;
	for(new i = 0; i < Vec_Len(vector); ++i)
	{
		if(@.predicate(Vec_Get(vector, i)))
		{
			Vec_RemoveAt(vector, i);
			found = true;
		}
	}
	return found ? E_VEC_ERROR_OK : E_VEC_ERROR_NOT_FOUND;
}

stock E_VEC_ERROR:Vec_RemoveAllElements(Vec:vector, value)
{
	inline matchValue(val)
	{
		inline_return val == value;
	}
	return Vec_RemoveAllBy(vector, using inline matchValue);
}

stock Vec_Get(Vec:vector, index)
{
	if(index >= Vec_Len(vector))
		return 0;

	return Malloc_Get(Alloc:vector, _:e_VEC_DATA + index);
}

stock bool:Vec_ContainsBy(Vec:vector, Func:predicate<i>)
{
	for(new i = 0, len = Vec_Len(vector); i < len; ++i)
	{
		if(@.predicate(Vec_Get(vector, i)))
			return true;
	}
	return false;
}

stock bool:Vec_Contains(Vec:vector, value)
{
	inline matchValue(val)
	{
		inline_return val == value;
	}
	return Vec_ContainsBy(vector, using inline matchValue);
}

stock E_VEC_ERROR:Vec_Clear(Vec:vector)
{
	if(Vec_IsReadOnly(vector))
		return E_VEC_ERROR_READ_ONLY;
	MemSet(Vec_GetAsArray(vector, 0), 0, Vec_Len(vector));
	Vec_SetLen(vector, 0);
	return E_VEC_ERROR_OK;
}

stock Vec_FindFirstBy(Vec:vector, Func:predicate<i>)
{
	for(new i = 0, len = Vec_Len(vector); i < len; ++i)
	{
		if(@.predicate(Vec_Get(vector, i)))
			return i;
	}
	return -1;
}

stock Vec_FindFirst(Vec:vector, value)
{
	inline matchValue(val)
	{
		inline_return val == value;
	}
	return Vec_FindFirstBy(vector, using inline matchValue);
}

stock Vec_FindLastBy(Vec:vector, Func:predicate<i>)
{
	for(new i = Vec_Len(vector) - 1; i >= 0; --i)
	{
		if(@.predicate(Vec_Get(vector, i)))
			return i;
	}
	return -1;
}

stock Vec_FindLastElement(Vec:vector, value)
{
	inline matchValue(val)
	{
		inline_return val == value;
	}
	return Vec_FindLastBy(vector, using inline matchValue);
}

stock Vec:Vec_FindAllBy(Vec:vector, Func:predicate<i>)
{
	new Vec:foundIndexes = Vec_New(10);
	for(new i = 0, len = Vec_Len(vector); i < len; ++i)
	{
		if(@.predicate(Vec_Get(vector, i)))
		{
			Vec_Append(foundIndexes, i);
		}
	}
	if(Vec_Len(foundIndexes) == 0)
	{
		Vec_Delete(foundIndexes);
		foundIndexes = INVALID_VECTOR;
	}
	return foundIndexes;
}

stock Vec:Vec_FindAllElements(Vec:vector, value)
{
	inline matchValue(val)
	{
		inline_return val == value;
	}
	return Vec_FindAllBy(vector, using inline matchValue);
}

stock bool:Vec_TrueForAll(Vec:vector, Func:predicate<i>)
{
	foreach(new val : Vector(vector))
	{
		if(!@.predicate(val))
		{
			return false;
		}
	}
	return true;
}

stock Vec_BinarySearchBy(Vec:vector, Func:predicate<i>)
{
	new first = 0, last = Vec_Len(vector) - 1, mid, check;

	while(first < last)
	{
		if(!@.predicate(Vec_Get(vector, first)))
			return first;
		if(!@.predicate(Vec_Get(vector, last)))
			return last;

		mid = (first + last)/2;
		check = @.predicate(Vec_Get(vector, mid));

		if(!check)
			return mid;
		else if(check == -1)
		{
			first = mid+1;
			--last;
		}
		else
		{
			last = mid-1;
			++first;
		}
	}
	return -1;
}

stock Vec_BinarySearch(Vec:vector, value)
{
	inline compareValues(val)
	{
		if(val == value)
		{
			inline_return 0;
		}
		else if(val < value)
		{
			inline_return -1;
		}
		else
		{
			inline_return 1;
		}
	}
	return Vec_BinarySearchBy(vector, using inline compareValues);
}

stock E_VEC_ERROR:Vec_AppendArray(&Vec:vector, const arr[], size = sizeof arr)
{
	if(Vec_IsReadOnly(vector))
		return E_VEC_ERROR_READ_ONLY;
	new len = Vec_Len(vector);
	new requiredSize = len + size;
	if(requiredSize >= Vec_Capacity(vector))
	{
		if(Vec_ReSize(vector, requiredSize + Vec_Growth(vector)) != E_VEC_ERROR_OK)
			return E_VEC_ERROR_NO_ALLOC;
	}

	memcpy(Vec_GetAsArray(vector, len), arr, 0, size * 4, size);
	Vec_SetLen(vector, len + size);
	return E_VEC_ERROR_OK;
}

stock E_VEC_ERROR:Vec_AppendString(&Vec:vector, const str[], len = -1)
{
	if(Vec_IsReadOnly(vector))
		return E_VEC_ERROR_READ_ONLY;
	if(len <= 0)
		len = strlen(str);

	//len + 1 so that it adds the null terminator
	new E_VEC_ERROR:err = Vec_AppendArray(vector, str, len + 1);
	if(err != E_VEC_ERROR_OK)
		return err;
	//make sure the null terminator is deleted next time we append
	Vec_SetLen(vector, Vec_Len(vector) - 1);
	return E_VEC_ERROR_OK;
}

stock E_VEC_ERROR:Vec_SetArray(&Vec:vector, const arr[], index ,size = sizeof arr)
{
	if(Vec_IsReadOnly(vector))
		return E_VEC_ERROR_READ_ONLY;
	new requiredSize = index + size;
	if(requiredSize >= Vec_Capacity(vector))
	{
		if(Vec_ReSize(vector, requiredSize + Vec_Growth(vector)) != E_VEC_ERROR_OK)
			return E_VEC_ERROR_NO_ALLOC;
	}

	memcpy(Vec_GetAsArray(vector, index), arr, 0, size * 4, size);
	new len = Vec_Len(vector);
	Vec_SetLen(vector, len >= requiredSize ? len : requiredSize);
	return E_VEC_ERROR_OK;
}

stock E_VEC_ERROR:Vec_AppendVector(&Vec:vector, Vec:toAppend, start = 0, end = 0)
{
	if(Vec_IsReadOnly(vector))
		return E_VEC_ERROR_READ_ONLY;
	if(end <= 0 || end > Vec_Len(toAppend))
		end = Vec_Len(toAppend);
	return Vec_AppendArray(vector, Vec_GetAsArray(toAppend, start), end-start);
}

stock E_VEC_ERROR:Vec_Swap(Vec:vector, index1, index2)
{
	if(Vec_IsReadOnly(vector))
		return E_VEC_ERROR_READ_ONLY;
	new len = Vec_Len(vector);
	if(index1 >= len || index2 >= len)
		return E_VEC_ERROR_INVALID_INDEX;

	new tmp = Vec_Get(vector, index1);
	Vec_Set(vector, index1, Vec_Get(vector, index2));
	Vec_Set(vector, index2, tmp);
	return E_VEC_ERROR_OK;
}

stock E_VEC_ERROR:Vec_Reverse(Vec:vector, start = 0, end = 0)
{
	if(Vec_IsReadOnly(vector))
		return E_VEC_ERROR_READ_ONLY;
	new len = Vec_Len(vector);
	if(end == 0 || end > len)
		end = len - 1;
	if(start > len)
		return E_VEC_ERROR_INVALID_INDEX;
	if(start == end)
		return E_VEC_ERROR_INVALID_INDEX;

	for(new i = start, j = end; i < j; ++i, --j)
	{
		Vec_Swap(vector, i, j);
	}
	return E_VEC_ERROR_OK;
}

stock Vec_CopyTo(Vec:vector, dest[], start = 0, end = 0, size = sizeof dest)
{
	if(end == 0 || end > size)
	{
		end = size;
	}

	memcpy(dest, Vec_GetAsArray(vector, start), 0, (end - start)*4, end);
}

static stock Vec_QSPartition(Vec:unsorted, Func:Comparer<ii>, start, end)
{
    new pivot = Vec_Get(unsorted, end);
    new i = start - 1;
    for(new j = start; j <= end-1; j++)
	{
        if(@.Comparer(Vec_Get(unsorted, j), pivot))
		{
            i++;
            Vec_Swap(unsorted, i, j);
        }
    }
    Vec_Swap(unsorted, i+1, end);
    return i+1;
}

stock E_VEC_ERROR:Vec_SortAt(Vec:unsorted, Func:Comparer<ii>, start = 0, end)
{
    if(Vec_IsReadOnly(unsorted))
		return E_VEC_ERROR_READ_ONLY;

    if(start < end)
	{
        new pi = Vec_QSPartition(unsorted, Comparer, start, end);
        Vec_SortAt(unsorted, Comparer, start, pi-1);
        Vec_SortAt(unsorted, Comparer, pi+1, end);
        return E_VEC_ERROR_OK;
    }
    return E_VEC_ERROR_INVALID_INDEX;
}

stock E_VEC_ERROR:Vec_Sort(Vec:unsorted, start = 0, end = 0) {
    inline defaultComparer(value1, value2)
	{
        inline_return value1 <= value2 ? true : false;
    }
	if(end == 0)
	{
		end = Vec_Len(unsorted) - 1;
	}
    return Vec_SortAt(unsorted, using inline defaultComparer, start, end);
}

stock Vec:Vec_Clone(Vec:toClone, bool:readOnly = false)
{
	new Vec:clonedVector = Vec_New(Vec_Capacity(toClone), Vec_IsOrdered(toClone), Vec_Growth(toClone));
	if(clonedVector == INVALID_VECTOR)
		return INVALID_VECTOR;
	Vec_AppendVector(clonedVector, toClone);
	Vec_SetReadOnly(clonedVector, readOnly);
	return clonedVector;
}

stock Vec_Delete(&Vec:vector)
{
	new res = Malloc_Free(Alloc:vector);
	vector = INVALID_VECTOR;
	return res;
}
