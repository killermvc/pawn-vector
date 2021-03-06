#define RUN_TESTS
#define YSI_NO_HEAP_MALLOC
#define YSI_NO_CACHE_MESSAGE
#define YSI_NO_OPTIMISATION_MESSAGE
#define YSI_NO_VERSION_CHECK

#include "vector.inc"
#include <YSI_Core\y_testing>

Test:New()
{
	new Vec:vec = Vec_New(25, true, 28);

	//y_malloc allocates multiples of 16 cells. so the capacity allocated is 32.
	//However y_malloc uses one of this cells to store the capacity
	//and pawn-vector uses 4 more to store the length and the attributes
	//thus the reported capacity is 29.
	ASSERT_NE(vec, INVALID_VECTOR);
	ASSERT_EQ(Vec_Capacity(vec), 29);
	ASSERT_EQ(Vec_Len(vec), 0);
	ASSERT_EQ(Vec_Growth(vec), 28);
	ASSERT(Vec_IsOrdered(vec));

	Vec_Delete(vec);
}

Test:Append()
{
	new Vec:vec = Vec_New(10);

	Vec_Append(vec, 16);
	Vec_Append(vec, 17);
	Vec_Append(vec, 18);

	ASSERT_EQ(Vec_Len(vec), 3);
	ASSERT_EQ(Malloc_Get(Alloc:vec, _:e_VEC_DATA), 16);
	ASSERT_EQ(Malloc_Get(Alloc:vec, _:e_VEC_DATA + 1), 17);
	ASSERT_EQ(Malloc_Get(Alloc:vec, _:e_VEC_DATA + 2), 18);

	Vec_Delete(vec);
}

Test:AppendResize()
{
	new Vec:vec = Vec_New(10, false, 10);

	for(new i = 0; i < 14; ++i)
	{
		Vec_Append(vec, i+1);
	}

	//y_malloc allocates multiples of 16 cells. when creating the vector, the capacity allocated is 16,
	//after the resize is 32. However y_malloc uses one of this cells to store the capacity
	//and pawn-vector uses 4 more to store the length and the attributes
	//thus the reported capacity is 29.
	ASSERT_GE(Vec_Capacity(vec), 10 + 10);
	for(new i = 0; i < 14; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), i+1);
	}
	Vec_Delete(vec);
}

Test:Get()
{
	new Vec:vec = Vec_New(10);

	Vec_Append(vec, 16);
	Vec_Append(vec, 17);
	Vec_Append(vec, 18);

	ASSERT_EQ(Vec_Get(vec, 0), 16);
	ASSERT_EQ(Vec_Get(vec, 1), 17);
	ASSERT_EQ(Vec_Get(vec, 2), 18);

	Vec_Delete(vec);
}

Test:Set()
{
	new Vec:vec = Vec_New(10);

	Vec_Set(vec, 5, 7);

	ASSERT_EQ(Vec_Len(vec), 6);
	ASSERT_EQ(Vec_Get(vec, 5), 7);

	Vec_Delete(vec);
}

Test:SetPastCapacity()
{
	new Vec:vec = Vec_New(10);
	Vec_Set(vec, 14, 7);
	ASSERT_EQ(Vec_Len(vec), 15);
	ASSERT_EQ(Vec_Get(vec, 14), 7);
	ASSERT_GE(Vec_Capacity(vec), 14 + VEC_DEFAULT_GROWTH );

	Vec_Delete(vec);
}

Test:RemoveAtUnordered()
{
	new
		Vec:vec = Vec_New(10),
		expected[] = {1, 2, 5, 4, 0};
	Vec_Append(vec, 1);
	Vec_Append(vec, 2);
	Vec_Append(vec, 3);
	Vec_Append(vec, 4);
	Vec_Append(vec, 5);

	Vec_RemoveAt(vec, 2);

	for(new i = 0; i < sizeof expected; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
	}
}

Test:RemoveAtOrdered()
{
	new
		Vec:vec = Vec_New(10, true),
		expected[] = {1, 2, 4, 5, 0};
	Vec_Append(vec, 1);
	Vec_Append(vec, 2);
	Vec_Append(vec, 3);
	Vec_Append(vec, 4);
	Vec_Append(vec, 5);

	Vec_RemoveAt(vec, 2);

	for(new i = 0; i < sizeof expected; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
	}
}

Test:RemoveFirstElementUnordered()
{
	new
		Vec:vec = Vec_New(10),
		expected[] = {1, 5, 3, 4, 0};
	Vec_Append(vec, 1);
	Vec_Append(vec, 2);
	Vec_Append(vec, 3);
	Vec_Append(vec, 4);
	Vec_Append(vec, 5);

	Vec_RemoveFirstElement(vec, 2);

	for(new i = 0; i < sizeof expected; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
	}
}

