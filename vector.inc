#if defined _inc_vector
    #undef _inc_vector
#endif

#if defined _vector_included
    #endinput
#endif

#define _vector_included

#include <YSI_Coding\y_malloc>

#if !defined VEC_DEFAULT_CAPACITY
    #define VEC_DEFAULT_CAPACITY    (32)
#endif


#if !defined VEC_DEFAULT_INCREASE
    #define VEC_DEFAULT_INCREASE    (6)
#endif

/*
    internal
*/

enum _:e_VEC_ERRORS
{
    VEC_OK = 0,
    VEC_NOALLOC,
    VEC_INVALID,
    VEC_NOT_FOUND,
    VEC_NOP
}


#define INVALID_VECTOR_ID           (Vec:-1)

static Alloc:Vec_vectorLength;
static Alloc:Vec_isOrdered;
static Alloc:Vec_increase;
static Alloc:Vec_vectorSlot;
static Alloc:Vec_isSizeFixed;
static Alloc:Vec_isReadOnly;

//highest id assigned
static  Vec:Vec_LastID;

//array with ids that were in use and then released
static  Alloc:Vec_releasedIDs;

//index of the last released id
static  Vec_lastReleasedID;


static stock Alloc:Vec_internalResize(Alloc:slot, newSize, length) {
    //alloc more memory with the new size
    new Alloc:newArr = calloc(newSize);
    if(newArr != NO_ALLOC) {
        //copy last alloc to the newly allocated data
        Malloc_SetA(newArr, 0, mget(slot, 0), length);
        //free old memory
        Malloc_Free(slot);
    }
    return newArr;
}

static stock Alloc:Vec_internalAppend(Alloc:slot, value, &length, increase = VEC_DEFAULT_INCREASE, bool:isFixed = false) {
    if(length < Malloc_SlotSize(slot)) {
        //if there is enough space for the new value just set it at the end
        mset(slot, length, value);
        length++;
        return slot;
    }
    if(isFixed) {
        return NO_ALLOC;
    }
    //if there isn't enough space resize the alloc and set the value at the end of the new alloc
    new Alloc:newArr = Vec_internalResize(slot, Malloc_SlotSize(slot) + increase, Malloc_SlotSize(slot));
    if(newArr != NO_ALLOC) {
        mset(newArr, length, value);
        length++;
    }
    //will return the new Alloc slot if Vec_internalResize succeeded or NO_ALLOC otherwise
    return newArr;
}

static stock Alloc:Vec_internalAppendArray(Alloc:slot, const arr[], arrSize, &length, increase = VEC_DEFAULT_INCREASE, bool:isFixed) {
    if(length+arrSize <= Malloc_SlotSize(slot)) {
        //if there is enough space for the new value just set it at the end
        Malloc_SetA(slot, length, arr, arrSize);
        length += arrSize;
        return slot;
    }
    if(isFixed) {
        return NO_ALLOC;
    }
    //if there isn't enough space resize the alloc and set the value at the end of the new alloc
    new Alloc:newArr = Vec_internalResize(slot, length + arrSize + increase, Malloc_SlotSize(slot));
    if(newArr != NO_ALLOC) {
        Malloc_SetA(newArr, length, arr, arrSize);
        length += arrSize;
    }
    return newArr;
}

static stock Vec_internalRemove(Alloc:slot, index, bool:ordered, &length) {
    if(index > length) {
        //if the index wasnt yet setted, don't remove it
        return 1;
    }
    if(!ordered) {
        mset(slot, index, mget(slot, length-1));
        length--;
        mset(slot, length, 0);
        return VEC_OK;
    }
    Malloc_SetA(slot, index, mget(slot, index+1), length-index);
    length--;
    return VEC_OK;
}

static stock Vec:Vec_getNewId() {
    if(Vec_lastReleasedID > 0) {
        //if there are released ids return one of them
        new id = mget(Vec_releasedIDs, Vec_lastReleasedID-1);
        Vec_lastReleasedID--;
        return Vec:id;
    }
    //else return a new id
    Vec_LastID++;
    return Vec_LastID-Vec:1;
}

