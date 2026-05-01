set(VVW_GEN_SOURCES)
set(VVW_GEN_HEADERS)

list(APPEND VVW_GEN_SOURCES
	src/vvw_gen/worker.cpp
	src/vvw_gen/pools/bitset_allocator.cpp
	src/vvw_gen/pools/segregated_free_list_allocator.cpp
)
list(APPEND VVW_GEN_HEADERS
	src/vvw_gen/worker.hpp
	src/vvw_gen/priority_event_queue.hpp
	src/vvw_gen/priority_event_queue_builder.hpp
	src/vvw_gen/observer_notifier/enum_concepts.hpp
	src/vvw_gen/observer_notifier/notifier.hpp
	src/vvw_gen/observer_notifier/observer.hpp
	src/vvw_gen/observer_notifier/observer_notifier_payload_trait.hpp
	src/vvw_gen/type_erasure/args_storage.hpp
	src/vvw_gen/type_erasure/function_wrappers.hpp
	src/vvw_gen/pools/bitset_allocator.hpp
	src/vvw_gen/pools/segregated_free_list_allocator.hpp
	src/vvw_gen/inputs/inputs_processor.hpp
)

set(VVW_GEN_FILES ${VVW_GEN_SOURCES} ${VVW_GEN_HEADERS})