Test:RemoveFirstElementOrdered()
{
	new
		Vec:vec = Vec_New(10, true),
		expected[] = {1, 3, 4, 5, 0};
	Vec_Append(vec, 1);
	Vec_Append(vec, 2);
	Vec_Append(vec, 3);
	Vec_Append(vec, 4);
	Vec_Append(vec, 5);

	Vec_RemoveFirstElement(vec, 2);

	for(new i = 0; i < sizeof expected; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
	}
}

Test:Contains()
{
	new Vec:vec = Vec_New(10);
	Vec_Append(vec, 7);
	Vec_Append(vec, 28);
	Vec_Append(vec, 46);

	ASSERT(Vec_Contains(vec, 7));
	ASSERT(Vec_Contains(vec, 28));
	ASSERT(Vec_Contains(vec, 46));
	ASSERT_FALSE(Vec_Contains(vec, 1));
	ASSERT_FALSE(Vec_Contains(vec, 2));
	ASSERT_FALSE(Vec_Contains(vec, 5));
}

Test:Clear()
{
	const CAPACITY = 12;
	new Vec:vec = Vec_New(CAPACITY);

	for(new i = 0; i < CAPACITY; ++i)
	{
		Vec_Append(vec, i+1);
	}

	Vec_Clear(vec);

	for(new i = 0; i < CAPACITY; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), 0);
	}
}

Test:FindFirst()
{
	new Vec:vec = Vec_New(10);
	Vec_Append(vec, 1);
	Vec_Append(vec, 2);
	Vec_Append(vec, 3);
	Vec_Append(vec, 2);

	ASSERT_EQ(Vec_FindFirst(vec, 2), 1);
	ASSERT_EQ(Vec_FindFirst(vec, 1), 0);
	ASSERT_EQ(Vec_FindFirst(vec, 3), 2);
}

Test:AppendArray()
{
	new
		Vec:vec = Vec_New(10),
		arr[] = {2, 3, 4, 5},
		expected[] = {1, 2, 3, 4, 5};
	Vec_Append(vec, 1);

	Vec_AppendArray(vec, arr, sizeof arr);

	ASSERT_EQ(Vec_Len(vec), 5);
	for(new i = 0; i < 5; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
	}
}

Test:NewFromArray()
{
	new arr[] = {1, 2, 3, 4, 5};

	new Vec:vec = Vec_NewFromArray(10, arr);

	ASSERT_EQ(Vec_Len(vec), 5);
	for(new i = 0; i < 5; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), arr[i]);
	}
}

Test:SetArray()
{
	new
		startingArr[] = {1, 2, 3, 4, 5},
		toSet[] = {6, 7, 8},
		expected[] = {1, 6, 7, 8, 5},
		Vec:vec = Vec_NewFromArray(10, startingArr);

	Vec_SetArray(vec, toSet, 1);

	ASSERT_EQ(Vec_Len(vec), 5);
	for(new i = 0; i < 5; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
	}
}

Test:SetArrayPastLength()
{
	new
		startingArr[] = {1, 2, 3, 4, 5},
		toSet[] = {6, 7, 8},
		expected[] = {1, 2, 3, 6, 7, 8},
		Vec:vec = Vec_NewFromArray(10, startingArr);

	Vec_SetArray(vec, toSet, 3);

	ASSERT_EQ(Vec_Len(vec), 6);
	for(new i = 0; i < 6; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
	}
}

Test:AppendVector()
{
	new
		arr[] = {1, 2},
		toAppendArr[] = {1, 2, 3, 4, 5, 6, 7},
		Vec:vec = Vec_NewFromArray(10, arr),
		Vec:toAppend = Vec_NewFromArray(10, toAppendArr);

	Vec_AppendVector(vec, toAppend, 2, 5);

	ASSERT_EQ(Vec_Len(vec), 5);
	for(new i = 0; i < 5; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), i + 1);
	}
}

Test:Swap()
{
	new
		arr[] = {1, 2, 3, 4, 5},
		Vec:vec = Vec_NewFromArray(10, arr),
		expected[] = {1, 4, 3, 2, 5};

	Vec_Swap(vec, 1, 3);

	for(new i = 0; i < 5; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
	}
}

Test:Reverse()
{
	new
		arr[] = {1, 2, 3, 4, 5, 6, 7},
		Vec:vec = Vec_NewFromArray(10, arr),
		expected[] = {1, 6, 5, 4, 3, 2, 7};

	Vec_Reverse(vec, 1, 5);

	for(new i = 0; i < 7; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
	}
}

