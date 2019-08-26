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

enum _:e_VEC_ERRORS {
    VEC_OK = 0,
    VEC_NOALLOC,
    VEC_INVALID
}


#define INVALID_VECTOR_ID           (Vec:-1)

static  Alloc:Vec_vectorLength;
static  Alloc:Vec_isVectorOrdered;
static  Alloc:Vec_vectorSlot;

//highest id assigned
static  Vec:Vec_LastID;

//array with ids that were in use and then released
static  Alloc:Vec_releasedIDs;

//index of the last released id
static  Vec_lastReleasedID = -1;


static stock Alloc:Vec_internalResize(Alloc:slot, newSize, length) {
    //alloc more memory with the new size
    new Alloc:newArr = malloc(newSize);
    if(newArr != NO_ALLOC) {
        //copy last alloc to the newly allocated data
        Malloc_SetA(newArr, 0, mget(slot, 0), length);        
        //free old memory
        Malloc_Free(slot);
    }
    return newArr;
}

static stock Alloc:Vec_internalAppend(Alloc:slot, value, &length) {
    if(length < Malloc_SlotSize(slot)) {
        //if there is enough space for the new value just set it at the end
        mset(slot, length, value);
        length++;
        return slot;
    }
    //if there isn't enough space resize the alloc and set the value at the end of the new alloc
    new Alloc:newArr = Vec_internalResize(slot, Malloc_SlotSize(slot) + VEC_DEFAULT_INCREASE, Malloc_SlotSize(slot));
    if(newArr != NO_ALLOC) {
        mset(slot, length, value);
        length++;
    }
    //will return the new Alloc slot if Vec_internalResize succeeded or NO_ALLOC otherwise
    return newArr;
}

static stock Alloc:Vec_internalAppendArray(Alloc:slot, const arr[], arrSize, &length) {
    if(length+arrSize < Malloc_SlotSize(slot)) {
        //if there is enough space for the new value just set it at the end
        Malloc_SetA(slot, length, arr, arrSize);
        length += arrSize;
        return slot;
    }
    //if there isn't enough space resize the alloc and set the value at the end of the new alloc
    new Alloc:newArr = Vec_internalResize(slot, length + arrSize + VEC_DEFAULT_INCREASE, Malloc_SlotSize(slot));
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
        return VEC_OK;
    }
    for(new i = index; i < length; i++) {
        mset(slot, i, mget(slot, i+1));
    }
    length--;
    return VEC_OK;
}

static stock Vec:Vec_getNewId() {
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
        //if the id wasn't yet assigned return false
        return false;
    }
    for(new i = 0; i <= _:Vec_lastReleasedID; i++) {
        if(mget(Vec_releasedIDs, i) == _:vec) {
            //if the id is released return false
            return false;
        }
    }
    //otherwise return true
    return true;
}

static Vec_ReleaseId(Vec:id) {
    if(!IsValidVec(id)) {
        return VEC_INVALID;
    }
    if(id == Vec_LastID-Vec:1) {
        //if the id is the last assigned just decrease the last assigned id
        Vec_LastID--;
        return VEC_OK;
    }
    //if none of the others, add the id to the free ids list
    Vec_lastReleasedID++;
    new Alloc:ret = Vec_internalAppend(Vec_releasedIDs, _:id, Vec_lastReleasedID);
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
    ret = Vec_internalResize(Vec_isVectorOrdered, id + VEC_DEFAULT_INCREASE, length);
    if(ret == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate more memory for storing vector ordered status");
        return VEC_NOALLOC;
    }
    Vec_isVectorOrdered = ret;
    ret = Vec_internalResize(Vec_vectorSlot, id + VEC_DEFAULT_INCREASE, length);
    if(ret == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate more memory for vectors");
        return VEC_NOALLOC;
    }
    Vec_vectorSlot = ret;
    return VEC_OK;
}

stock Alloc:Vec_GetVecAllocSlot(Vec:vec) {
    return Alloc:mget(Vec_vectorSlot, _:vec);
}

