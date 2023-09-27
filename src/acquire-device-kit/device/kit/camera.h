#ifndef H_ACQUIRE_KIT_CAMERA_V0
#define H_ACQUIRE_KIT_CAMERA_V0

#include "device/props/device.h"
#include "device/props/camera.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct Camera
    {
        struct Device device;
        enum DeviceState state;

        /// Attempts to set the given properties on the given camera.
        /// TODO: this will attempt to set as many properties values
        /// as it can. Some properties (e.g. shape and offset) may be
        /// coupled - these will be either set atomically or in the
        /// case that one value is not compatible with others, not
        /// set at all.
        /// TODO: should we define a force_set? or define a way to clear
        /// a settings cache?
        /// TODO: should we return something to indicate which properties
        /// were set? Should we define a setget?
        enum DeviceStatusCode (*set)(struct Camera*,
                                     struct CameraProperties* settings);
        /// Fills out the given properties from the given camera.
        enum DeviceStatusCode (*get)(const struct Camera*,
                                     struct CameraProperties* settings);
        /// Fills out the given property metadata from the given camera.
        /// TODO: should each call to the same camera instance fill out the
        /// the same values? Or should metadata/capabilities depend on
        /// current property values?
        enum DeviceStatusCode (*get_meta)(const struct Camera*,
                                          struct CameraPropertyMetadata* meta);
        /// The shape of the next frame that the camera will capture based on its current properties.
        enum DeviceStatusCode (*get_shape)(const struct Camera*,
                                           struct ImageShape* shape);
        /// Starts acquiring frames.
        enum DeviceStatusCode (*start)(struct Camera*);
        /// Stops acquiring frames.
        /// TODO: does this wait/block until any pending frames have been acquired?
        enum DeviceStatusCode (*stop)(struct Camera*);

        /// Fire the software trigger if it's enabled.
        enum DeviceStatusCode (*execute_trigger)(struct Camera*);

        /// Gets the next frame from the camera.
        enum DeviceStatusCode (*get_frame)(struct Camera*,
                                           void* im,
                                           size_t* nbytes,
                                           struct ImageInfo* info);
    };

#ifdef __cplusplus
}
#endif

#endif // H_ACQUIRE_KIT_CAMERA_V0