Test:CopyTo()
{
	new
		arr[] = {1, 2, 3, 4, 5},
		Vec:vec = Vec_NewFromArray(10, arr),
		dest[5];

	Vec_CopyTo(vec, dest, 1, 4);

	for(new i = 0; i < 3; ++i)
	{
		ASSERT_EQ(dest[i], i + 2);
	}
}

Test:ForEach()
{
	new
		arr[] = {1, 2, 3, 4, 5},
		Vec:vec = Vec_NewFromArray(10, arr),
		i = 0;

	foreach(new value : Vector(vec))
	{
		ASSERT_EQ(Vec_Get(vec, i), value);
		i++;
	}
}

Test:ForEach_Range()
{
	new
		arr[] = {1, 2, 3, 4, 5, 6, 7, 8},
		Vec:vec = Vec_NewFromArray(10, arr),
		i = 2;

	foreach(new value : Vector(vec, 2, 6))
	{
		ASSERT_EQ(Vec_Get(vec, i), value);
		i++;
	}
}

Test:ForEach_RangeNegativeStart()
{
	new
		arr[] = {1, 2, 3, 4, 5, 6, 7, 8},
		Vec:vec = Vec_NewFromArray(10, arr),
		i = 0;

	foreach(new value : Vector(vec, -1, 8))
	{
		ASSERT_EQ(Vec_Get(vec, i), value);
		i++;
	}
	ASSERT_EQ(i, 8);
}

Test:ForEach_RangeEndPastLen()
{
	new
		arr[] = {1, 2, 3, 4, 5, 6, 7, 8},
		Vec:vec = Vec_NewFromArray(10, arr),
		i = 0;

	foreach(new value : Vector(vec, 0, 9))
	{
		ASSERT_EQ(Vec_Get(vec, i), value);
		i++;
	}
	ASSERT_EQ(i, 8);
}

Test:String()
{
	new Vec:vec = Vec_NewString("Hello, ");
	Vec_AppendString(vec, "worl");
	Vec_AppendChar(vec, 'd');

	ASSERT_SAME(Vec_GetAsArray(vec,0), "Hello, world");
	ASSERT_EQ(Vec_Len(vec), 12);
}

Test:RemoveFirstBy()
{
	new
		Vec:vec = Vec_NewFromArray(10, {1, 2, 3, 4, 5}),
		expected[] = {1, 5, 3, 4};

	inline isEven(val)
	{
		inline_return val % 2 == 0;
	}
	Vec_RemoveFirstBy(vec, using inline isEven);

	for(new i = 0; i < 4; ++i)
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
}

Test:RemoveLastBy()
{
	new
		Vec:vec = Vec_NewFromArray(10, {1, 2, 3, 4, 5}),
		expected[] = {1, 2, 3, 5};

	inline isEven(val)
	{
		inline_return val % 2 == 0;
	}
	Vec_RemoveLastBy(vec, using inline isEven);

	for(new i = 0; i < 4; ++i)
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
}

Test:RemoveLastElement()
{
	new
		Vec:vec = Vec_NewFromArray(10, {1, 3, 3, 2, 5}),
		expected[] = {1, 3, 5, 2};

	Vec_RemoveLastElement(vec, 3);

	for(new i = 0; i < 4; ++i)
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
}

Test:RemoveAllBy()
{
	new
		Vec:vec = Vec_NewFromArray(10, {1, 2, 3, 4, 5}),
		expected[] = {1, 5, 3};

	inline isEven(val)
	{
		inline_return val % 2 == 0;
	}
	Vec_RemoveAllBy(vec, using inline isEven);

	for(new i = 0; i < 3; ++i)
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
}

Test:RemoveAllElements()
{
	new
		Vec:vec = Vec_NewFromArray(10, {1, 2, 2, 4, 5}),
		expected[] = {1, 5, 4};

	Vec_RemoveAllElements(vec, 2);

	for(new i = 0; i < 3; ++i)
		ASSERT_EQ(Vec_Get(vec, i), expected[i]);
}

Test:ContainsBy()
{
	new
		Vec:vec = Vec_NewFromArray(10, {1, 2, 2, 4, 5});
	inline isEven(val)
		inline_return val % 2 == 0;
	inline isOdd(val)
		inline_return val % 2 != 0;
	inline isMultipleOf8(val)
		inline_return val % 8 == 0;

	ASSERT(Vec_ContainsBy(vec, using inline isEven));
	ASSERT(Vec_ContainsBy(vec, using inline isOdd));
	ASSERT_FALSE(Vec_ContainsBy(vec, using inline isMultipleOf8));
}

