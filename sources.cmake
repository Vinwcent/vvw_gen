set(VVW_GEN_SOURCES)
set(VVW_GEN_HEADERS)

list(APPEND VVW_GEN_SOURCES
	src/vvw_gen/worker.cpp
)
list(APPEND VVW_GEN_HEADERS
	src/vvw_gen/worker.hpp
	src/vvw_gen/priority_event_queue.hpp
	src/vvw_gen/priority_event_queue_builder.hpp
	src/vvw_gen/type_erasure/args_storage.hpp
	src/vvw_gen/type_erasure/function_wrappers.hpp
)

set(VVW_GEN_FILES ${VVW_GEN_SOURCES} ${VVW_GEN_HEADERS})