stock bool:Vec_IsValid(Vec:vec) {
    if(vec >= Vec_LastID || vec == INVALID_VECTOR_ID) {
        //if the id wasn't yet assigned return false
        return false;
    }
    for(new i = 0; i < _:Vec_lastReleasedID; i++) {
        if(mget(Vec_releasedIDs, i) == _:vec) {
            //if the id is released return false
            return false;
        }
    }
    //otherwise return true
    return true;
}

static stock Vec_ReleaseId(Vec:id) {
    if(!Vec_IsValid(id)) {
        return VEC_INVALID;
    }
    if(id == Vec_LastID-Vec:1) {
        //if the id is the last assigned just decrease the last assigned id
        Vec_LastID--;
        return VEC_OK;
    }
    //if none of the others, add the id to the free ids list
    new Alloc:ret = Vec_internalAppend(Vec_releasedIDs, _:id, Vec_lastReleasedID);
    Vec_releasedIDs = ret;
    if(ret == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate memory for released id's");
        return VEC_NOALLOC;
    }
    return VEC_OK;
}

static stock Vec_resizeVectorsData(id, length) {
    new Alloc:ret = Vec_internalResize(Vec_vectorLength, id + VEC_DEFAULT_INCREASE, length);
    if(ret == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate more memory for the vector's length");
        return VEC_NOALLOC;
    }
    Vec_vectorLength = ret;
    ret = Vec_internalResize(Vec_isOrdered, id + VEC_DEFAULT_INCREASE, length);
    if(ret == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate more memory for storing vector ordered status");
        return VEC_NOALLOC;
    }
    Vec_isOrdered = ret;
    ret = Vec_internalResize(Vec_vectorSlot, id + VEC_DEFAULT_INCREASE, length);
    if(ret == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate more memory for vectors");
        return VEC_NOALLOC;
    }
    Vec_vectorSlot = ret;
    ret = Vec_internalResize(Vec_increase, id + VEC_DEFAULT_INCREASE, length);
    if(ret == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate more memory for vectors' increase");
        return VEC_NOALLOC;
    }
    Vec_increase = ret;
    ret = Vec_internalResize(Vec_isSizeFixed, id + VEC_DEFAULT_INCREASE, length);
    if(ret == NO_ALLOC) {
        P:F("PAWN-VECTOR Couldn't allocate more memory for vectors size");
        return VEC_NOALLOC;
    }
    Vec_isSizeFixed = ret;
    ret = Vec_internalResize(Vec_isReadOnly, id + VEC_DEFAULT_INCREASE, length);
    if(ret == NO_ALLOC) {
        P:F("PAWN-VECTOR Couldn't allocate more memory for vectors size");
        return VEC_NOALLOC;
    }
    Vec_isReadOnly = ret;
    return VEC_OK;
}

stock Alloc:Vec_GetVecAllocSlot(Vec:vec) {
    return Alloc:mget(Vec_vectorSlot, _:vec);
}

#include <YSI_Coding\y_hooks>
hook OnScriptInit() {
    Vec_vectorLength = calloc(20);
    if(Vec_vectorLength == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate memory for the vector's length");
    }
    Vec_vectorSlot = calloc(20);
    if(Vec_vectorSlot == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate memory for vectors");
    }
    Vec_isOrdered = calloc(20);
    if(Vec_isOrdered == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate memory for storing vector ordered property");
    }
    Vec_increase = calloc(20);
    if(Vec_increase == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate memory for vectors' increase");
    }
    Vec_releasedIDs = calloc(20);
    if(Vec_releasedIDs == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate memory for released ids");
    }
    Vec_isSizeFixed = calloc(20);
    if(Vec_isSizeFixed == NO_ALLOC) {
        P:F("PAWN-VECTOR Couldn't allocate memory for vectors fixedSize property");
    }
    Vec_isReadOnly = calloc(20);
    if(Vec_isReadOnly == NO_ALLOC) {
        P:F("PAWN-VECTOR Couldn't allocate memory for vectors readonly property");
    }
    return 1;
}

