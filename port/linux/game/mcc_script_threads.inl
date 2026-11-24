/* (port, this fork: as MCC's units, mcc_cache.c's MCC_CACHE_64BIT_REFUSED) */
#if defined(HALO_64BIT) && defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-conversion"
#pragma clang diagnostic ignored "-Wint-to-pointer-cast"
#pragma clang diagnostic ignored "-Wpointer-to-int-cast"
#pragma clang diagnostic ignored "-Wpointer-integer-compare"
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
#endif
/* Included by hs_runtime.c after its private types. MCC threads use a bounded
 * array in checked, otherwise unused CPU-image space. Inline native stacks,
 * allocation records and Xbox/CE checkpoints keep their original layout. */
#include "mcc_script_threads.h"
#include "mcc_checkpoint.h"
#include "mcc_campaign.h"
#include "saved games/game_state.h"
#include "../../third_party/zlib/zlib_prefixed.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MCC_SCRIPT_STACK_BYTES 4096
#define MCC_SCRIPT_STACK_BYTES_COMPACT 3456
#define MCC_SCRIPT_THREAD_COUNT 256
#define MCC_SCRIPT_THREAD_BYTES (offsetof(struct hs_thread_datum,stack_data)+MCC_SCRIPT_STACK_BYTES)
#define MCC_SCRIPT_THREAD_STORAGE_FOR(stride) (16+sizeof(struct data_array)+MCC_SCRIPT_THREAD_COUNT*(stride))
#define MCC_SCRIPT_THREAD_STORAGE MCC_SCRIPT_THREAD_STORAGE_FOR(MCC_SCRIPT_THREAD_BYTES)

static struct {
    byte *storage, *original;
    struct data_array *array, *native;
    unsigned long stride, bytes;
} mcc_threads;

static byte *mcc_hs_stack_end(struct hs_thread_datum *thread)
{
    if (mcc_threads.array && hs_thread_data==mcc_threads.array)
        return (byte *)thread+mcc_threads.stride;
    return thread->stack_data+HS_THREAD_STACK_SIZE;
}

static unsigned long mcc_threads_checksum(byte const *bytes, unsigned long count)
{
    return (uint32_t)crc32(0, bytes, (uInt)count);
}

int mcc_script_threads_prepare(int scriptless)
{
    unsigned long used,capacity;
    void *image=game_state_port_cpu_region(&used,&capacity);
    unsigned long stride=MCC_SCRIPT_THREAD_BYTES, bytes=MCC_SCRIPT_THREAD_STORAGE;
    byte *storage=mcc_checkpoint_script_storage(image,used,capacity,bytes);
    if (mcc_threads.native) return 0;
    if (!hs_thread_data || hs_thread_data->size!=sizeof(struct hs_thread_datum) ||
        hs_thread_data->maximum_count!=MCC_SCRIPT_THREAD_COUNT) {
        error(_error_silent,"mcc scripts: unexpected native thread array");
        return 0;
    }
    if (!storage) {
        /* Larger native pools can leave too little checkpoint tail for 4 KiB
         * per thread. Keep every thread and both checkpoint slots, using the
         * smaller MCC stride only when the original layout cannot fit. */
        stride=offsetof(struct hs_thread_datum,stack_data)+MCC_SCRIPT_STACK_BYTES_COMPACT;
        bytes=MCC_SCRIPT_THREAD_STORAGE_FOR(stride);
        storage=mcc_checkpoint_script_storage(image,used,capacity,bytes);
    }
    if (!storage) {
        /* Maps without authored scripts or globals can use the native console
         * threads. Preserve the larger stacks and checkpoint layout whenever
         * the expanded array fits, and require it for authored scripts. */
        if (scriptless) {
            mcc_threads.native=hs_thread_data;
            return 1;
        }
        error(_error_silent,"mcc scripts: cannot fit %lu bytes in CPU image tail (used %lu, capacity %lu)",
            bytes,used,capacity);
        return 0;
    }
    mcc_threads.original=malloc(bytes);
    if (!mcc_threads.original) {
        error(_error_silent,"mcc scripts: could not retain the original CPU image tail");
        return 0;
    }
    memcpy(mcc_threads.original,storage,bytes);
    memset(storage,0,bytes);
    mcc_threads.storage=storage;
    mcc_threads.stride=stride;
    mcc_threads.bytes=bytes;
    mcc_threads.native=hs_thread_data;
    mcc_threads.array=(struct data_array *)(storage+16);
    data_initialize(mcc_threads.array,"MCC hs thread",MCC_SCRIPT_THREAD_COUNT,(short)stride);
    return 1;
}