Test:FindFirstBy()
{
	new Vec:vec = Vec_NewFromArray(10, {1, 2, 2, 3, 4});
	inline isEven(val)
		inline_return val % 2 == 0;
	inline isMultipleOf8(val)
		inline_return val % 8 == 0;

	ASSERT_EQ(Vec_FindFirstBy(vec, using inline isEven), 1);
	ASSERT_EQ(Vec_FindFirstBy(vec, using inline isMultipleOf8), -1);
}

Test:FindLastBy()
{
	new Vec:vec = Vec_NewFromArray(10, {1, 2, 2, 3, 4});
	inline isOdd(val)
		inline_return val % 2 != 0;
	inline isMultipleOf8(val)
		inline_return val % 8 == 0;

	ASSERT_EQ(Vec_FindLastBy(vec, using inline isOdd), 3);
	ASSERT_EQ(Vec_FindLastBy(vec, using inline isMultipleOf8), -1);
}

Test:FindLastElement()
{
	new Vec:vec = Vec_NewFromArray(10, {1, 2, 2, 3, 4});

	ASSERT_EQ(Vec_FindLastElement(vec, 2), 2);
	ASSERT_EQ(Vec_FindLastElement(vec, 3), 3);
	ASSERT_EQ(Vec_FindLastElement(vec, 5), -1);
}

Test:FindAllby()
{
	new
		Vec:vec = Vec_NewFromArray(10, {1, 2, 3, 4, 5, 6, 7, 8}),
		expected1[] = {1, 3, 5, 7},
		expected2[] = {0, 2, 4, 6};
	inline isEven(val)
		inline_return val % 2 == 0;
	inline isOdd(val)
		inline_return val % 2 != 0;

	new
		Vec:found1 = Vec_FindAllBy(vec, using inline isEven),
		Vec:found2 = Vec_FindAllBy(vec, using inline isOdd);

	for(new i = 0; i < 4; ++i)
		ASSERT_EQ(Vec_Get(found1, i), expected1[i]);
	for(new i = 0; i < 4; ++i)
		ASSERT_EQ(Vec_Get(found2, i), expected2[i]);
}

Test:FindAllElements()
{
	new
		Vec:vec = Vec_NewFromArray(10, {1, 2, 1, 2, 1, 2, 1, 2}),
		expected1[] = {1, 3, 5, 7},
		expected2[] = {0, 2, 4, 6};

	new
		Vec:found1 = Vec_FindAllElements(vec, 2),
		Vec:found2 = Vec_FindAllElements(vec, 1);

	for(new i = 0; i < 4; ++i)
		ASSERT_EQ(Vec_Get(found1, i), expected1[i]);
	for(new i = 0; i < 4; ++i)
		ASSERT_EQ(Vec_Get(found2, i), expected2[i]);
}

Test:TrueForAll()
{
	new Vec:vec = Vec_NewFromArray(10, {2, 4, 6 ,8, 10});

	inline isEven(val)
		inline_return val % 2 == 0;

	ASSERT(Vec_TrueForAll(vec, using inline isEven));
	Vec_Append(vec, 1);
	ASSERT_FALSE(Vec_TrueForAll(vec, using inline isEven));
}

Test:BinarySearch()
{
	new Vec:vec = Vec_NewFromArray(10, {1, 2, 3, 4, 5, 6, 7, 8});

	ASSERT_EQ(Vec_BinarySearch(vec, 3), 2);
	ASSERT_EQ(Vec_BinarySearch(vec, 7), 6);
	ASSERT_EQ(Vec_BinarySearch(vec, 8), 7);
	ASSERT_EQ(Vec_BinarySearch(vec, 9), -1);
}

Test:Sort()
{
	new
		Vec:vec = Vec_NewFromArray(10, {7, 4, 5, 3, 1, 8, 2, 6}),
		sorted[] = {1, 2, 3, 4, 5, 6, 7, 8};

	Vec_Sort(vec);
	for(new i = 0; i < 8; ++i)
	{
		ASSERT_EQ(Vec_Get(vec, i), sorted[i]);
	}
}

Test:Clone()
{
	new Vec:vec = Vec_NewFromArray(5, {1, 2, 3, 4, 5});

	new Vec:clone = Vec_Clone(vec, true);

	ASSERT(Vec_IsReadOnly(clone));
	for(new i = 0; i < 5; ++i)
	{
		ASSERT_EQ(Vec_Get(clone, i), Vec_Get(vec, i));
	}

	Vec_Delete(vec);
	Vec_Delete(clone);
}