hook OnScriptExit() {
    free(Vec_releasedIDs);
    free(Vec_vectorLength);
    free(Vec_isOrdered);
    free(Vec_isSizeFixed);
    free(Vec_isReadOnly);
    for(new i = 0; i < _:Vec_LastID; i++) {
        free(Alloc:mget(Vec_vectorSlot, i));
    }
    free(Vec_vectorSlot);
    return 1;
}


/*
    api
*/

stock bool:Vec_IsFixedSize(Vec:vec) {
    if(!Vec_IsValid(vec)) {
        return false;
    }
    return bool:mget(Vec_isSizeFixed, _:vec);
}

stock Vec_GetIncrease(Vec:vec) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }
    return mget(Vec_increase, _:vec);
}

stock bool:Vec_IsReadOnly(Vec:vec) {
    if(!Vec_IsValid(vec)) {
        return false;
    }
    return bool:mget(Vec_isReadOnly, _:vec);
}

stock bool:Vec_IsOrdered(Vec:vec) {
    if(!Vec_IsValid(vec)) {
        return false;
    }
    return bool:mget(Vec_isOrdered, _:vec);
}

#define Vec_GetString(%0,%1)    mget(Vec_GetVecAllocSlot((%0)), _:(%1))

stock Vec:Vec_New(capacity = VEC_DEFAULT_CAPACITY, sizeIncrease = VEC_DEFAULT_INCREASE) {
    new Alloc:a = calloc(capacity);
    new Vec:id = Vec_getNewId();
    new length = Malloc_SlotSize(Vec_vectorSlot);
    if(_:id >= length) {
        if(Vec_resizeVectorsData(_:id, length) == VEC_NOALLOC) {
            return INVALID_VECTOR_ID;
        }
    }
    mset(Vec_vectorLength, id, 0);
    mset(Vec_increase, id, sizeIncrease);
    mset(Vec_isOrdered, id, 0);
    mset(Vec_vectorSlot, id, _:a);
    mset(Vec_isSizeFixed, id, 0);
    mset(Vec_isReadOnly, id, 0);
    return id;
}

stock Vec:Vec_NewFromArray(const arr[], capacity = VEC_DEFAULT_CAPACITY, sizeIncrease = VEC_DEFAULT_INCREASE, arrSize = sizeof arr) {
    if(capacity < arrSize) {
        capacity = arrSize + sizeIncrease;
    }
    new Alloc:a = calloc(capacity);
    new Vec:id = Vec_getNewId();

    new length = Malloc_SlotSize(Vec_vectorSlot);
    if(_:id >= length) {
        if(Vec_resizeVectorsData(_:id, length) == VEC_NOALLOC) {
            return INVALID_VECTOR_ID;
        }
    }
    mset(Vec_isOrdered, id, 0);
    length = 0;
    a = Vec_internalAppendArray(a, arr, arrSize, length, sizeIncrease, false);
    mset(Vec_vectorSlot, id, _:a);
    mset(Vec_increase, id, sizeIncrease);
    mset(Vec_vectorLength, id, length);
    mset(Vec_isSizeFixed, id, 0);
    mset(Vec_isReadOnly, id, 0);
    return id;
}

stock Vec:Vec_NewString(const str[], capacity = VEC_DEFAULT_CAPACITY, sizeIncrease = VEC_DEFAULT_INCREASE, size = sizeof str) {
    new Vec:stringVec = Vec_NewFromArray(str, capacity, sizeIncrease, size);
    if(stringVec != INVALID_VECTOR_ID) {
        mset(Vec_vectorLength, _:stringVec, Vec_GetLength(stringVec)-1);
    }
    return stringVec;
}

stock Vec:Vec_Clone(Vec:vec, bool:readonly = false, start = 0, end = 0) {
    new Vec:newVec = Vec_New(Vec_GetCapacity(vec), mget(Vec_increase, _:vec));
    Vec_ToggleFixedSize(newVec, Vec_IsFixedSize(vec));
    Vec_ToggleOrdered(newVec, Vec_IsOrdered(vec));
    if(newVec == INVALID_VECTOR_ID) {
        return INVALID_VECTOR_ID;
    }

    Vec_AppendVector(newVec, vec, start, end);

    if(readonly) {
        mset(Vec_isReadOnly, _:newVec, 1);
    }
    return newVec;

}

