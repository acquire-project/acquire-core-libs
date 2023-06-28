#ifndef H_ACQUIRE_PROPS_STORAGE_V0
#define H_ACQUIRE_PROPS_STORAGE_V0

#include "components.h"
#include "metadata.h"

#ifdef __cplusplus
extern "C"
{
#endif
    struct DeviceManager;
    struct VideoFrame;

    /// Properties for a storage driver.
    struct StorageProperties
    {
        struct String filename;
        struct String external_metadata_json;
        uint32_t first_frame_id;
        struct PixelScale pixel_scale_um;

        /// Parameters for chunking.
        /// Tile dimensions, width, height, and planes, are in pixels and
        /// determine how to break up frames. Together with
        /// `max_bytes_per_chunk`, they determine the dimensions of a chunk.
        struct storage_properties_chunking_s
        {
            uint64_t max_bytes_per_chunk;
            struct storage_properties_chunking_tile_s
            {
                uint32_t width, height, planes;
            } tile;
        } chunking;

        /// Parameters for multiscale.
        /// `max_layer` is the maximum number of layers to generate, where 0 is
        /// the original resolution, 1 is the first downscale, etc. -1 means to
        /// generate as many layers as will downsample the image to the tile
        /// shape.
        struct storage_properties_multiscale_s
        {
            int16_t max_layer;
        } multiscale;
    };

    struct StoragePropertyMetadata
    {
        /// Metadata for chunking.
        /// Indicates whether chunking is supported, and if so, bounds on the
        /// dimensions of tiles and the maximum number of bytes per chunk.
        struct storage_property_metadata_chunking_s
        {
            uint8_t supported;
            struct Property max_bytes_per_chunk;
            struct storage_property_metadata_chunk_dim_s
            {
                struct Property width, height, planes;
            } tile;
        } chunking;

        struct storage_property_metadata_multiscale_s
        {
            uint8_t supported;
            struct Property max_layer;
        } multiscale;
    };

    /// Initializes StorageProperties, allocating string storage on the heap
    /// and filling out the struct fields.
    /// @returns 0 when `bytes_of_out` is not large enough, otherwise 1.
    /// @param[out] out The constructed StorageProperties object.
    /// @param[in] first_frame_id (unused; aiming for future file rollover
    /// support
    /// @param[in] filename A c-style null-terminated string. The file to create
    ///                     for streaming.
    /// @param[in] bytes_of_filename Number of bytes in the `filename` buffer
    ///                              including the terminating null.
    /// @param[in] metadata A c-style null-terminated string. Metadata string
    ///                     to save along side the created file.
    /// @param[in] bytes_of_metadata Number of bytes in the `metadata` buffer
    ///                              including the terminating null.
    /// @param[in] pixel_scale_um The pixel scale or size in microns.
    int storage_properties_init(struct StorageProperties* out,
                                uint32_t first_frame_id,
                                const char* filename,
                                size_t bytes_of_filename,
                                const char* metadata,
                                size_t bytes_of_metadata,
                                struct PixelScale pixel_scale_um);

    /// Copies contents, reallocating string storage if necessary.
    /// @returns 1 on success, otherwise 0
    /// @param[in,out] dst Must be zero initialized or previously initialized
    ///                    via `storage_properties_init()`
    /// @param[in] src Copied to `dst`
    int storage_properties_copy(struct StorageProperties* dst,
                                const struct StorageProperties* src);

    /// @brief Set the filename string in `out`.
    /// Copies the string into storage owned by the properties struct.
    /// @returns 1 on success, otherwise 0
    /// @param[in,out] out The storage properties to change.
    /// @param[in] filename pointer to the beginning of the filename buffer.
    /// @param[in] bytes_of_filename the number of bytes in the filename buffer.
    ///                              Should include the terminating NULL.
    int storage_properties_set_filename(struct StorageProperties* out,
                                        const char* filename,
                                        size_t bytes_of_filename);

    /// @brief Set the metadata string in `out`.
    /// Copies the string into storage owned by the properties struct.
    /// @returns 1 on success, otherwise 0
    /// @param[in,out] out The storage properties to change.
    /// @param[in] metadata pointer to the beginning of the metadata buffer.
    /// @param[in] bytes_of_filename the number of bytes in the metadata buffer.
    ///                              Should include the terminating NULL.
    int storage_properties_set_external_metadata(struct StorageProperties* out,
                                                 const char* metadata,
                                                 size_t bytes_of_metadata);

    /// @brief Set chunking properties for `out`.
    /// Convenience function to set tiling/chunking properties in a single call.
    /// @returns 1 on success, otherwise 0
    /// @param[in, out] out The storage properties to change.
    /// @param[in] tile_width The width, in px, of a tile.
    /// @param[in] tile_height The height, in px, of a tile.
    /// @param[in] tile_planes The number of @p tile_width x @p tile_height
    ///            planes in a single tile.
    /// @param[in] max_bytes_per_chunk The maximum size, in bytes, of a chunk.
    int storage_properties_set_chunking_props(struct StorageProperties* out,
                                              uint32_t tile_width,
                                              uint32_t tile_height,
                                              uint32_t tile_planes,
                                              uint64_t max_bytes_per_chunk);

    /// @brief Set multiscale properties for `out`.
    /// Convenience function to set multiscale properties in a single call.
    /// @returns 1 on success, otherwise 0
    /// @param[in, out] out The storage properties to change.
    /// @param[in] max_layer The maximum number of multiscale layers to create,
    ///            where 0 is the base layer.
    ///            -1 builds all possible layers, down to the tile size.
    int storage_properties_set_multiscale_props(struct StorageProperties* out,
                                                int16_t max_layer);

    /// Free's allocated string storage.
    void storage_properties_destroy(struct StorageProperties* self);

#ifdef __cplusplus
}
#endif

#endif // H_ACQUIRE_PROPS_STORAGE_V0
