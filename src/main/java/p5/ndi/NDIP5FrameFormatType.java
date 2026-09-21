package p5.ndi;

/**
 * Represents the scan type of any given frame. Can either represent a progressive frame, a fielded frame, or individual
 * fields of an interlaced frame.
 */
public enum NDIP5FrameFormatType {
    /**
     * A progressive frame
     */
    PROGRESSIVE(1),

    /**
     * A fielded frame with the field 0 being on the even lines and field 1 being on the odd lines
     */
    INTERLEAVED(0),

    /**
     * Individual fields
     */
    FIELD_0(2),
    FIELD_1(3);

    int id;

    NDIP5FrameFormatType(int id) {
        this.id = id;
    }

    /**
     * Queries the {@link NDIP5FrameFormatType} based on an ID value.
     * Throws an {@link IllegalArgumentException} when an unsupported ID is given.
     *
     * @param id The internal ID of the requested frame format type.
     * @return The enum instance of the represented type.
     */
    public static NDIP5FrameFormatType valueOf(int id) {
        if(id == PROGRESSIVE.id) {
            return PROGRESSIVE;
        } else if (id == INTERLEAVED.id) {
            return INTERLEAVED;
        } else if (id == FIELD_0.id) {
            return FIELD_0;
        } else if (id == FIELD_1.id) {
            return FIELD_1;
        } else {
            throw new IllegalArgumentException("Unknown frame format id: " + id);
        }
    }
}
