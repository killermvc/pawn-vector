#if defined _inc_vector
    #undef _inc_vector
#endif

#if defined _vector_included
    #endinput
#endif
#define _vector_included

#include <a_samp>
#include <YSI_Coding\y_malloc>

#define VEC_DEFAULT_GROWTH		(16)

enum e_VEC_DATA()
{
	e_VEC_DATA_LEN,
	e_VEC_DATA_GROWTH,
	e_VEC_DATA_IS_ORDERED
}

enum E_VEC_ERROR()
{
	E_VEC_ERROR_OK,
	E_VEC_ERROR_NO_ALLOC,
	E_VEC_ERROR_INVALID_INDEX
}

#define Vec_GetAsArray(%0,%1)			Malloc_GetData(Alloc:%0,(%1)+_:e_VEC_DATA)
#define VEC_FOREACH(new%0:%1) for(new i__=0,len__=Vec_Len(%1),%0=Vec_Get(%1,0);i__<len__;++i__,%0=Vec_Get(%1,i__))

stock Vec:Vec_New(capacity, bool:isOrdered = false, growth = VEC_DEFAULT_GROWTH)
{
	new Alloc:newVec = calloc(capacity + 3);
	Malloc_Set(newVec, e_VEC_DATA_GROWTH, growth);
	Malloc_Set(newVec, e_VEC_DATA_IS_ORDERED, _:isOrdered);
	return Vec:newVec;
}

stock Vec:Vec_NewFromArray(capacity, const arr[], size = sizeof arr, bool:isOrdered = false, growth = VEC_DEFAULT_GROWTH)
{
	new Vec:vector = Vec_New(capacity >= size ? capacity : size, isOrdered, growth);
	memcpy(Vec_GetAsArray(vector, 0), arr, 0, size * 4, size);
	Vec_SetLen(vector, size);
	return vector;
}

stock Vec_Len(Vec:vector)
{
	return Malloc_Get(Alloc:vector, e_VEC_DATA_LEN);
}

stock bool:Vec_IsOrdered(Vec:vector)
{
	return bool:Malloc_Get(Alloc:vector, e_VEC_DATA_IS_ORDERED);
}

stock Vec_Growth(Vec:vector)
{
	return Malloc_Get(Alloc:vector, e_VEC_DATA_GROWTH);
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
	new size =  Vec_Capacity(vector);
	if(size <= Vec_Len(vector))
	{
		if(Vec_ReSize(vector, size + Vec_Growth(vector)) != E_VEC_ERROR_OK)
			return E_VEC_ERROR_NO_ALLOC;
	}
	new len = Vec_Len(vector);
	Malloc_Set(Alloc:vector,  len + _:e_VEC_DATA, value);
	Malloc_Set(Alloc:vector, e_VEC_DATA_LEN, len + 1);
	return E_VEC_ERROR_OK;
}

stock E_VEC_ERROR:Vec_Set(&Vec:vector, index, value)
{
	if(index >= Vec_Capacity(vector))
	{
		if(Vec_ReSize(vector, index + Vec_Growth(vector)) != E_VEC_ERROR_OK)
			return E_VEC_ERROR_NO_ALLOC;
	}

	Malloc_Set(Alloc:vector, _:e_VEC_DATA + index, value);
	if(index >= Vec_Len(vector))
		Malloc_Set(Alloc:vector, e_VEC_DATA_LEN, index+1);

	return E_VEC_ERROR_OK;
}

stock Vec_RemoveAt(Vec:vector, index)
{
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
}

stock Vec_RemoveFirstElement(Vec:vector, value)
{
	for(new i = 0; i < Vec_Len(vector); ++i)
	{
		if(Vec_Get(vector, i) == value)
		{
			Vec_RemoveAt(Vec:vector, i);
			return i;
		}
	}
	return -1;
}

stock Vec_Get(Vec:vector, index)
{
	if(index >= Vec_Len(vector))
		return 0;

	return Malloc_Get(Alloc:vector, _:e_VEC_DATA + index);
}

stock bool:Vec_Contains(Vec:vector, value)
{
	for(new i = 0, len = Vec_Len(vector); i < len; ++i)
	{
		if(Vec_Get(vector, i) == value)
			return true;
	}
	return false;
}

stock Vec_Clear(Vec:vector)
{
	memset(Vec_GetAsArray(vector, 0), 0, Vec_Len(vector));
	Vec_SetLen(vector, 0);
}

stock Vec_FindFirst(Vec:vector, value)
{
	for(new i = 0, len = Vec_Len(vector); i < len; ++i)
	{
		if(Vec_Get(vector, i) == value)
			return i;
	}
	return -1;
}

stock E_VEC_ERROR:Vec_AppendArray(Vec:vector, const arr[], size = sizeof arr)
{
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

stock E_VEC_ERROR:Vec_SetArray(Vec:vector, const arr[], index ,size = sizeof arr)
{
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

stock E_VEC_ERROR:Vec_AppendVector(Vec:vector, Vec:toAppend, start = 0, end = 0)
{
	if(end <= 0 || end > Vec_Len(toAppend))
		end = Vec_Len(toAppend);
	return Vec_AppendArray(vector, Vec_GetAsArray(toAppend, start), end-start);
}

stock E_VEC_ERROR:Vec_Swap(Vec:vector, index1, index2)
{
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
		end = size;

	memcpy(dest, Vec_GetAsArray(vector, start), 0, (end - start)*4, end);
}

stock Vec_Delete(Vec:vector)
{
	return Malloc_Free(Alloc:vector);
}