package ndi.stream;

/**
 * An object representing the result of a {@link NDIP5Sender#getTally(int)} call, describing where a {@link NDIP5Sender}
 * feed is currently being displayed.
 */
public class NDIP5Tally {

    private final boolean isOnProgram;
    private final boolean isOnPreview;

    /**
     * Creates a tally object to represent the result of a {@link NDIP5Sender#getTally(int)} call.
     *
     * @param isOnProgram If the current sender is being displayed on program.
     * @param isOnPreview If the current sender is being displayed on preview.
     */
    public NDIP5Tally(boolean isOnProgram, boolean isOnPreview) {
        this.isOnProgram = isOnProgram;
        this.isOnPreview = isOnPreview;
    }

    /**
     * Gets whether the current {@link NDIP5Sender} is being displayed on program.
     *
     * @return true if the sender is being displayed, false if it is not being displayed.
     */
    public boolean isOnProgram() {
        return isOnProgram;
    }

    /**
     * Gets whether the current {@link NDIP5Sender} is being displayed on preview.
     *
     * @return true if the sender is being displayed, false if it is not being displayed.
     */
    public boolean isOnPreview() {
        return isOnPreview;
    }
}
