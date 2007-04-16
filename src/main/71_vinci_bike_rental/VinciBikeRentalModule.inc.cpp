
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

#include "71_vinci_bike_rental/VinciCustomerSearchAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciCustomerAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciBikeSearchAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciBikeAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciReportsAdminInterfaceElement.h"

#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciRateTableSync.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/VinciAntivolTableSync.h"

#include "71_vinci_bike_rental/VinciAddBike.h"
#include "71_vinci_bike_rental/AddCustomerAction.h"
#include "71_vinci_bike_rental/VinciUpdateCustomerAction.h"
#include "71_vinci_bike_rental/VinciAddGuaranteeAction.h"
#include "71_vinci_bike_rental/RentABikeAction.h"
#include "71_vinci_bike_rental/VinciReturnGuaranteeAction.h"
#include "71_vinci_bike_rental/ReturnABikeAction.h"
#include "71_vinci_bike_rental/VinciUpdateBikeAction.h"

#include "71_vinci_bike_rental/VinciContractPrintRequest.h"
#include "71_vinci_bike_rental/BarCodeInterpretFunction.h"

#include "71_vinci_bike_rental/VinciPrintedContractInterfacePage.h"

#include "71_vinci_bike_rental/BarCodeHTMLFormValueInterfaceElement.h"