stock Vec_GetLength(Vec:vec) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }
    return mget(Vec_vectorLength, _:vec);
}

stock Vec_GetCapacity(Vec:vec) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }
    return Malloc_SlotSize(mget(Vec_vectorSlot, _:vec));
}

stock Vec_ToggleOrdered(Vec:vec, bool:toggle) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }
    mset(Vec_isOrdered, _:vec, _:toggle);
    return VEC_OK;
}

stock Vec_ChangeIncrease(Vec:vec, increase) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }
    mset(Vec_increase, _:vec, increase);
    return VEC_OK;
}

stock Vec_ToggleReadOnly(Vec:vec, bool:toggle) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }
    mset(Vec_isReadOnly, _:vec, _:toggle);
    return VEC_OK;
}

stock Vec_ToggleFixedSize(Vec:vec, bool:toggle) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }
    mset(Vec_isSizeFixed, _:vec, _:toggle);
    return VEC_OK;
}

stock Vec_Resize(Vec:vec, newSize) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }
    if(Vec_IsReadOnly(vec) || Vec_IsFixedSize(vec)) {
        return VEC_NOP;
    }

    if(newSize < Vec_GetLength(vec)) {
        return VEC_NOALLOC;
    }

    //resize the alloc of the vec
    new Alloc:arr = Vec_internalResize(Vec_GetVecAllocSlot(vec), newSize,  Vec_GetLength(vec));
    if(arr == NO_ALLOC) {
        return VEC_NOALLOC;
    }
    //update the allocated data id for the vector
    mset(Vec_vectorSlot, _:vec, _:arr);
    return VEC_OK;
}

stock Vec_Append(Vec:vec, value) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    if(Vec_IsReadOnly(vec)) {
        return VEC_NOP;
    }

    new length = mget(Vec_vectorLength, _:vec);
    new Alloc:arr = Vec_GetVecAllocSlot(vec);
    //append the value to the alloc
    arr = Vec_internalAppend(arr, value, length, mget(Vec_increase, _:vec), Vec_IsFixedSize(vec));
    if(arr == NO_ALLOC) {
        return VEC_NOALLOC;
    }
    //update the allocated data id
    mset(Vec_vectorSlot, _:vec, _:arr);
    //update the length of the vector
    mset(Vec_vectorLength, _:vec, length);
    return VEC_OK;
}

stock Vec_AppendFloat(Vec:vec, Float:value) {
    return Vec_Append(vec, _:value);
}

stock Vec_AppendVec(Vec:vec, Vec:value) {
    return Vec_Append(vec, _:value);
}

stock Vec_AppendBool(Vec:vec, bool:value) {
    return Vec_Append(vec, _:value);
}

stock Vec_AppendArray(Vec:vec, const arr[], arrSize = sizeof arr) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    if(Vec_IsReadOnly(vec)) {
        return VEC_NOP;
    }

    new length = mget(Vec_vectorLength, _:vec);
    new Alloc:alloc = Vec_GetVecAllocSlot(vec);
    //append the value to the alloc
    alloc = Vec_internalAppendArray(alloc, arr, arrSize, length, mget(Vec_increase, _:vec), Vec_IsFixedSize(vec));
    if(alloc == NO_ALLOC) {
        return VEC_NOALLOC;
    }
    //update the allocated data id
    mset(Vec_vectorSlot, _:vec, _:alloc);
    //update the length of the vector
    mset(Vec_vectorLength, _:vec, length);
    return VEC_OK;
}

stock Vec_AppendString(Vec:vec, const str[], arrSize = sizeof str) {
    new ret = Vec_AppendArray(vec, str, arrSize);
    if(ret == VEC_OK) {
        mset(Vec_vectorLength, _:vec, Vec_GetLength(vec)-1);
    }
    return ret;
}

stock Vec_GetValue(Vec:vec, index) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }
    if(index >= Vec_GetCapacity(vec)) {
        return VEC_NOALLOC;
    }
    return mget(Vec_GetVecAllocSlot(vec), index);
}

