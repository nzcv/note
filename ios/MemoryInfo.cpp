#include "UnityPrefix.h"
#include "Configuration/UnityConfigure.h"

#include "Runtime/Misc/SystemInfo.h"
#include "Runtime/Logging/LogAssert.h"
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/vm_region.h>
#include <sys/sysctl.h>

#include <unistd.h>

#if ENABLE_PROFILER
#if PLATFORM_OSX
#include <libproc.h>
#elif PLATFORM_APPLE_NONDESKTOP
//On iPhone, this gets pulled in dynamically through libproc.dylib
extern "C" int proc_regionfilename(int pid, uint64_t address, void * buffer, uint32_t buffersize);
#endif
#endif

void systeminfo::GetAppCommittedMemoryRegionsSlow(dynamic_array<MemoryRegionInfo> &results)
{
    mach_port_t task = mach_task_self();

    // Iterate all VM regions within valid address range and
    // report these which belong to the Unity proccess
    vm_size_t regionSize = 0;
    int prevUserTag = -1;
    for (vm_address_t queryAddr = MACH_VM_MIN_ADDRESS; queryAddr <= MACH_VM_MAX_ADDRESS; queryAddr += regionSize)
    {
        mach_port_t objectName;
        mach_msg_type_number_t count = VM_REGION_EXTENDED_INFO_COUNT;
        struct vm_region_extended_info info;
        kern_return_t retCode = vm_region_64(task, &queryAddr, &regionSize, VM_REGION_EXTENDED_INFO, (vm_region_info_t)&info, &count, &objectName);
        if (retCode != KERN_SUCCESS)
            break;

        // We don't count all the regions that have shared_mode == SM_EMPTY
        // SM_EMPTY - region that has no kernel page backing such as stack guards,
        // reserved and shared regions
        if (info.share_mode == SM_EMPTY)
            continue;

        // Treat copy-on-write regions with single reference as private
        if ((info.share_mode == SM_COW) && (info.ref_count == 1))
            info.share_mode = SM_PRIVATE;

        MemoryRegionInfo region(kMemMemoryProfiler);
        region.address = static_cast<uint64_t>(queryAddr);
        region.size = regionSize;
        region.residentSize = info.pages_resident * PAGE_SIZE;

        // Deduce memory type based on usage (user_tag) and share mode
        // IOKit & IOAccelerator - device driver memory
        // Any non-private pages - shared memory
        region.type = MemoryRegionInfo::Type::Private;
        if ((info.user_tag == VM_MEMORY_IOKIT) || (info.user_tag == VM_MEMORY_IOACCELERATOR) || (info.user_tag == VM_MEMORY_IOSURFACE))
            region.type = MemoryRegionInfo::Type::Device;
        else if ((info.share_mode != SM_PRIVATE) && (info.share_mode != SM_LARGE_PAGE))
            region.type = MemoryRegionInfo::Type::Shared;

#if ENABLE_PROFILER
        #define VM_USER_TAG_NAME(TagID) \
            case VM_MEMORY_##TagID:     \
                region.name = #TagID;   \
                break;

        // Generate name based on region user-tag
        // These should closly match names shown by Xcode Instruments
        switch (info.user_tag)
        {
            VM_USER_TAG_NAME(MALLOC);
            VM_USER_TAG_NAME(MALLOC_SMALL);
            VM_USER_TAG_NAME(MALLOC_LARGE);
            VM_USER_TAG_NAME(MALLOC_HUGE);
            VM_USER_TAG_NAME(MALLOC_TINY);
            VM_USER_TAG_NAME(MALLOC_LARGE_REUSABLE);
            VM_USER_TAG_NAME(MALLOC_LARGE_REUSED);
            VM_USER_TAG_NAME(ANALYSIS_TOOL);
            VM_USER_TAG_NAME(MALLOC_NANO);
            VM_USER_TAG_NAME(MALLOC_MEDIUM);
            VM_USER_TAG_NAME(STACK);

            VM_USER_TAG_NAME(IOKIT);
            VM_USER_TAG_NAME(IOACCELERATOR);
            VM_USER_TAG_NAME(FOUNDATION);
            VM_USER_TAG_NAME(CORESERVICES);

            VM_USER_TAG_NAME(COREGRAPHICS);
            VM_USER_TAG_NAME(COREGRAPHICS_DATA);
            VM_USER_TAG_NAME(COREGRAPHICS_SHARED);
            VM_USER_TAG_NAME(COREGRAPHICS_FRAMEBUFFERS);
            VM_USER_TAG_NAME(COREGRAPHICS_BACKINGSTORES);

            VM_USER_TAG_NAME(DYLIB);
            VM_USER_TAG_NAME(DYLD_MALLOC);
            VM_USER_TAG_NAME(OBJC_DISPATCHERS);

            VM_USER_TAG_NAME(SHARED_PMAP);
            VM_USER_TAG_NAME(UNSHARED_PMAP);

            VM_USER_TAG_NAME(LAYERKIT);
            VM_USER_TAG_NAME(CGIMAGE);
            VM_USER_TAG_NAME(COREIMAGE);
            VM_USER_TAG_NAME(COREUI);
            VM_USER_TAG_NAME(IOSURFACE);
        }
#undef VM_USER_TAG_NAME
        if (info.user_tag == 0)
        {
            // If we can't classify the region by a user tag,
            // try  to check if it's a mapped file region
            pid_t pid = getpid();
            char regionName[PATH_MAX];
            int ret = proc_regionfilename(pid, queryAddr, regionName, PATH_MAX);
            if (ret > 0)
            {
                regionName[ret] = '\0';
                region.name.assign(regionName, ret);
                region.type = MemoryRegionInfo::Type::Mapped;
            }
        }
#endif // ENABLE_PROFILER

        // Regions can be reported as separate, even when really contiguous -
        // this led to the size of certain objects in the memory profiler reporting
        // a lower native size than expected, eg. Texture2DArrays listing at most
        // a native size of 128MB when they were actually larger.
        // Therefore, if two memory regions are conceptually the same -
        // adjacent, with same flags etc. - merge them.
        //
        // N.B. not sure if this is caused by our allocation setup, or MacOS.
        if (results.empty())
        {
            results.push_back(region);
        }
        else
        {
            auto& prevRegion = results.back();
            if (prevRegion.type == region.type &&
                prevUserTag == info.user_tag &&
                prevRegion.address + prevRegion.size == region.address &&
                (info.user_tag != 0 || prevRegion.name == region.name))
            {
                prevRegion.size += region.size;
            }
            else
            {
                results.push_back(region);
            }
        }

        prevUserTag = info.user_tag;
    }
}
