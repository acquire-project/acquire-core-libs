#ifndef H_ACQUIRE_PROPS_CAMERA_V0
#define H_ACQUIRE_PROPS_CAMERA_V0

#include <stdint.h>

#include "components.h"
#include "metadata.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /// @brief Stores the properties of a camera.
    /// @details This can be populated with values from a camera.
    ///          Or can be filled out to request new values a camera should adopt.
    struct CameraProperties
    {
        /// @brief The exposure time of a frame in microseconds.
        /// @details Acquire assumes that exposure is always manually specified by this period of time.
        ///          It does not currently support auto-exposure or durations defined by trigger widths.
        float exposure_time_us;

        float line_interval_us;
        enum Direction readout_direction;

        /// @brief The binning factor, which determines how many pixels in each spatial dimension on the sensor are aggregated to form pixels in an image.
        /// @details For example, if we have a sensor of size 1920x1200 and a binning factor of 2, one image should be 960x600.
        uint8_t binning;

        /// @brief The type of each image pixel or sample.
        enum SampleType pixel_type;

        /// @brief The offset of the active image region of interest on the sensor from its top-left corner.
        /// @details Each value represents a number of aggregated pixels in the frame after binning has been applied.
        struct camera_properties_offset_s
        {
            uint32_t x, y;
        } offset;

        /// @brief The shape or size of the active region of interest on the sensor.
        /// Each value represents a number of aggregated pixels in the frame after binning has been applied.
        struct camera_properties_shape_s
        {
            uint32_t x, y;
        } shape;
        struct camera_properties_input_triggers_s
        {
            struct Trigger acquisition_start, frame_start, exposure;
        } input_triggers;
        struct camera_properties_output_triggers_s
        {
            struct Trigger exposure, frame_start, trigger_wait;
        } output_triggers;
    };

    /// @brief Stores the metadata about camera properties.
    /// @details This is only used to request metadata from a camera, which
    ///          expresses capabilities of the camera and acceptable property values.
    struct CameraPropertyMetadata
    {
        struct Property exposure_time_us;
        struct Property line_interval_us;
        struct Property readout_direction;
        struct Property binning;
        struct camera_properties_metadata_offset_s
        {
            struct Property x, y;
        } offset;
        struct camera_properties_metadata_shape_s
        {
            struct Property x, y;
        } shape;

        /// bit field: bit i is 1 if SampleType(i) is supported, 0 otherwise
        uint64_t supported_pixel_types;

        struct CameraPropertyMetadataDigitalLineMetadata
        {
            /// @brief The number of supported digital IO lines. Must be less than 8.
            uint8_t line_count;

            /// @brief Support describing up to 8 names for use with triggering.
            /// @details name[i] is a short, null terminated string naming line i.
            char names[8][64];
        } digital_lines;

        struct CameraPropertiesTriggerMetadata
        {
            struct camera_properties_metadata_trigger_capabilities_s
            {
                /// @brief Bit x is set if line x can be used as a trigger input.
                uint8_t input;
                /// @brief Bit x is set if line x can be used as a trigger output.
                uint8_t output;
            } acquisition_start, exposure, frame_start;
        } triggers;
    };

#ifdef __cplusplus
}
#endif

#endif // H_ACQUIRE_PROPS_CAMERA_V0
