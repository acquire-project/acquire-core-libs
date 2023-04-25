#include "storage.h"
#include "logger.h"
#include "device.manager.h"
#include "driver.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define containerof(P, T, F) ((T*)(((char*)(P)) - offsetof(T, F)))
#define countof(e) (sizeof(e) / sizeof((e)[0]))

#define LOG(...) aq_logger(0, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOGE(...) aq_logger(1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define EXPECT(e, ...)                                                         \
    do {                                                                       \
        if (!(e)) {                                                            \
            LOGE(__VA_ARGS__);                                                 \
            goto Error;                                                        \
        }                                                                      \
    } while (0)
#define CHECK(e) EXPECT(e, #e)
#define CHECK_NOJUMP(e)                                                        \
    do {                                                                       \
        if (!(e)) {                                                            \
            LOGE("Expression evaluated as false:\n\t%s", #e);                  \
        }                                                                      \
    } while (0)
#define CHECK_SILENT(e)                                                        \
    do {                                                                       \
        if (!(e)) {                                                            \
            goto Error;                                                        \
        }                                                                      \
    } while (0)

//
//                  STORAGE
//

int
storage_validate(const struct DeviceManager* system,
                 const struct DeviceIdentifier* identifier,
                 const struct StorageProperties* settings)
{
    int is_ok = 1;
    struct Storage* self = 0;
    {
        struct Device* device = 0;
        EXPECT(identifier->kind == DeviceKind_Storage,
               "Expected a Storage device. Got %s.",
               device_kind_as_string(identifier->kind));
        // Verify that the (driver_id,device_id) correspond to
        // this identifier. Otherwise, a user could set up an inconsistent
        // identifier.
        {
            struct DeviceIdentifier selected = { 0 };
            CHECK(Device_Ok ==
                  device_manager_select(
                    system,
                    identifier->kind,
                    identifier->name,
                    strnlen(identifier->name, sizeof(identifier->name)),
                    &selected));
            CHECK(selected.driver_id == identifier->driver_id &&
                  selected.device_id == identifier->device_id);
        }
        CHECK(Device_Ok ==
              driver_open_device(device_manager_get_driver(system, identifier),
                                 identifier->device_id,
                                 &device));
        EXPECT(device->identifier.kind == DeviceKind_Storage,
               "Expected a Storage device, but a %s device was opened.",
               device_kind_as_string(device->identifier.kind));
        device->identifier = *identifier;
        self = containerof(device, struct Storage, device);
    }
    if (self) {
        self->state = self->set(self, settings);
        CHECK(self->state == DeviceState_Armed);
    }
Finalize:
    storage_close(self);
    return is_ok;
Error:
    is_ok = 0;
    goto Finalize;
}

struct Storage*
storage_open(const struct DeviceManager* system,
             const struct DeviceIdentifier* identifier,
             struct StorageProperties* settings)
{
    struct Storage* self = 0;

    CHECK(identifier);
    CHECK(identifier->kind == DeviceKind_Storage);

    {
        struct Device* device = 0;
        CHECK(Device_Ok ==
              driver_open_device(device_manager_get_driver(system, identifier),
                                 identifier->device_id,
                                 &device));
        self = containerof(device, struct Storage, device);
    }

    if (self) {
        self->state = self->set(self, settings);
        CHECK(self->state == DeviceState_Armed);
        self->state = self->start(self);
        CHECK(self->state == DeviceState_Running);
    }
    return self;
Error:
    storage_close(self);
    return 0;
}

enum DeviceStatusCode
storage_get(const struct Storage* self, struct StorageProperties* settings)
{
    CHECK(self);
    CHECK(self->get);
    self->get(self, settings);
    return Device_Ok;
Error:
    return Device_Err;
}

enum DeviceStatusCode
storage_get_meta(const struct Storage* self,
                 struct StoragePropertyMetadata* meta)
{
    CHECK(self);
    CHECK(self->get_meta);
    self->get_meta(self, meta);
    return Device_Ok;
Error:
    return Device_Err;
}

enum DeviceStatusCode
storage_append(struct Storage* self,
               const struct VideoFrame* beg,
               const struct VideoFrame* end)
{
    CHECK(self);
    CHECK(self->state == DeviceState_Running);
    CHECK(end >= beg);
    if (beg < end) {
        size_t nbytes = (uint8_t*)end - (uint8_t*)beg;
        // FIXME: (nclack) api inconsistency. What happens if we don't consume
        // all bytes?
        self->state = self->append(self, beg, &nbytes);
        CHECK(self->state == DeviceState_Running);
    }
    return Device_Ok;
Error:
    return Device_Err;
}

enum DeviceStatusCode
storage_close(struct Storage* self)
{
    enum DeviceStatusCode ecode = Device_Ok;
    CHECK_SILENT(self);
    self->state = self->stop(self);
    EXPECT(self->state == DeviceState_Armed ||
             self->state == DeviceState_AwaitingConfiguration,
           "Expected Armed or AwaitingConfiguration. Got state: %s.",
           device_state_as_string(self->state));
Finalize:
    if (self) {
        driver_close_device(&self->device);
        self->state = DeviceState_Closed;
    }
    return ecode;
Error:
    ecode = Device_Err;
    goto Finalize;
}

enum DeviceState
storage_get_state(const struct Storage* const self)
{
    CHECK(self);
    return self->state;
Error:
    return DeviceState_Closed;
}
