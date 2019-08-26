#include <a_samp>
#include "pawn-vector.inc"

main() {
    new Vec:vector = Vec_New();
    if(!IsValidVec(vector)) {
        print("Vec_New failed");
    } else {
        print("Vec_New passed");
    }

    Vec_Append(vector, 1);
    Vec_Append(vector, 2);
    Vec_Append(vector, 3);
    
    if(Vec_GetValue(vector, 0) != 1 || Vec_GetValue(vector, 1) != 2 || Vec_GetValue(vector, 2) != 3) {
        print("Vec_Append failed");
    } else {
        print("Vec_Append passed");
    }

    new arr[3];
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    new Vec:vec = Vec_NewFromArray(arr, sizeof arr);
    if(Vec_GetValue(vec, 0) != 1 || Vec_GetValue(vec, 1) != 2 ||  Vec_GetValue(vec, 2) != 3) {
        printf("Vec_NewFromArray failed", Vec_GetLength(vec));
    } else {
        print("Vec_NewFromArray passed");
    }

    if(Vec_GetCapacity(vec) != VEC_DEFAULT_CAPACITY) {
        printf("Vec_GetCapacity failed: %d", Vec_GetCapacity(vec));
    } else {
        print("Vec_GetCapacity passed");
    }

    Vec_Resize(vec, 51);
    if(Vec_GetCapacity(vec) != 51) {
        print("Vec_Resize failed");
        printf("capacity: %d", Vec_GetCapacity(vec));
    } else {
        print("Vec_Resize passed");
    }
    
    Vec_AppendArray(vec, arr);
    if(Vec_GetValue(vec, 0) != 1 || Vec_GetValue(vec, 1) != 2 ||  Vec_GetValue(vec, 2) != 3 || Vec_GetValue(vec, 3) != 1 || Vec_GetValue(vec, 4) != 2 ||  Vec_GetValue(vec, 5) != 3) {
        printf("Vec_AppendArray failed");       
    } else {
        print("Vec_AppendArray passed");
    }

    Vec_SetValue(vec, 2, 10);
    if(Vec_GetValue(vec, 2) != 10) {
        print("Vec_SetValue failed");
    } else {
        print("Vec_SetValue passed");
    }

    Vec_Remove(vec, 2);
    if(Vec_GetValue(vec, 0) != 1 || Vec_GetValue(vec, 1) != 2 ||  Vec_GetValue(vec, 2) != 3 || Vec_GetValue(vec, 3) != 1 || Vec_GetValue(vec, 4) != 2 || Vec_GetLength(vec) != 5) {
        printf("Vec_Remove failed");
    } else {
        print("Vec_Remove passed");
    }

    Vec_Delete(vec);
    if(IsValidVec(vec)) {
        print("Vec_Delete failed");
    } else {
        print("Vec_Delete passed");
    }

    new arr2[6];
    for(new i = 0; i < 6; i++) {
        arr2[i] = i+1;
    }
    new Vec:vector2 = Vec_NewFromArray(arr2, sizeof arr2, VEC_DEFAULT_CAPACITY, true);
    
    Vec_Remove(vector2, 3);

    if(Vec_GetValue(vector2, 0) != 1 || Vec_GetValue(vector2, 1) != 2 || Vec_GetValue(vector2, 2) != 3 || Vec_GetValue(vector2, 3) != 5 || Vec_GetValue(vector2, 4) != 6) {
        print("Vec_Remove ordered failed");
    } else {
        print("Vec_Remove ordered passed");
    }

    //using a vector as an string
    new Vec:vector3 = Vec_NewFromArray("hello,", 6);
    printf("vector3: %s", Vec_GetString(vector3, 0));

    Vec_AppendString(vector3, " world!");
    printf("vector3: %s", Vec_GetString(vector3, 0));

    Vec_ChangeString(vector3, "hola, mundo!");
    printf("vector3: %s", Vec_GetString(vector3, 0));

    new Vec:slots[50];
    for(new i =0; i < 50; i++) {
        slots[i] = Vec_New();
    }

    for(new i =0; i < 50; i++) {
        Vec_Delete(slots[i]);
    }
    
}
