package p5.ndi;

public class NDIP5Finder implements AutoCloseable {
    /**
     * Holds the reference to the NDIlib_find_instance_t object
     */
    private final long ndiLibFindInstancePointer;

    private NDIP5Source[] previouslyQueriedSources;

    /**
     * Creates a NDIP5Finder instance.
     *
     * @param showLocalSources If NDI sources running on the local machine should be included.
     * @param groups A comma-separated list of groups this {@link NDIP5Finder} should query for.
     * @param extraIps A comma-separated list of IP addresses NDI should additionally query for. See Processing.NDI.Find.h
     */
    public NDIP5Finder(boolean showLocalSources, String groups, String extraIps) {
        // TODO: Implement this forced reference more effectively
        NDIP5.loadLibraries();

        this.ndiLibFindInstancePointer = findCreate(showLocalSources, groups, extraIps);
    }

    /**
     * Creates a {@link NDIP5Finder} instance, with no additional IP addresses queried.
     *
     * @param showLocalSources If NDI sources running on the local machine should be included.
     * @param groups A comma-separated list of groups this {@link NDIP5Finder} should query for.
     */
    public NDIP5Finder(boolean showLocalSources, String groups) {
        this(showLocalSources, groups, null);
    }

    /**
     * Creates a {@link NDIP5Finder} instance, with no additional IP addresses queried or restrictions on queried groups.
     *
     * @param showLocalSources If NDI sources running on the local machine should be included
     */
    public NDIP5Finder(boolean showLocalSources) {
        this(showLocalSources, null, null);
    }

    /**
     * Creates a {@link NDIP5Finder} instance, with no additional IP addresses queried, no restrictions on queried groups,
     * and with sources running on the local machine included.
     */
    public NDIP5Finder() {
        // TODO: Implement this forced reference more effectively
        NDIP5.loadLibraries();

        this.ndiLibFindInstancePointer = findCreateDefaultSettings();
    }

    /**
     * Queries for sources that are currently active and visible to this {@link NDIP5Finder} instance.
     * The {@link NDIP5Source} instances created with this method are only valid until
     * the next NDIP5Finder#getCurrentSources() or {@link NDIP5Finder#close} call.
     *
     * @return An array of {@link NDIP5Source}s with information about each current source.
     */
    public synchronized NDIP5Source[] getCurrentSources() {
        if(previouslyQueriedSources != null) {
            for(NDIP5Source prev : previouslyQueriedSources) {
                prev.close();
            }
        }
        long[] currentSources = findGetCurrentSources(ndiLibFindInstancePointer);
        NDIP5Source[] currentNDIP5Sources = new NDIP5Source[currentSources.length];
        for(int i = 0; i < currentSources.length; i++) {
            currentNDIP5Sources[i] = new NDIP5Source(currentSources[i]);
        }

        this.previouslyQueriedSources = currentNDIP5Sources;

        return currentNDIP5Sources;
    }

    /**
     * Waits until the number of online sources have changed, or until the timeout is reached.
     *
     * @param timeout The timeout for the query in milliseconds, or 0 for no timeout.
     * @return true if the sources have changed, false if the timeout was reached.
     */
    public boolean waitForSources(int timeout) {
        return findWaitForSources(ndiLibFindInstancePointer, timeout);
    }

    @Override
    public void close() {
        if(previouslyQueriedSources != null) {
            for(NDIP5Source prev : previouslyQueriedSources) {
                prev.close();
            }
        }
        // TODO: Auto-clean resources.
        findDestroy(ndiLibFindInstancePointer);
    }

    // Native Methods

    private static native long findCreate(boolean showLocalSources, String groups, String extraIps);
    // Should offer a small performance/memory improvement over using #findCreate with the default values.
    private static native long findCreateDefaultSettings();
    private static native void findDestroy(long structPointer);

    private static native long[] findGetCurrentSources(long structPointer);
    private static native boolean findWaitForSources(long structPointer, int timeout);
}