#include <YSI\y_hooks>
hook OnScriptInit() {
    Vec_vectorLength = malloc(10);
    if(Vec_vectorLength == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate memory for the vector's length");
    }
    Vec_vectorSlot = malloc(10);
    if(Vec_vectorSlot == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate memory for vectors");
    }
    Vec_isVectorOrdered = malloc(10);
    if(Vec_isVectorOrdered == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate memory for storing vector ordered status");
    }
    Vec_releasedIDs = malloc(10);
    if(Vec_releasedIDs == NO_ALLOC) {
        P:F("PAWN-VECTOR couldn't allocate memory for released ids");
    }
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

#define Vec_GetString(%0,%1)    mget(Vec_GetVecAllocSlot((%0)), _:(%1))

stock Vec:Vec_New(capacity = VEC_DEFAULT_CAPACITY, bool:ordered = false) {
    new Alloc:a = malloc(capacity);
    new Vec:id = Vec_getNewId();
    new length = Malloc_SlotSize(Vec_vectorSlot);
    if(_:id >= length) {
        if(Vec_resizeVectorsData(_:id, length) == VEC_NOALLOC) {
            return INVALID_VECTOR_ID;
        }
    }
    mset(Vec_vectorLength, id, 0);
    mset(Vec_isVectorOrdered, id, _:ordered);
    mset(Vec_vectorSlot, id, _:a);
    return id;
}

stock Vec:Vec_NewFromArray(const arr[], arrSize, capacity = VEC_DEFAULT_CAPACITY, bool:ordered = false) {
    if(capacity < arrSize) {
        capacity = arrSize + VEC_DEFAULT_INCREASE;
    }
    new Alloc:a = malloc(capacity);
    new Vec:id = Vec_getNewId();

    new length = Malloc_SlotSize(Vec_vectorSlot);
    if(_:id >= length) {
        if(Vec_resizeVectorsData(_:id, length) == VEC_NOALLOC) {    
            return INVALID_VECTOR_ID;
        }
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
        return -1;
    }
    return mget(Vec_vectorLength, _:vec);
}

stock Vec_GetCapacity(Vec:vec) {
    if(!IsValidVec(vec)) {
        return -1;
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
        return VEC_INVALID;
    }
    mset(Vec_isVectorOrdered, _:vec, _:toggle);
    return VEC_OK;
}

stock Vec_Resize(Vec:vec, newSize) {
    if(!IsValidVec(vec)) {
        return VEC_INVALID;
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
    if(!IsValidVec(vec)) {
        return VEC_INVALID;
    }
    new length = mget(Vec_vectorLength, _:vec);
    new Alloc:arr = Vec_GetVecAllocSlot(vec);
    //append the value to the alloc
    arr = Vec_internalAppend(arr, value, length);
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

stock Vec:AppendBool(Vec:vec, bool:value) {
    return Vec_Append(vec, _:value); 
}

stock Vec_AppendArray(Vec:vec, const arr[], arrSize = sizeof arr) {
    if(!IsValidVec(vec)) {
        return VEC_INVALID;
    }
    new length = mget(Vec_vectorLength, _:vec);
    new Alloc:alloc = Vec_GetVecAllocSlot(vec);
    //append the value to the alloc
    alloc = Vec_internalAppendArray(alloc, arr, arrSize, length);
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
    return Vec_AppendArray(vec, str, arrSize);
}

stock Vec_GetValue(Vec:vec, index) {
    if(!IsValidVec(vec)) {
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
    if(!IsValidVec(vec)) {
        return VEC_INVALID;
    }
    if(index > Vec_GetLength(vec)) {
        if(index >= Vec_GetCapacity(vec)) {            
            if(Vec_Resize(vec, index + VEC_DEFAULT_INCREASE) == VEC_NOALLOC) {
                return VEC_NOALLOC;
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

stock Vec_Remove(Vec:vec, index) {
    if(!IsValidVec(vec)) {
        return VEC_INVALID;
    }
    new length = Vec_GetLength(vec);
    Vec_internalRemove(Vec_GetVecAllocSlot(vec), index, Vec_IsOrdered(vec), length);
    mset(Vec_vectorLength, _:vec, length);
    return VEC_OK;
}

stock Vec_ChangeString(Vec:vec, const str[], len = -1) {
    if(!IsValidVec(vec)) {
        return VEC_INVALID;
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

stock Vec_Delete(Vec:vec) {
    if(!IsValidVec(vec)) {
        return VEC_INVALID;
    }
    Vec_ReleaseId(vec);
    new Alloc:slot = Vec_GetVecAllocSlot(vec);
    Malloc_Free(slot);
    mset(Vec_vectorLength, _:vec, 0);
    mset(Vec_isVectorOrdered, _:vec, 0);
    mset(Vec_vectorSlot, _:vec, 0);
    return VEC_OK;
}
