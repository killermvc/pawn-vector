#define RUN_TESTS
#define YSI_NO_HEAP_MALLOC

#include <a_samp>
#include "vector.inc"
#include <YSI_Core\y_testing>


Test:New() {
	new Vec:vec = Vec_New();
    ASSERT(Vec_IsValid(vec));
    Vec_Delete(vec);
}

Test:NewArray() {
	new arr[] = {1, 2, 3, 4, 5, 6};
    new Vec:vec = Vec_NewFromArray(arr, sizeof arr);
    ASSERT(Vec_IsValid(vec));
    for(new i = 0; i < 6; i++) {
        ASSERT(Vec_GetValue(vec, i) == arr[i]);
    }
    Vec_Delete(vec);
}

Test:append() {
    new Vec:vec = Vec_New();

    Vec_Append(vec, 1);
    Vec_Append(vec, 2);
    Vec_Append(vec, 3);

    ASSERT(Vec_GetValue(vec, 0) == 1);
    ASSERT(Vec_GetValue(vec, 1) == 2);
    ASSERT(Vec_GetValue(vec, 2) == 3);
    Vec_Delete(vec);
}

Test:GetCapacity() {
	new Vec:vec = Vec_New();
    //y_malloc allocates at least the number of cells specified, but might be higher
    ASSERT(Vec_GetCapacity(vec) >= VEC_DEFAULT_CAPACITY);
    Vec_Delete(vec);
}

Test:Resize() {
	new Vec:vec = Vec_New();

    Vec_Resize(vec, 51);
    //y_malloc allocates at least the number of cells specified, but might be higher
    ASSERT(Vec_GetCapacity(vec) >= 51);

    Vec_Resize(vec, 150);
    //y_malloc allocates at least the number of cells specified, but might be higher
    ASSERT(Vec_GetCapacity(vec) >= 150);

    Vec_Delete(vec);
}

Test:AppendArray() {
	new Vec:vec = Vec_New();
    new arr[] = {1,2,3};

    Vec_AppendArray(vec, arr);

    ASSERT(Vec_GetValue(vec, 0) == 1);
    ASSERT(Vec_GetValue(vec, 1) == 2);
    ASSERT(Vec_GetValue(vec, 2) == 3);

    Vec_Delete(vec);
}

Test:SetValue() {
	new Vec:vec = Vec_New();

    Vec_SetValue(vec, 2, 10);
    ASSERT(Vec_GetValue(vec, 2) == 10);

    Vec_Delete(vec);
}

Test:RemoveAt() {
    new arr[] = {1, 2, 3};

    new Vec:vec = Vec_NewFromArray(arr, sizeof(arr));

    Vec_RemoveAt(vec, 1);
    ASSERT(Vec_GetValue(vec, 0) == 1);
    ASSERT(Vec_GetValue(vec, 1) == 3);

    Vec_Delete(vec);
}

Test:Delete() {
	new Vec:vec = Vec_New();
    ASSERT(Vec_IsValid(vec));

    Vec_Delete(vec);
    ASSERT(!Vec_IsValid(vec));
}

Test:CreateDeleteMany() {
    new Vec:vecs[100];

    for(new i = 0; i < 100; i++) {
        vecs[i] = Vec_New();
    }

    for(new i = 0; i < 100; i++) {
        Vec_Delete(vecs[i]);
    }
}

Test:RemoveAtOrdered() {
    new arr[] = {1, 2, 3, 4, 5, 6};
    new Vec:vec = Vec_NewFromArray(arr, VEC_DEFAULT_CAPACITY);
    Vec_ToggleOrdered(vec, true);

    Vec_RemoveAt(vec, 2);
    ASSERT(Vec_GetValue(vec, 0) == 1);
    ASSERT(Vec_GetValue(vec, 1) == 2);
    ASSERT(Vec_GetValue(vec, 2) == 4);
    ASSERT(Vec_GetValue(vec, 3) == 5);
    ASSERT(Vec_GetValue(vec, 4) == 6);

    Vec_Delete(vec);
}

Test:SizeIncrease() {
    new Vec:vec = Vec_New(1, 10);

    ASSERT(Vec_GetCapacity(vec) >= 1);
    Vec_Append(vec, 1);
    Vec_Append(vec, 2);
    ASSERT(Vec_GetCapacity(vec) >= 11);

    Vec_Delete(vec);
}

