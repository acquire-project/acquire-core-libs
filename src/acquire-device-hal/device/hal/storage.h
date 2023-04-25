#ifndef H_ACQUIRE_HAL_STORAGE_V0
#define H_ACQUIRE_HAL_STORAGE_V0

#include "device/kit/storage.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /// Checks that a storage device can be initialize with the given
    /// properties.
    /// @returns True if properties appear valid, otherwise False
    int storage_validate(const struct DeviceManager* system,
                         const struct DeviceIdentifier* identifier,
                         const struct StorageProperties* settings);

    struct Storage* storage_open(const struct DeviceManager* system,
                                 const struct DeviceIdentifier* identifier,
                                 struct StorageProperties* settings);

    enum DeviceStatusCode storage_get(const struct Storage* self,
                                      struct StorageProperties* settings);

    enum DeviceStatusCode storage_get_meta(
      const struct Storage* self,
      struct StoragePropertyMetadata* meta);

    /// @brief Append data in `[beg,end)` to Storage
    /// @param[in] beg The beginning of the packet of frames to write.
    /// @param[in] end The end of the packet of frames to write.
    enum DeviceStatusCode storage_append(struct Storage* self,
                                         const struct VideoFrame* beg,
                                         const struct VideoFrame* end);

    enum DeviceStatusCode storage_close(struct Storage* self);

    enum DeviceState storage_get_state(const struct Storage* self);

#ifdef __cplusplus
}
#endif

#endif // H_ACQUIRE_HAL_STORAGE_V0
