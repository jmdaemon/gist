# Samples
# Copy testing samples to build directory
set(SAMPLES_DIR samples)
set(SAMPLE_FILES ${SAMPLES_DIR}/gists.p1.json)
file(COPY samples DESTINATION ${CMAKE_BINARY_DIR})
