#include <fintrf.h>
      subroutine mexfilerequiredapiversion(built_by_rel, target_api_ver)
         integer*4 built_by_rel(1), target_api_ver(1)
         built_by_rel(1) = z'2017a'
         target_api_ver(1) = MX_TARGET_API_VER
      end

