set_source_groups()

set(vehicle_SRCS
Composition.cpp
Composition.hpp
CompositionTableSync.cpp
CompositionTableSync.hpp
CompositionUpdateAction.cpp
CompositionUpdateAction.hpp
CompositionVehicleAddAction.cpp
CompositionVehicleAddAction.hpp
CompositionVehicleRemoveAction.cpp
CompositionVehicleRemoveAction.hpp
CurrentJourney.cpp
CurrentJourney.hpp
GetCompositionsService.cpp
GetCompositionsService.hpp
GetVehiclesService.cpp
GetVehiclesService.hpp
NextStop.cpp
NextStop.hpp
RealTimeVehicleUpdateAction.cpp
RealTimeVehicleUpdateAction.hpp
RollingStock.cpp
RollingStock.hpp
RollingStockAdmin.cpp
RollingStockAdmin.hpp
RollingStocksAdmin.cpp
RollingStocksAdmin.hpp
RollingStockTableSync.cpp
RollingStockTableSync.hpp
RollingStockUpdateAction.cpp
RollingStockUpdateAction.hpp
ServiceComposition.cpp
ServiceComposition.hpp
Vehicle.cpp
Vehicle.hpp
VehicleAdmin.cpp
VehicleAdmin.hpp
VehiclesAdmin.cpp
VehiclesAdmin.hpp
VehicleInformationsService.cpp
VehicleInformationsService.hpp
VehicleModule.cpp
VehicleModule.gen.cpp
VehicleModule.hpp
VehicleModule.inc.cpp
VehicleModuleRegister.cpp
VehiclePosition.cpp
VehiclePosition.hpp
VehiclePositionTableSync.cpp
VehiclePositionTableSync.hpp
VehiclePositionUpdateAction.cpp
VehiclePositionUpdateAction.hpp
VehicleScreen.cpp
VehicleScreen.hpp
VehicleTableSync.cpp
VehicleTableSync.hpp
VehicleUpdateAction.cpp
VehicleUpdateAction.hpp
)

set_source_files_properties(VehicleModule.gen.cpp VehicleModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(38_vehicle ${vehicle_SRCS})

target_link_libraries(38_vehicle
  00_framework
  01_util
  10_db
  34_road
)

install(TARGETS 38_vehicle DESTINATION lib)
