#define RUN_TESTS
#include <a_samp>
#include "vector.inc"
#include <YSI_Core\y_testing>


Test:New() {
	new Vec:vec = Vec_New();
    ASSERT(IsValidVec(vec));
    Vec_Delete(vec);
}

Test:NewArray() {
	new arr[] = {1, 2, 3, 4, 5, 6};
    new Vec:vec = Vec_NewFromArray(arr, sizeof arr);
    ASSERT(IsValidVec(vec));
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
    ASSERT(IsValidVec(vec));

    Vec_Delete(vec);
    ASSERT(!IsValidVec(vec));
}

Test:CreateDeleteMany() {
    new Vec:vecs[5000];

    for(new i = 0; i < 5000; i++) {
        vecs[i] = Vec_New();
    }

    for(new i = 0; i < 5000; i++) {
        Vec_Delete(vecs[i]);
    }
}

Test:RemoveAtOrdered() {
    new arr[] = {1, 2, 3, 4, 5, 6};
    new Vec:vec = Vec_NewFromArray(arr, sizeof arr, VEC_DEFAULT_CAPACITY, false, false, true);

    Vec_RemoveAt(vec, 2);
    ASSERT(Vec_GetValue(vec, 0) == 1);
    ASSERT(Vec_GetValue(vec, 1) == 2);
    ASSERT(Vec_GetValue(vec, 2) == 4);
    ASSERT(Vec_GetValue(vec, 3) == 5);
    ASSERT(Vec_GetValue(vec, 4) == 6);

    Vec_Delete(vec);
}

Test:SizeIncrease() {
    new Vec:vec = Vec_New(1, false, false, false, 10);

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

    ASSERT(pass);

    Vec_Delete(vec);
}

Test:Remove() {
    new
        arr[] = {1, 2, 3, 4 ,3, 6},
        result[] = {1, 2, 6, 4, 3},
        bool:pass = true,
        index,
        Vec:vec = Vec_NewFromArray(arr, sizeof arr);

    Vec_Remove(vec, 3, index);

    for(new i = 0; i < 5; i++) {
        if(Vec_GetValue(vec, i) != result[i]) {
            pass = false;
        }
    }

    ASSERT(pass && index == 2);

    Vec_Delete(vec);
}

Test:RemoveOrdered() {
    new
        arr[] = {1, 2, 3, 4, 3, 6},
        result[] = {1, 2, 4 ,3, 6},
        bool:pass = true,
        index,
        Vec:vec = Vec_NewFromArray(arr, sizeof arr, 6, false, false, true);

    Vec_Remove(vec, 3, index);

    for(new i = 0; i < 5; i++) {
        if(Vec_GetValue(vec, i) != result[i]) {
            pass = false;
        }
    }

    ASSERT(pass && index == 2);

    Vec_Delete(vec);
}

Test:Clone() {
    new
        arr[] = {1, 2, 3, 4, 5, 6},
        bool:pass = true,
        Vec:vec = Vec_NewFromArray(arr, sizeof arr, 7, true, false, true, 10);

    new Vec:vec2 = Vec_Clone(vec);

    for(new i = 0; i < 6; i++) {
        if(Vec_GetValue(vec, i) != Vec_GetValue(vec2, i)) {
            pass = false;
        }
    }

    ASSERT(pass);
    ASSERT(Vec_IsOrdered(vec2));
    ASSERT(Vec_IsFixedSize(vec2));
    ASSERT(Vec_IsOrdered(vec2));

    ASSERT(Vec_GetIncrease(vec) == Vec_GetIncrease(vec2));

    Vec_Delete(vec);
    Vec_Delete(vec2);
}