static void mcc_hs_threads_activate(void)
{
    if (!mcc_cache_tags_loaded()) return;
    mcc_campaign_reset_model_lighting();
    if (mcc_threads.array) {
        data_make_invalid(mcc_threads.native);
        hs_thread_data=mcc_threads.array;
    }
}

void mcc_script_threads_dispose(void)
{
    if (mcc_threads.storage) {
        if (hs_thread_data==mcc_threads.array) {
            hs_thread_data=mcc_threads.native;
            /* An old checkpoint may have made the dormant native array valid. */
            data_make_invalid(mcc_threads.native);
        }
        memcpy(mcc_threads.storage,mcc_threads.original,mcc_threads.bytes);
        free(mcc_threads.original);
    }
    memset(&mcc_threads,0,sizeof(mcc_threads));
}

/* Saved frame pointers refer to the fixed live CPU arena. Check against that
 * thread's own stack, not against the whole image or another thread's buffer. */
static int mcc_threads_thread_valid(struct hs_thread_datum const *thread,
    struct hs_thread_datum const *live, unsigned long stack_bytes)
{
    uintptr_t first=(uintptr_t)live->stack_data, end=first+stack_bytes;
    uintptr_t cursor=(uintptr_t)thread->stack;
    unsigned long steps=0;
    if (thread->type>=NUMBER_OF_HS_THREAD_TYPES || (thread->flags & ~7u) ||
        thread->sleep_until<NONE-1) return 0;
    for (;;) {
        struct hs_stack_frame const *frame;
        uintptr_t previous,result;
        if (cursor<first || cursor>end-sizeof(*frame) || ++steps>stack_bytes/sizeof(*frame)) return 0;
        frame=(struct hs_stack_frame const *)(thread->stack_data+(cursor-first));
        if (frame->size<0 || (unsigned long)frame->size>end-cursor-offsetof(struct hs_stack_frame,data)) return 0;
        if (cursor==first) return frame->previous==NULL;
        previous=(uintptr_t)frame->previous;
        if (previous<first || previous>cursor-sizeof(*frame)) return 0;
        frame=(struct hs_stack_frame const *)(thread->stack_data+(previous-first));
        if (frame->size<0 || previous+sizeof(*frame)+(unsigned long)frame->size!=cursor) return 0;
        result=(uintptr_t)frame->result;
        if (result!=(uintptr_t)&live->result &&
            (frame->size<sizeof(long) || result<previous+offsetof(struct hs_stack_frame,data) ||
             result>previous+offsetof(struct hs_stack_frame,data)+frame->size-sizeof(long))) return 0;
        cursor=previous;
    }
}

static int mcc_threads_array_valid(struct data_array const *array,
    struct data_array const *live, unsigned long stride)
{
    short i,actual=0;
    if (array->signature!='d@t@' || array->maximum_count!=MCC_SCRIPT_THREAD_COUNT ||
        array->size!=stride || array->valid!=1 || array->identifier_zero_invalid>1 ||
        !array->next_identifier || !memchr(array->name,0,sizeof(array->name)) ||
        xbox_pointer(array->data)!=live+1 || array->count<0 || array->count>MCC_SCRIPT_THREAD_COUNT ||
        array->actual_count<0 || array->actual_count>array->count ||
        array->first_free_absolute_index<0 || array->first_free_absolute_index>MCC_SCRIPT_THREAD_COUNT) return 0;
    for (i=0;i<MCC_SCRIPT_THREAD_COUNT;i++) {
        struct hs_thread_datum const *thread=(void const *)((byte const *)(array+1)+i*stride);
        struct hs_thread_datum const *target=(void const *)((byte const *)(live+1)+i*stride);
        if (!thread->identifier) continue;
        if (i>=array->count) return 0;
        actual++;
        if (!mcc_threads_thread_valid(thread,target,stride-offsetof(struct hs_thread_datum,stack_data))) return 0;
    }
    return actual==array->actual_count;
}

static void mcc_threads_stamp(byte *storage)
{
    unsigned long *header=(unsigned long *)storage;
    header[0]=0x5453434Du; /* MCST */
    header[1]=1;
    header[2]=mcc_threads.bytes;
    header[3]=mcc_threads_checksum(storage+16,mcc_threads.bytes-16);
}

