#define RUN_TESTS
#define YSI_NO_HEAP_MALLOC
#define YSI_NO_CACHE_MESSAGE
#define YSI_NO_OPTIMISATION_MESSAGE
#define YSI_NO_VERSION_CHECK

#include "vector.inc"
#include <YSI_Core\y_testing>


Test:New()
{
	new Vec:vec = Vec_New(25, 28, true);

	//y_malloc allocates multiples of 16 cells. so the capacity allocated is 32.
	//However y_malloc uses one of this cells to store the capacity
	//and pawn-vector uses 3 more to store the length, the growth and whether is ordered or not
	//thus the reported capacity is 28.
	ASSERT_EQ(Vec_Capacity(vec), 28);
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
	new Vec:vec = Vec_New(10, 10);

	for(new i = 0; i < 14; ++i)
	{
		Vec_Append(vec, i+1);
	}

	//y_malloc allocates multiples of 16 cells. when creating the vector, the capacity allocated is 16,
	//after the resize is 32. However y_malloc uses one of this cells to store the capacity
	//and pawn-vector uses 3 more to store the length, the growth and whether is ordered or not
	//thus the reported capacity is 28.
	ASSERT_EQ(Vec_Capacity(vec), 28);
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

	Vec_Set(vec, 12, 7);

	ASSERT_EQ(Vec_Len(vec), 13);
	ASSERT_EQ(Vec_Get(vec, 12), 7);
	ASSERT_GE(Vec_Capacity(vec), 12 + VEC_DEFAULT_GROWTH);

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
		Vec:vec = Vec_New(10, VEC_DEFAULT_GROWTH, true),
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
		Vec:vec = Vec_New(10, VEC_DEFAULT_GROWTH, true),
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