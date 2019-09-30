#define RUN_TESTS
#include <a_samp>
#include "pawn-vector.inc"
#include <YSI_Core\y_testing>


Test:New() {
	new Vec:vector = Vec_New();
    ASSERT(IsValidVec(vector));
}

Test:NewArray() {
	new arr[] = {1,2,3};
    new Vec:vec = Vec_NewFromArray(arr, sizeof arr);
    ASSERT(IsValidVec(vec));
    ASSERT(Vec_GetValue(vec, 0) == 1);
    ASSERT(Vec_GetValue(vec, 1) == 2);
    ASSERT(Vec_GetValue(vec, 2) == 3);
}

Test:append() {
    new Vec:vector = Vec_New();

    Vec_Append(vector, 1);
    Vec_Append(vector, 2);
    Vec_Append(vector, 3);

    ASSERT(Vec_GetValue(vector, 0) == 1);
    ASSERT(Vec_GetValue(vector, 1) == 2);
    ASSERT(Vec_GetValue(vector, 2) == 3);
}

Test:GetCapacity() {
	new Vec:vec = Vec_New();
    //y_malloc allocates at least the number of cells specified, but might be higher
    ASSERT(Vec_GetCapacity(vec) >= VEC_DEFAULT_CAPACITY);
}

Test:Resize() {
	new Vec:vec = Vec_New();

    Vec_Resize(vec, 51);
    //y_malloc allocates at least the number of cells specified, but might be higher
    ASSERT(Vec_GetCapacity(vec) >= 51);

    Vec_Resize(vec, 150);
    //y_malloc allocates at least the number of cells specified, but might be higher
    ASSERT(Vec_GetCapacity(vec) >= 150);
}

Test:AppendArray() {
	new Vec:vector = Vec_New();
    new arr[] = {1,2,3};

    Vec_AppendArray(vector, arr);

    ASSERT(Vec_GetValue(vector, 0) == 1);
    ASSERT(Vec_GetValue(vector, 1) == 2);
    ASSERT(Vec_GetValue(vector, 2) == 3);
}

Test:SetValue() {
	new Vec:vec = Vec_New();

    Vec_SetValue(vec, 2, 10);
    ASSERT(Vec_GetValue(vec, 2) == 10);
}

Test:Remove() {
    new arr[] = {1, 2, 3};

    new Vec:vec = Vec_NewFromArray(arr, sizeof(arr));

    Vec_RemoveAt(vec, 1);
    ASSERT(Vec_GetValue(vec, 0) == 1);
    ASSERT(Vec_GetValue(vec, 1) == 3);
}

Test:Delete() {
	new Vec:vector = Vec_New();
    ASSERT(IsValidVec(vector));

    Vec_Delete(vector);
    ASSERT(!IsValidVec(vector));
}

Test:RemoveOrdered() {
    new arr[] = {1, 2, 3, 4, 5, 6};
    new Vec:vector = Vec_NewFromArray(arr, sizeof arr, VEC_DEFAULT_CAPACITY, true);

    Vec_RemoveAt(vector, 2);
    ASSERT(Vec_GetValue(vector, 0) == 1);
    ASSERT(Vec_GetValue(vector, 1) == 2);
    ASSERT(Vec_GetValue(vector, 2) == 4);
    ASSERT(Vec_GetValue(vector, 3) == 5);
    ASSERT(Vec_GetValue(vector, 4) == 6);
}

Test:SizeIncrease() {
    new Vec:vec = Vec_New(1, false, false, false, 10);
    ASSERT(Vec_GetCapacity(vec) >= 1);
    Vec_Append(vec, 1);
    Vec_Append(vec, 2);
    ASSERT(Vec_GetCapacity(vec) >= 11);
}