stock bool:Vec_GetValueBool(Vec:vec, index) {
    return bool:Vec_GetValue(vec, index);
}

stock Vec:Vec_GetValueVec(Vec:vec, index) {
    return Vec:Vec_GetValue(vec, index);
}

stock bool:Vec_GetValueFloat(Vec:vec, index) {
    return Float:Vec_GetValue(vec, index);
}

stock Vec_SetValue(Vec:vec, index, value) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    if(Vec_IsReadOnly(vec)) {
        return VEC_NOP;
    }

    if(index > Vec_GetLength(vec)) {
        if(index >= Vec_GetCapacity(vec)) {
            new ret = Vec_Resize(vec, index + mget(Vec_increase, _:vec));
            if(ret != VEC_OK) {
                return ret;
            }
        }
        mset(Vec_vectorLength, _:vec, index+1);
    }
    mset(Vec_GetVecAllocSlot(vec), index, value);
    return VEC_OK;
}

stock Vec_SetValueFloat(Vec:vec, index, Float:value) {
    return Vec_SetValue(vec, index, _:value);
}

stock Vec_SetValueVec(Vec:vec, index, Vec:value) {
    return Vec_SetValue(vec, index, _:value);
}

stock Vec_SetValueBool(Vec:vec, index, bool:value) {
    return Vec_SetValue(vec, index, _:value);
}

stock Vec_RemoveAt(Vec:vec, index) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    if(Vec_IsReadOnly(vec)) {
        return VEC_NOP;
    }

    new length = Vec_GetLength(vec);
    Vec_internalRemove(Vec_GetVecAllocSlot(vec), index, Vec_IsOrdered(vec), length);
    mset(Vec_vectorLength, _:vec, length);
    return VEC_OK;
}

stock Vec_Remove(Vec:vec, value) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    if(Vec_IsReadOnly(vec)) {
        return VEC_NOP;
    }

    for(new i = 0, j = Vec_GetLength(vec); i < j; i++) {
        if(Vec_GetValue(vec, i) == value) {
            Vec_RemoveAt(vec, i);
            return VEC_OK;
        }
    }
    return VEC_NOT_FOUND;
}

stock Vec_RemoveLast(Vec:vec, value) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    if(Vec_IsReadOnly(vec)) {
        return VEC_NOP;
    }

    for(new i = Vec_GetLength(vec) - 1; i >= 0; i--) {
        if(Vec_GetValue(vec, i) == value) {
            Vec_RemoveAt(vec, i);
            return VEC_OK;
        }
    }
    return VEC_NOT_FOUND;
}

stock Vec_RemoveAll(Vec:vec, value) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    if(Vec_IsReadOnly(vec)) {
        return VEC_NOP;
    }

    for(new i = 0, j = Vec_GetLength(vec); i < j; i++) {
        if(Vec_GetValue(vec, i) == value) {
            Vec_RemoveAt(vec, i);
        }
    }
    return VEC_OK;
}

stock Vec_ChangeString(Vec:vec, const str[], len = -1) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    if(Vec_IsReadOnly(vec)) {
        return VEC_NOP;
    }

    if(len == -1) {
        len = strlen(str);
    }

    //reset vector's length and append string at the start
    mset(Vec_vectorLength, _:vec, 0);
    if (Vec_AppendArray(vec, str, len) == VEC_NOALLOC) {
        return VEC_NOALLOC;
    }
    //set a null terminator after string
    Vec_SetValue(vec, mget(Vec_vectorLength, _:vec), '\0');
    return VEC_OK;
}

stock Vec_AppendVector(Vec:vec, Vec:toAppend, start = 0, end = 0) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    if(end == 0) {
        end = Vec_GetLength(toAppend);
    }

    if(Vec_IsReadOnly(vec)) {
        return VEC_NOP;
    }

    new len = Vec_GetLength(vec);

    if(len + end - start >= Vec_GetCapacity(vec)) {
        new ret = Vec_Resize(vec, len + end - start + mget(Vec_increase, _:vec));
        if(ret != VEC_OK) {
            return ret;
        }
    }
    new Alloc:slot = Vec_GetVecAllocSlot(vec), Alloc:slotToAppend = Vec_GetVecAllocSlot(toAppend);

    Malloc_SetA(slot, len, mget(slotToAppend, start), end);
    mset(Vec_vectorLength, _:vec, len + end - start + 1);
    return VEC_OK;
}

