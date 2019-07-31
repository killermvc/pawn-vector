#include <YSI\y_malloc>

#if !defined VEC_DEFAULT_CAPACITY
    #define VEC_DEFAULT_CAPACITY    (50)
#endif


#if !defined VEC_DEFAULT_INCREASE
    #define VEC_DEFAULT_INCREASE    (5)
#endif

/*
    internal
*/

static  Alloc:Vec_vectorLength;
static  Alloc:Vec_isVectorOrdered;
static  Alloc:Vec_vectorSlot;

//highest id assigned
static  Vec:Vec_LastID;

//array with ids that were in use and then released
static  Alloc:Vec_releasedIDs;

//index of the last released id
static  Vec_lastReleasedID = -1;


static Alloc:Vec_internalResize(Alloc:slot, newSize, length) {
    //alloc more memory with the new size
    new Alloc:newArr = malloc(newSize);
    //copy last alloc to the newly allocated data
    Malloc_SetA(newArr, 0, mget(slot, 0), length);
    
    //free old memory
    Malloc_Free(slot);

    return newArr;
}

static Alloc:Vec_internalAppend(Alloc:slot, value, &length) {
    if(length < Malloc_SlotSize(slot)) {
        //if there is enough space for the new value just set it at the end
        mset(slot, length, value);
        length++;

        return slot;
    }
    //if there isn't enough space resize the alloc and set the value at the end of the new alloc
    new Alloc:newArr = Vec_internalResize(slot, Malloc_SlotSize(slot) + VEC_DEFAULT_INCREASE, Malloc_SlotSize(slot));
    mset(slot, length, value);
    length++;
    return newArr;
}

static Alloc:Vec_internalAppendArray(Alloc:slot, const arr[], arrSize, &length) {
    if(length+arrSize < Malloc_SlotSize(slot)) {
        //if there is enough space for the new value just set it at the end
        Malloc_SetA(slot, length, arr, arrSize);
        length += arrSize;
        return slot;
    }
    //if there isn't enough space resize the alloc and set the value at the end of the new alloc
    new Alloc:newArr = Vec_internalResize(slot, length + arrSize + VEC_DEFAULT_INCREASE, Malloc_SlotSize(slot));
    Malloc_SetA(newArr, length, arr, arrSize);
    length += arrSize;
    return newArr;
}

static Vec_internalRemove(Alloc:slot, index, bool:ordered, &length) {
    if(index > length) {
        return 0;
    }
    if(!ordered) {
        mset(slot, index, mget(slot, length-1));
        length--;
        return 1;
    }
    for(new i = index; i < length; i++) {
        mset(slot, i, mget(slot, i+1));
    }
    length--;
    return 1;
}

static Vec:Vec_getNewId() {
    if(Vec_lastReleasedID >= 0) {
        //if there are released ids return one of them

        new id = mget(Vec_releasedIDs, Vec_lastReleasedID);
        Vec_lastReleasedID--;
        return Vec:id;
    }
    //else return a new id
    Vec_LastID++;
    return Vec_LastID-Vec:1;
}

bool:IsValidVec(Vec:vec) {
    if(vec >= Vec_LastID) {

        return false;
    }
    for(new i = 0; i <= _:Vec_lastReleasedID; i++) {
        if(mget(Vec_releasedIDs, i) == _:vec) {
            return false;
        }
    }
    return true;
}

static Vec_ReleaseId(Vec:id) {
    if(!IsValidVec(id)) {
        return 0;
    }
    if(id == Vec_LastID-Vec:1) {
        //if the id is the last assigned just decrease the last assigned id
        Vec_LastID--;
        return 1;
    }
    //if none of the others, add the id to the free ids list
    Vec_lastReleasedID++;
    Vec_internalAppend(Vec_releasedIDs, _:id, Vec_lastReleasedID);
    return 1;
}



Alloc:Vec_GetVecAllocSlot(Vec:vec) {
    return Alloc:mget(Vec_vectorSlot, _:vec);
}

#include <YSI\y_hooks>
hook OnScriptInit() {
    Vec_vectorLength = malloc(10);
    Vec_vectorSlot = malloc(10);
    Vec_isVectorOrdered = malloc(10);
    Vec_releasedIDs = malloc(10);
    return 1;
}

hook OnScriptExit() {
    free(Vec_releasedIDs);
    free(Vec_vectorLength);    
    free(Vec_isVectorOrdered);
    for(new i = 0; i < _:Vec_LastID; i++) {
        free(Alloc:mget(Vec_vectorSlot, i));
    }
    free(Vec_vectorSlot);
    return 1;
}


/*
    api
*/

stock Vec:Vec_CreateVector(capacity = VEC_DEFAULT_CAPACITY, bool:ordered = false) {
    new Alloc:a = malloc(capacity);
    new Vec:id = Vec_getNewId();
    new length = Malloc_SlotSize(Vec_vectorSlot);
    if(_:id >= length) {
        Vec_internalResize(Vec_vectorLength, _:id + VEC_DEFAULT_INCREASE, length);
        Vec_internalResize(Vec_isVectorOrdered, _:id + VEC_DEFAULT_INCREASE, length);
        Vec_internalResize(Vec_vectorSlot, _:id + VEC_DEFAULT_INCREASE, length);
    }
    mset(Vec_vectorLength, id, 0);
    mset(Vec_isVectorOrdered, id, _:ordered);
    mset(Vec_vectorSlot, id, _:a);
    return id;
}

