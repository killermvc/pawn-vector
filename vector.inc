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
	E_VEC_ERROR_NO_ALLOC
}

stock Vec:Vec_New(capacity, growth = VEC_DEFAULT_GROWTH, bool:isOrdered = false)
{
	new Alloc:newVec = calloc(capacity + 3);
	Malloc_Set(newVec, e_VEC_DATA_GROWTH, growth);
	Malloc_Set(newVec, e_VEC_DATA_IS_ORDERED, _:isOrdered);
	return Vec:newVec;
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
	memset(Malloc_GetData(Alloc:vector, _:e_VEC_DATA), 0, Vec_Len(vector));
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

stock Vec_Delete(Vec:vector)
{
	return Malloc_Free(Alloc:vector);
}