stock Vec_CopyTo(Vec:vec, dest[], start = 0, end = 0, maxSize = sizeof(dest)) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    if(end == 0 || end > maxSize) {
        end = maxSize;
    }
    memcpy(dest, mget(Vec_GetVecAllocSlot(vec), start), 0, (end - start)*4, end);
    return VEC_OK;
}

stock Vec_Reverse(Vec:vec, start = 0, end = 0) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    if(Vec_IsReadOnly(vec)) {
        return VEC_NOP;
    }

    if(end == 0) {
        end = Vec_GetLength(vec)-1;
    }

    if(start == end) {
        return VEC_NOP;
    }

    if(start > end) {
        new tmp = end;
        end = start;
        start = tmp;
    }

    for(new i = start, tmp, j = end; i < j; i++, j--) {
        tmp = Vec_GetValue(vec, j);
        Vec_SetValue(vec, j, Vec_GetValue(vec, i));
        Vec_SetValue(vec, i, tmp);
    }
    return VEC_OK;
}

stock Vec_SetArray(Vec:vec, index, const array[], start = 0, end = 0, size = sizeof(array)) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    if(Vec_IsReadOnly(vec)) {
        return VEC_NOP;
    }

    if(end == 0 || end > size) {
        end = size-1;
    }

    if(start > end) {
        new tmp = end;
        end = start;
        start = tmp;
    }

    if(start == end) {
        return Vec_SetValue(vec, index, array[start]);
    }

    if(index + end - start >= Vec_GetCapacity(vec)) {
        new ret = Vec_Resize(vec, index + end - start + mget(Vec_increase, _:vec));
        if(ret != VEC_OK) {
            return ret;
        }
    }

    mseta(Vec_GetVecAllocSlot(vec), index, array[start], end);
    return VEC_OK;
}

stock bool:Vec_Find(Vec:vec, value, &index = 0) {
    if(!Vec_IsValid(vec)) {
        return false;
    }

    for(new i = 0; i < Vec_GetLength(vec); i++) {
        if(Vec_GetValue(vec, i) == value) {
            index = i;
            return true;
        }
    }
    index = -1;
    return false;
}

stock Vec_FindLast(Vec:vec, value, &index = 0) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    for(new i = Vec_GetLength(vec) - 1; i >= 0; i--) {
        if(Vec_GetValue(vec, i) == value) {
            index = i;
            return VEC_OK;
        }
    }
    return VEC_NOT_FOUND;
}

stock Vec:Vec_FindAll(Vec:vec, value) {
    if(!Vec_IsValid(vec)) {
        return INVALID_VECTOR_ID;
    }

    new Vec:indexVector = Vec_New(2);
    for(new i = 0; i < Vec_GetLength(vec); i++) {
        if(Vec_GetValue(vec, i) == value) {
            Vec_Append(indexVector, i);
        }
    }
    if(Vec_GetLength(indexVector) == 0) {
        Vec_Delete(indexVector);
        indexVector = INVALID_VECTOR_ID;
    }
    return indexVector;
}

stock Vec_Clear(Vec:vec) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }
    memset(mget(Vec_GetVecAllocSlot(vec), 0), 0, Vec_GetLength(vec));
    mset(Vec_vectorLength, _:vec, 0);
}

stock Vec_Delete(Vec:vec) {
    if(!Vec_IsValid(vec)) {
        return VEC_INVALID;
    }

    Vec_ReleaseId(vec);
    Malloc_Free(Vec_GetVecAllocSlot(vec));
    mset(Vec_vectorLength, _:vec, 0);
    mset(Vec_isOrdered, _:vec, 0);
    mset(Vec_vectorSlot, _:vec, 0);

    return VEC_OK;
}