stock Vec:Vec_CreateVectorFromArray(const arr[], arrSize, capacity = VEC_DEFAULT_CAPACITY, bool:ordered = false) {
    if(capacity < arrSize) {
        capacity = arrSize + VEC_DEFAULT_INCREASE;
    }
    new Alloc:a = malloc(capacity);
    new Vec:id = Vec_getNewId();

    new length = Malloc_SlotSize(Vec_vectorSlot);
    if(_:id >= length) {
        Vec_internalResize(Vec_vectorLength, _:id + VEC_DEFAULT_INCREASE, length);
        Vec_internalResize(Vec_isVectorOrdered, _:id + VEC_DEFAULT_INCREASE, length);
        Vec_internalResize(Vec_vectorSlot, _:id + VEC_DEFAULT_INCREASE, length);
    }        
    mset(Vec_isVectorOrdered, id, _:ordered);
    length = 0;
    a = Vec_internalAppendArray(a, arr, arrSize, length);
    mset(Vec_vectorSlot, id, _:a);
    mset(Vec_vectorLength, id, length);
    return id;
}

stock Vec_GetLength(Vec:vec) {
    if(!IsValidVec(vec)) {
        return 0;
    }
    return mget(Vec_vectorLength, _:vec);
}

stock Vec_GetCapacity(Vec:vec) {
    if(!IsValidVec(vec)) {
        return 0;
    }
    return Malloc_SlotSize(mget(Vec_vectorSlot, _:vec));
}

stock bool:Vec_IsOrdered(Vec:vec) {
    if(!IsValidVec(vec)) {
        return false;
    }
    return bool:mget(Vec_isVectorOrdered, _:vec);
}

stock bool:Vec_ToggleOrdered(Vec:vec, bool:toggle) {
    if(!IsValidVec(vec)) {
        return 0;
    }
    mset(Vec_isVectorOrdered, _:vec, _:toggle);
    return 1;
}

stock Vec_Resize(Vec:vec, newSize) {
    if(!IsValidVec(vec)) {
        return 0;
    }
    if(newSize < Vec_GetLength(vec)) {
        return 0;
    }
    //resize the alloc of the vec
    new Alloc:arr = Vec_internalResize(Vec_GetVecAllocSlot(vec), newSize,  Vec_GetLength(vec));
    //update the allocated data id for the vector
    mset(Vec_vectorSlot, _:vec, _:arr);
    return 1;
}

stock Vec_Append(Vec:vec, value) {
    if(!IsValidVec(vec)) {
        return 0;
    }
    new length = mget(Vec_vectorLength, _:vec);
    new Alloc:arr = Vec_GetVecAllocSlot(vec);
    //append the value to the alloc
    arr = Vec_internalAppend(arr, value, length);
    //update the allocated data id
    mset(Vec_vectorSlot, _:vec, _:arr);
    //update the length of the vector
    mset(Vec_vectorLength, _:vec, length);
    return 1;
}

stock Vec_AppendFloat(Vec:vec, Float:value) {
    return Vec_Append(vec, _:value);
}

stock Vec:AppendBool(Vec:vec, bool:value) {
    return Vec_Append(vec, _:value); 
}

stock Vec_AppendArray(Vec:vec, const arr[], arrSize = sizeof arr) {
    if(!IsValidVec(vec)) {
        return 0;
    }
    new length = mget(Vec_vectorLength, _:vec);
    new Alloc:alloc = Vec_GetVecAllocSlot(vec);
    //append the value to the alloc
    alloc = Vec_internalAppendArray(alloc, arr, arrSize, length);
    //update the allocated data id
    mset(Vec_vectorSlot, _:vec, _:alloc);
    //update the length of the vector
    mset(Vec_vectorLength, _:vec, length);
    return 1;
}

stock Vec_GetValue(Vec:vec, index) {
    /*if(!IsValidVec(vec)) {
        return 0;
    }
    if(index > Vec_GetLength(vec)) {
        //if the index wasn't set yet dont try to acces it
        return 0;
    }*/
    return mget(Vec_GetVecAllocSlot(vec), index);
}

stock bool:Vec_GetValueBool(Vec:vec, index) {
    return bool:Vec_GetValue(vec, index);
}

stock bool:Vec_GetValueFloat(Vec:vec, index) {
    return Float:Vec_GetValue(vec, index);
}

stock Vec_GetArray(Vec:vec, index, dest[], length) {
    return Malloc_GetS(dest, length, Vec_GetVecAllocSlot(vec), index, false);
}

stock Vec_SetValue(Vec:vec, index, value) {
    if(!IsValidVec(vec)) {
        return 0;
    }
    if(index > Vec_GetLength(vec)) {
        //if the index wasn't set yet dont try to acces it
        return 0;
    }
    mset(Vec_GetVecAllocSlot(vec), index, value);
    return 1;
}

stock Vec_SetValueFloat(Vec:vec, index, Float:value) {
    return Vec_SetValue(vec, index, _:value);
}

stock Vec_SetValueBool(Vec:vec, index, bool:value) {
    return Vec_SetValue(vec, index, _:value);
}

stock Vec_Remove(Vec:vec, index) {
    if(!IsValidVec(vec)) {
        return 0;
    }
    new length = Vec_GetLength(vec);
    Vec_internalRemove(Vec_GetVecAllocSlot(vec), index, Vec_IsOrdered(vec), length);
    mset(Vec_vectorLength, _:vec, length);
    return 1;
}

stock Vec_Delete(Vec:vec) {
    if(!IsValidVec(vec)) {
        return 0;
    }
    Vec_ReleaseId(vec);
    new Alloc:slot = Vec_GetVecAllocSlot(vec);
    Malloc_Free(slot);
    mset(Vec_vectorLength, _:vec, 0);
    mset(Vec_isVectorOrdered, _:vec, 0);
    mset(Vec_vectorSlot, _:vec, 0);
    return 1;
}

#define Vec_GetString(%0,%1)    mget(Vec_GetVecAllocSlot((%0)), _:(%1))