Test:SetArray() {
    new
        arr[] = {1, 2, 3, 4, 5, 6},
        arrToSet[] = {7, 8, 9},
        resultArr[] = {1, 2, 8, 9, 5, 6},
        Vec:vec = Vec_NewFromArray(arr, sizeof arr),
        bool:pass = true;

    Vec_SetArray(vec, 2, arrToSet, 1);

    for(new i = 0; i < 6; i++) {
        if(Vec_GetValue(vec, i) != resultArr[i]) {
            pass = false;
        }
    }

    ASSERT(pass);

    Vec_Delete(vec);
}

Test:Reverse() {
    new
        arr[] = {1, 2, 3, 4 ,5, 6},
        arrReversed[] = {6, 5, 4, 3, 2, 1},
        arrReversed2[] = {6, 5, 2, 3, 4, 1},
        bool:pass = true,
        Vec:vec = Vec_NewFromArray(arr, sizeof arr);

    Vec_Reverse(vec);

    for(new i = 0; i < 6; i++) {
        if(Vec_GetValue(vec, i) != arrReversed[i]) {
            pass = false;
        }
    }
    ASSERT(pass);

    Vec_Reverse(vec, 2, 4);

    for(new i = 0; i < 6; i++) {
        if(Vec_GetValue(vec, i) != arrReversed2[i]) {
            pass = false;
        }
    }

    ASSERT(pass);

    Vec_Delete(vec);
}

Test:CopyTo() {
    new
        arr[] = {1, 2, 3, 4, 5, 6},
        dest[6],
        Vec:vec = Vec_NewFromArray(arr, sizeof arr),
        bool:pass = true;

    Vec_CopyTo(vec, dest);

    for(new i = 0; i < 6; i++) {
        if(dest[i] != arr[i]) {
            pass = false;
        }
    }
    ASSERT(pass);

    Vec_Delete(vec);
}

Test:AppendVector() {
    new
        arr[] = {1, 2},
        toAppend[] = {1, 2, 3, 4, 5, 6},
        result[] = {1, 2, 3, 4, 5},
        Vec:vec = Vec_NewFromArray(arr, sizeof arr),
        Vec:vec2 = Vec_NewFromArray(toAppend, sizeof toAppend),
        bool:pass = true;

    Vec_AppendVector(vec, vec2, 2, 4);

    for(new i = 0; i < 5; i++) {
        if(Vec_GetValue(vec, i) != result[i]) {
            pass = false;
        }
    }

    printf("%d", Vec_GetLength(vec));

    ASSERT(pass && Vec_GetLength(vec) == 5);

    Vec_Delete(vec);
}

Test:Remove() {
    new
        arr[] = {1, 2, 3, 4 ,3, 6},
        result[] = {1, 2, 6, 4, 3},
        bool:pass = true,
        Vec:vec = Vec_NewFromArray(arr, sizeof arr);

    Vec_RemoveElement(vec, 3);

    for(new i = 0; i < 5; i++) {
        if(Vec_GetValue(vec, i) != result[i]) {
            pass = false;
        }
    }

    ASSERT(pass);

    Vec_Delete(vec);
}

Test:RemoveOrdered() {
    new
        arr[] = {1, 2, 3, 4, 3, 6},
        result[] = {1, 2, 4 ,3, 6},
        bool:pass = true,
        Vec:vec = Vec_NewFromArray(arr, 6);

    Vec_ToggleOrdered(vec, true);

    Vec_RemoveElement(vec, 3);

    for(new i = 0; i < 5; i++) {
        if(Vec_GetValue(vec, i) != result[i]) {
            pass = false;
        }
    }

    ASSERT(pass);

    Vec_Delete(vec);
}

Test:Clone() {
    new
        arr[] = {1, 2, 3, 4, 5, 6},
        bool:pass = true,
        Vec:vec = Vec_NewFromArray(arr, 7, 10);

    Vec_ToggleOrdered(vec, true);
    Vec_ToggleFixedSize(vec, true);

    new Vec:vec2 = Vec_Clone(vec);

    for(new i = 0; i < 6; i++) {
        if(Vec_GetValue(vec, i) != Vec_GetValue(vec2, i)) {
            pass = false;
        }
    }

    ASSERT(pass);
    ASSERT(Vec_IsOrdered(vec2));
    ASSERT(Vec_IsFixedSize(vec2));

    ASSERT(Vec_GetIncrease(vec) == Vec_GetIncrease(vec2));

    Vec_Delete(vec);
    Vec_Delete(vec2);
}

