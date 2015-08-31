set_source_groups()

set(spike_SRCS
SpikeModule.cpp
SpikeModule.gen.cpp

SpikeModuleRegister.cpp
SpikeModule.hpp
SpikeModule.inc.cpp

FwParent.hpp
FwParent.cpp
FwChild1.hpp
FwChild1.cpp
FwChild2.hpp
FwChild2.cpp

)

set_source_files_properties(SpikeModule.gen.cpp SpikeModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(99_spike ${spike_SRCS})

install(TARGETS 99_spike DESTINATION lib)