int mcc_script_threads_capture(void)
{
    if (!mcc_threads.array)
        return mcc_threads.native && hs_thread_data==mcc_threads.native &&
            mcc_threads_array_valid(mcc_threads.native,mcc_threads.native,sizeof(struct hs_thread_datum));
    if (hs_thread_data!=mcc_threads.array ||
        !mcc_threads_array_valid(mcc_threads.array,mcc_threads.array,mcc_threads.stride)) return 0;
    mcc_threads_stamp(mcc_threads.storage);
    return 1;
}

static int mcc_native_threads_validate(void const *image, void const *current, unsigned long used)
{
    uintptr_t offset;
    struct data_array const *incoming;
    if (!mcc_threads.native || !image || !current) return 0;
    offset=(uintptr_t)mcc_threads.native-(uintptr_t)current;
    if (offset>used || sizeof(struct data_array)+MCC_SCRIPT_THREAD_COUNT*sizeof(struct hs_thread_datum)>used-offset)
        return 0;
    incoming=(void const *)((byte const *)image+offset);
    return mcc_threads_array_valid(incoming,mcc_threads.native,sizeof(struct hs_thread_datum));
}

int mcc_script_threads_validate(void const *image, void const *current,
    unsigned long used, unsigned long capacity)
{
    byte *storage=mcc_checkpoint_script_storage((void *)image,used,capacity,mcc_threads.bytes);
    unsigned long const *header=(unsigned long const *)storage;
    struct data_array *array;
    if (!mcc_threads.array) return mcc_native_threads_validate(image,current,used);
    if (!storage ||
        mcc_checkpoint_script_storage((void *)current,used,capacity,mcc_threads.bytes)!=mcc_threads.storage) return 0;
    array=(struct data_array *)(storage+16);
    if (header[0]==0x5453434Du)
        return header[1]==1 && header[2]==mcc_threads.bytes &&
            header[3]==mcc_threads_checksum(storage+16,mcc_threads.bytes-16) &&
            mcc_threads_array_valid(array,mcc_threads.array,mcc_threads.stride);
    /* Old checkpoints used only the native inline stacks. Validate them first,
     * then copy their live frames and relocate only intra-thread pointers. */
    return mcc_native_threads_validate(image,current,used);
}

int mcc_script_threads_restore(void *image, unsigned long used, unsigned long capacity)
{
    byte *storage=mcc_checkpoint_script_storage(image,used,capacity,mcc_threads.bytes);
    struct data_array *array;
    struct data_array const *native=mcc_threads.native;
    short i;
    if (!mcc_script_threads_validate(image,image,used,capacity)) return 0;
    if (!mcc_threads.array) return 1;
    array=(void *)(storage+16);
    if (*(unsigned long *)storage==0x5453434Du) return 1;
    {
        memset(storage,0,mcc_threads.bytes);
        memcpy(array,native,sizeof(*array));
        array->size=(short)mcc_threads.stride;
        array->data=xbox_address(mcc_threads.array+1);
        for (i=0;i<native->count;i++) {
            struct hs_thread_datum const *old=(void const *)((byte const *)(native+1)+i*sizeof(*old));
            struct hs_thread_datum const *old_live=(void const *)((byte const *)(mcc_threads.native+1)+i*sizeof(*old));
            struct hs_thread_datum *new_thread=(void *)((byte *)(array+1)+i*mcc_threads.stride);
            struct hs_thread_datum *new_live=(void *)((byte *)(mcc_threads.array+1)+i*mcc_threads.stride);
            uintptr_t offset,cursor,delta=(uintptr_t)new_live-(uintptr_t)old_live;
            memcpy(new_thread,old,sizeof(*old));
            if (!old->identifier) continue;
            cursor=(uintptr_t)old->stack;
            new_thread->stack=(void *)(cursor+delta);
            while (cursor!=(uintptr_t)old_live->stack_data) {
                struct hs_stack_frame *frame;
                offset=cursor-(uintptr_t)old_live;
                frame=(void *)((byte *)new_thread+offset);
                cursor=(uintptr_t)frame->previous;
                frame->previous=(void *)(cursor+delta);
                frame=(void *)((byte *)new_thread+(cursor-(uintptr_t)old_live));
                frame->result=(void *)((uintptr_t)frame->result+delta);
            }
        }
    }
    mcc_threads_stamp(storage);
    return mcc_threads_array_valid(array,mcc_threads.array,mcc_threads.stride);
}
#if defined(HALO_64BIT) && defined(__clang__)
#pragma clang diagnostic pop
#endif