Test:FindAll() {
    new
        arr[] = {7, 5, 6, 4, 2, 4, 1, 3, 4, 9, 4},
        exptectedIndexes[] = {3, 5, 8, 10},
        bool:pass = true,
        Vec:vec = Vec_NewFromArray(arr);

    new Vec:indexVec = Vec_FindAll(vec, 4);

    ASSERT(Vec_GetLength(indexVec) == 4);

    for(new i = 0; i < 4; i++) {
        if(Vec_GetValue(indexVec, i) != exptectedIndexes[i]) {
            pass = false;
        }
    }
    ASSERT(pass);

    Vec_Delete(indexVec);

    indexVec = Vec_FindAll(vec, 15);

    ASSERT(indexVec == INVALID_VECTOR_ID);

    Vec_Delete(vec);
    Vec_Delete(indexVec);
}

Test:find() {
    new
        arr[] = {1, 2, 3, 4, 5, 6},
        Vec:vec = Vec_NewFromArray(arr),
        index;

    ASSERT(Vec_Find(vec, 3, index));
    ASSERT(index == 2);

    ASSERT(!Vec_Find(vec, 7, index));
    ASSERT(index == -1);

    Vec_Delete(vec);
}

Test:RemoveLast() {
    new
        arr[] = {9, 8, 3, 8, 4, 6 ,8 ,1 ,6},
        Vec:vec = Vec_NewFromArray(arr),
        res[] =  {9, 8, 3, 8, 4, 6, 6, 1},
        bool:pass = true;

    Vec_RemoveLastElement(vec, 8);

    for(new i = 0; i < 8; i++) {
        if(Vec_GetValue(vec, i) != res[i]) {
            pass = false;
        }
    }

    ASSERT(pass);

    Vec_Delete(vec);
}

Test:FindLast() {
    new
        arr[] = {9, 8, 3, 8, 4, 6 ,8 ,1 ,6},
        Vec:vec = Vec_NewFromArray(arr),
        index;

    new ret = Vec_FindLast(vec, 8, index);
    ASSERT(ret == VEC_OK && index == 6);

    ret = Vec_FindLast(vec, 15);
    ASSERT(ret == VEC_NOT_FOUND);

    Vec_Delete(vec);
}

Test:RemoveAll() {
    new
        arr[] = {9, 8, 3, 8, 4, 6 ,8 ,1 ,6},
        Vec:vec = Vec_NewFromArray(arr),
        res[] =  {9, 6, 3, 1, 4, 6},
        bool:pass = true;

    Vec_RemoveAllElements(vec, 8);

    for(new i = 0; i < 6; i++) {
        if(Vec_GetValue(vec, i) != res[i]) {
            pass = false;
        }
    }

    ASSERT(pass);
    Vec_Delete(vec);
}

Test:Swap() {
    new
        arr[] = {1, 2, 3, 4, 5, 6},
        res[] = {1, 5, 3, 4, 2, 6},
        Vec:vec = Vec_NewFromArray(arr),
        bool:pass = true;

    Vec_Swap(vec, 1, 4);

    for(new i = 0; i < Vec_GetLength(vec); i++) {
        if(Vec_GetValue(vec, i) != res[i]) {
            pass = false;
        }
    }

    ASSERT(pass);

    Vec_Delete(vec);
}

Test:SortDef() {
    new
        arr[] = {16, 61, 64, 91, 23, 97, 51, 70, 90, 80},
        res[] = {16, 23, 51, 61, 64, 70, 80, 90, 91, 97},
        Vec:vec = Vec_NewFromArray(arr),
        bool:pass = true;

    Vec_Sort(vec);

    for(new i = 0; i < Vec_GetLength(vec); i++) {
        if(Vec_GetValue(vec, i) != res[i]) {
            pass = false;
        }
    }

    ASSERT(pass);
}

Test:sort() {
    new
        arr[] = {16, 61, 64, 91, 23, 97, 51, 70, 90, 80},
        res[] = {97, 91, 90, 80, 70, 64, 61, 51, 23, 16},
        Vec:vec = Vec_NewFromArray(arr),
        bool:pass = true;

    inline compare(value1, value2) {
        inline_return value1 >= value2 ? true : false;
    }

    Vec_SortBy(vec, using inline compare);

    for(new i = 0; i < Vec_GetLength(vec); i++) {
        if(Vec_GetValue(vec, i) != res[i]) {
            pass = false;
        }
    }

    ASSERT(pass);
}

Test:Strings() {
    new Vec:string = Vec_NewString("hello");
    Vec_AppendString(string, ", world!");

    new bool:pass;
    if(!strcmp("hello, world!", Vec_GetString(string, 0))) {
        pass = true;
    }
    ASSERT(pass);

    Vec_ChangeString(string, "hola");
    Vec_AppendChar(string, ',');
    Vec_AppendString(string, " mundo!");

    pass = false;
    if(!strcmp("hola, mundo!", Vec_GetString(string, 0))) {
        pass = true;
    }
    ASSERT(pass);
}