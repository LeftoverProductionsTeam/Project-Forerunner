/* Native map unload must respect Halo's non-null debug_free contract. */
#include "harness.h"

static void **ce_vertex_memory;
static long ce_vertex_memory_count;
static void const **ce_multipurpose_bitmaps;
static long ce_multipurpose_bitmap_count;
static struct {void const *bitmap;void *copy;} *ce_multipurpose_copies;
static long ce_multipurpose_copy_count;
static void *allocations[16];
static int allocated,released;

static void *allocate(size_t bytes) {
    void *pointer=calloc(1,bytes);CHECK(pointer&&allocated<16,"allocation");
    allocations[allocated++]=pointer;return pointer;
}
static void checked_free(void *pointer) {
    int i;CHECK(pointer,"native allocator rejects free(NULL)");
    for(i=0;i<allocated;i++)if(allocations[i]==pointer) {
        allocations[i]=NULL;released++;free(pointer);return;
    }
    CHECK(FALSE,"unknown or repeated release");
}
#define free checked_free
#define XPhysicalFree checked_free
#include "under_test.inc"
#undef free

int main(int argc,char **argv) {
    (void)argv;CHECK(argc==2,"case");
    ce_models_tags_unloaded();CHECK(released==0,"empty map release");
    ce_vertex_memory=allocate(2*sizeof(void *));ce_vertex_memory_count=2;
    ce_vertex_memory[0]=allocate(1);ce_vertex_memory[1]=allocate(1);
    ce_multipurpose_bitmaps=allocate(sizeof(void *));ce_multipurpose_bitmap_count=1;
    ce_multipurpose_copies=allocate(2*sizeof(*ce_multipurpose_copies));ce_multipurpose_copy_count=2;
    ce_multipurpose_copies[0].copy=allocate(1);ce_multipurpose_copies[1].copy=allocate(1);
    ce_models_tags_unloaded();CHECK(released==7,"all map resources released exactly once");
    CHECK(!ce_vertex_memory&&!ce_vertex_memory_count&&!ce_multipurpose_bitmaps&&!ce_multipurpose_bitmap_count&&
        !ce_multipurpose_copies&&!ce_multipurpose_copy_count,"map ownership was not reset");
    ce_models_tags_unloaded();CHECK(released==7,"repeated unload");
    ce_multipurpose_copies=allocate(sizeof(*ce_multipurpose_copies));ce_multipurpose_copy_count=1;
    ce_multipurpose_copies[0].copy=allocate(1);
    ce_models_tags_unloaded();CHECK(released==9,"later map's resources released");
    ce_models_tags_unloaded();CHECK(released==9,"later empty unload");
    return 0;
}
